// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCharacter.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
//#include "SNegativeActionButton.h"
//#include "EditorState/EditorState.h"
#include "Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "Shader/Preshader2.h"
//#include "WorldPartition/ContentBundle/ContentBundleLog.h"
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
	}
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

/**
 * 
 * Listan över vilka variabler som skickas över nätverket.
 * Health replikeras till alla datorer, machealth behövs inte (den ändras aldrig)
 */
void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMyCharacter, Health);
}

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
	OnHealthChanged(Health, MaxHealth);
}

void AMyCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

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

void AMyCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

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

void AMyCharacter::DoDash()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDashTime < DashCooldown)
	{
		return;
	}
    
	LastDashTime = CurrentTime;
    
	if (!HasAuthority())
	{
		Server_DoDash();
		return;
	}
    
	float DashForce = DashSpeed;
	FVector Velocity = GetActorForwardVector() * DashForce;
	
	if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		Velocity /= 3.75f;
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
    
	LaunchCharacter(Velocity, true, false);
}

void AMyCharacter::Server_DoDash_Implementation()
{
	// sends the function from a client to the server variant and repeats DoDash() on server
	DoDash();
}

void AMyCharacter::DoSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

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

	GetWorld()->SweepSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * InteractionRange,
									 GetActorRotation().Quaternion(), ECC_GameTraceChannel1,
									 FCollisionShape::MakeSphere(TraceRadius));
	
	return Hit;
}

void AMyCharacter::SendServerInteract_Implementation(FHitResult Hit)
{
	IInteractable::Execute_Server_OnInteract(Hit.GetActor(), this);
}
