// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCharacter.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
//#include "SNegativeActionButton.h"
//#include "EditorState/EditorState.h"
#include "Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "Shader/Preshader2.h"
//#include "WorldPartition/ContentBundle/ContentBundleLog.h"
#include "PlayerUpgradeComponent.h"
#include "UpgradeManager.h"
#include "Net/UnrealNetwork.h"

AMyCharacter::AMyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;
	
	// Configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}


void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Health = MaxHealth;
		OnRep_Health();
		
		CurMana = MaxMana;
		OnRep_Mana();
	}
	
	MeleeBoxComponent = GetComponentByClass<UBoxComponent>();
	UpgradeComponent = GetComponentByClass<UPlayerUpgradeComponent>();
	
	SaveBaseStats();
}

void AMyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	FHitResult Hit = DoInteractionSweep();
	
	if (Hit.GetActor() != nullptr)
	{
		if (Hit.GetActor()->Implements<UInteractable>())
		{
			OnInteractPromptUpdated.Broadcast(IInteractable::Execute_GetInteractPrompt(Hit.GetActor()));
		}
	}
	else
	{
		OnInteractPromptUpdated.Broadcast(FText());
	}
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMyCharacter::LookInput);
		
		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::DoDash);
		
		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMyCharacter::DoSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
		
		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyCharacter::DoInteract);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

#pragma region STATS

/**
 * Listan över vilka variabler som skickas över nätverket.
 * Health replikeras till alla datorer
 */
void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMyCharacter, Health);
	DOREPLIFETIME(AMyCharacter, MaxHealth);
	
	DOREPLIFETIME(AMyCharacter, MagicType);
	DOREPLIFETIME(AMyCharacter, SelectedSpell)
	
	DOREPLIFETIME(AMyCharacter, SpellCost);
	
	DOREPLIFETIME(AMyCharacter, MaxMana);
	DOREPLIFETIME(AMyCharacter, CurMana);
	
	DOREPLIFETIME(AMyCharacter, ManaRegenAmount);
	DOREPLIFETIME(AMyCharacter, ManaRegenInterval);
	
	DOREPLIFETIME(AMyCharacter, ChargedSpellCost);
	
	DOREPLIFETIME(AMyCharacter, SpellCooldown);
	DOREPLIFETIME(AMyCharacter, ChargeUpTime);
	DOREPLIFETIME(AMyCharacter, MeleeCooldown);
	
	DOREPLIFETIME(AMyCharacter, MeleeDamage);
	
	DOREPLIFETIME(AMyCharacter, DashCooldown);
}

void AMyCharacter::OnRep_Mana()
{
	OnManaChanged.Broadcast(CurMana, MaxMana);
}

void AMyCharacter::SelectMagicType(EMagicType ChosenMagicType)
{
	if (!HasAuthority())
	{
		Server_SelectMagicType(ChosenMagicType);
		return;
	}
	
	MagicType = ChosenMagicType;
	SetSpell(MagicType);
}


void AMyCharacter::Server_SelectMagicType_Implementation(EMagicType ChosenMagicType)
{
	SelectMagicType(ChosenMagicType);
}

void AMyCharacter::SetSpell(EMagicType SpellType)
{
	if (!HasAuthority())
	{
		Server_SetSpell(SpellType);
		return;
	}
	
	switch (SpellType)
	{
	case EMagicType::Fire:
		SelectedSpell = FireSpell;
		break;
	case EMagicType::Wind:
		SelectedSpell = WindSpell;
		break;
	case EMagicType::Nature:
		SelectedSpell = NatureSpell;
		break;
	case EMagicType::Water:
		SelectedSpell = WaterSpell;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("No Spell Type Selected!"));
		break;
	}
}

void AMyCharacter::Server_SetSpell_Implementation(EMagicType SpellType)
{
	SetSpell(SpellType);
}

#pragma region UPGRADES

void AMyCharacter::Server_UpgradeStat_Implementation(EUpgradeModifierType StatToUpgrade, float DeltaValue)
{
	ApplyUpgradeStat(StatToUpgrade, DeltaValue);
}

void AMyCharacter::ApplyUpgradeStat(EUpgradeModifierType StatToUpgrade, float DeltaValue)
{
	if (!HasAuthority())
	{
		return;
	}
	
	UpgradeComponent->ApplyUpgrades(StatToUpgrade, DeltaValue);
	ApplyStatChange();
}

void AMyCharacter::ApplyStatChange()
{
	if (!HasAuthority() || !UpgradeComponent) return;

	const float PreviousMaxHealth = MaxHealth;
	MaxHealth = UpgradeComponent->BaseMaxHealth * UpgradeComponent->MaxHealthModifier;

	if (MaxHealth > PreviousMaxHealth)
	{
		Health += (MaxHealth - PreviousMaxHealth);
	}
	else
	{
		Health = FMath::Min(Health, MaxHealth);
	}
	OnRep_Health();

	const int32 PreviousMaxMana = MaxMana;
	MaxMana = FMath::Max(1, FMath::RoundToInt(UpgradeComponent->BaseMaxMana * UpgradeComponent->MaxManaModifier));
	ManaRegenAmount = FMath::Max(0, FMath::RoundToInt(UpgradeComponent->BaseManaRegen * UpgradeComponent->ManaRegenModifier));
	ManaRegenInterval = FMath::Max(0.05f, UpgradeComponent->BaseManaRegenInterval * UpgradeComponent->ManaRegenIntervalModifier);

	if (MaxMana > PreviousMaxMana)
	{
		CurMana += (MaxMana - PreviousMaxMana);
	}
	else
	{
		CurMana = FMath::Min(CurMana, MaxMana);
	}
	OnRep_Mana();

	SpellCost = FMath::Max(0, FMath::RoundToInt(UpgradeComponent->BaseSpellCost * UpgradeComponent->SpellCostModifier));
	SpellCooldown = FMath::Max(0.05f, UpgradeComponent->BaseSpellCooldown * UpgradeComponent->SpellCooldownModifier);

	ChargedSpellCost = FMath::Max(0, FMath::RoundToInt(UpgradeComponent->BaseChargedCost * UpgradeComponent->ChargedCostModifier));
	ChargeUpTime = FMath::Max(0.05f, UpgradeComponent->BaseChargeTime * UpgradeComponent->ChargeTimeModifier);

	MeleeDamage = FMath::Max(0.f, UpgradeComponent->BaseMeleeDamage * UpgradeComponent->MeleeDamageModifier);
	MeleeCooldown = FMath::Max(0.05f, UpgradeComponent->BaseMeleeCooldown * UpgradeComponent->MeleeCooldownModifier);
}

void AMyCharacter::SaveBaseStats() const
{
	UpgradeComponent->BaseMaxHealth = MaxHealth;
	UpgradeComponent->BaseMaxMana = MaxMana;
	
	UpgradeComponent->BaseSpellCost = SpellCost;
	
	UpgradeComponent->BaseManaRegen = ManaRegenAmount;
	UpgradeComponent->BaseManaRegenInterval = ManaRegenInterval;
	
	UpgradeComponent->BaseChargedCost = ChargedSpellCost;
	UpgradeComponent->BaseChargeTime = ChargeUpTime;
	
	UpgradeComponent->BaseMeleeCooldown = MeleeCooldown;
	UpgradeComponent->BaseMeleeDamage = MeleeDamage;
}

#pragma endregion

#pragma endregion

#pragma region HEALTH

/**
 * 
 * Unreals skadefunktion, anropas a apply damage
 * körs bara på server
 */
float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	if (!HasAuthority() || Health <= 0.f)
	{
		return 0.f;
	}
	
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}
	
	Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);
	OnRep_Health();
	
	if (Health <= 0.f)
	{
		OnHealthDepleted();
	}
	
	return ActualDamage;
}

void AMyCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

#pragma endregion

#pragma region MOVEMENT

void AMyCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMyCharacter::DoMove(float Right, float Forward)
{
	
	if (bIsDashing) return;
	
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

#pragma region JUMP

void AMyCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AMyCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

#pragma endregion JUMP

#pragma region DASH

void AMyCharacter::DoDash()
{
	if (!bCanDash || bIsDashing) return;

	const float SafeDuration = FMath::Max(0.05f, DashDuration);

	if (!HasAuthority())
	{
		bCanDash = false;
		bIsDashing = true;

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			SavedGravityScale = MoveComp->GravityScale;
			SavedGroundFriction = MoveComp->GroundFriction;
			SavedBrakingDecelerationWalking = MoveComp->BrakingDecelerationWalking;
			SavedBrakingDecelerationFalling = MoveComp->BrakingDecelerationFalling;

			MoveComp->GravityScale = 0.0f;
			MoveComp->GroundFriction = 0.0f;
			MoveComp->BrakingDecelerationWalking = 0.0f;
			MoveComp->BrakingDecelerationFalling = 0.0f;
			MoveComp->StopMovementImmediately();

			FVector DashDir = MoveComp->GetLastInputVector().GetSafeNormal2D();
			if (DashDir.IsNearlyZero())
			{
				DashDir = GetActorForwardVector().GetSafeNormal2D();
			}

			MoveComp->Velocity = DashDir * (DashDistance / SafeDuration);
		}

		GetWorldTimerManager().SetTimer(
			DashDurationTimerHandle,
			this,
			&AMyCharacter::EndDash,
			SafeDuration,
			false
		);

		if (DashCooldown > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				DashCooldownTimerHandle,
				this,
				&AMyCharacter::ResetDashCooldown,
				DashCooldown,
				false
			);
		}
		else
		{
			bCanDash = true;
		}

		Server_DoDash();
		return;
	}

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	bCanDash = false;
	bIsDashing = true;

	SavedGravityScale = MoveComp->GravityScale;
	SavedGroundFriction = MoveComp->GroundFriction;
	SavedBrakingDecelerationWalking = MoveComp->BrakingDecelerationWalking;
	SavedBrakingDecelerationFalling = MoveComp->BrakingDecelerationFalling;

	MoveComp->GravityScale = 0.0f;
	MoveComp->GroundFriction = 0.0f;
	MoveComp->BrakingDecelerationWalking = 0.0f;
	MoveComp->BrakingDecelerationFalling = 0.0f;
	MoveComp->StopMovementImmediately();

	FVector DashDir = MoveComp->GetLastInputVector().GetSafeNormal2D();
	if (DashDir.IsNearlyZero())
	{
		DashDir = GetActorForwardVector().GetSafeNormal2D();
	}

	const float DashSpeed = DashDistance / SafeDuration;
	MoveComp->Velocity = DashDir * DashSpeed;

	OnDash.Broadcast();

	GetWorldTimerManager().SetTimer(
		DashDurationTimerHandle,
		this,
		&AMyCharacter::EndDash,
		SafeDuration,
		false
	);

	if (DashCooldown > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			DashCooldownTimerHandle,
			this,
			&AMyCharacter::ResetDashCooldown,
			DashCooldown,
			false
		);
	}
	else
	{
		bCanDash = true;
	}
}

void AMyCharacter::Server_DoDash_Implementation()
{
	// sends the function from a client to the server variant and repeats DoDash() on server
	DoDash();
}

void AMyCharacter::EndDash()
{
	const bool bWasFalling = GetCharacterMovement() ? GetCharacterMovement()->IsFalling() : false;
	bIsDashing = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->GravityScale = SavedGravityScale;
		MoveComp->GroundFriction = SavedGroundFriction;
		MoveComp->BrakingDecelerationWalking = SavedBrakingDecelerationWalking;
		MoveComp->BrakingDecelerationFalling = SavedBrakingDecelerationFalling;

		const float VelocityKeepFactor = bWasFalling ? 0.1f : 0.35f;
		FVector CurrentVel = MoveComp->Velocity;
		CurrentVel.X *= VelocityKeepFactor;
		CurrentVel.Y *= VelocityKeepFactor;
		MoveComp->Velocity = CurrentVel;
	}
}

void AMyCharacter::ResetDashCooldown()
{
	bCanDash = true;
}

#pragma endregion DASH

#pragma region SPRINT

void AMyCharacter::DoSprint()
{
	if (!HasAuthority())
	{
		Server_DoSprint();
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyCharacter::Server_DoSprint_Implementation()
{
	DoSprint();
}

void AMyCharacter::StopSprint()
{
	if (!HasAuthority())
	{
		Server_StopSprint();
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::Server_StopSprint_Implementation()
{
	StopSprint();
}

#pragma endregion SPRINT

#pragma endregion

#pragma region CAMERA_MOVEMENT

void AMyCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AMyCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

#pragma endregion

#pragma region INTERACTION

void AMyCharacter::DoInteract()
{
	FHitResult Hit = DoInteractionSweep();

	if (Hit.GetActor() != nullptr)
	{
		if (Hit.GetActor()->Implements<UInteractable>())
		{
			// makes blueprint version available
			IInteractable::Execute_Client_OnInteract(Hit.GetActor(), this);
			SendServerInteract(Hit);
		}
	}
}

FHitResult AMyCharacter::DoInteractionSweep()
{
	FHitResult Hit;
	
	// if box trace
	FVector TraceSize = FVector(100.0f, 100.0f, 100.0f);
	
	// if sphere trace
	float TraceRadius = 20.f;

	GetWorld()->SweepSingleByChannel(Hit, GetFirstPersonCameraComponent()->GetComponentLocation(), GetFirstPersonCameraComponent()->GetComponentLocation() + GetFirstPersonCameraComponent()->GetForwardVector() * InteractionRange,
									 GetActorRotation().Quaternion(), ECC_GameTraceChannel1,
									 FCollisionShape::MakeSphere(TraceRadius));
	
	return Hit;
}

void AMyCharacter::SendServerInteract_Implementation(FHitResult Hit)
{
	IInteractable::Execute_Server_OnInteract(Hit.GetActor(), this);
}

#pragma endregion