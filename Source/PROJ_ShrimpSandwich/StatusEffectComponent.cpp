// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffectComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "MyCharacter.h"
#include "BaseEnemy.h"


//Komponent replikeras
UStatusEffectComponent::UStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UStatusEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStatusEffectComponent, bIsBurning);
	DOREPLIFETIME(UStatusEffectComponent, bIsSlippery);
	DOREPLIFETIME(UStatusEffectComponent, bIsSlowed);
	DOREPLIFETIME(UStatusEffectComponent, bIsStopped);
}

void UStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	//Spara rörelsens normalvärden (på alla datorer)
	if (UCharacterMovementComponent* Move = GetOwnerMovement())
	{
		DefaultFriction = Move->GroundFriction;
		DefaultBraking = Move->BrakingDecelerationWalking;
		DefaultMaxWalkSpeed = Move->MaxWalkSpeed;
	}
	
}

//startar effekt: Bara server, kollar immunitet och väljer effekt efter element
void UStatusEffectComponent::ApplyElementEffect(EMagicType Element, AController* InstigatorController, AActor* DamageCauser, float Strength)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	//Ingen elemetn eller samma element som ägare = ingen effekt 
	if (Element == EMagicType::None || Element == ImmuneElement)
	{
		return;
	}
	
	switch (Element)
	{
		case EMagicType::Fire:
			StartBurn(InstigatorController, DamageCauser, Strength);
			break;
				
		case EMagicType::Water:
			StartSlippery();
			break;
		
			
		case EMagicType::Nature:
		//Spelare saktas ner, enemues stannar
		if (IsOwnerPlayer())
		{
			StartSlow();
		}
		else
		{
			StartStop();
		}
		break;
			
		case EMagicType::Wind:
		ApplyKnockback(DamageCauser, Strength);
		break;
			
		default:
			break;
	}
}



void UStatusEffectComponent::StartBurn(AController* InstigatorController, AActor* DamageCauser, float Strength)
{
	CurrentBurnDamage = BurnDamagePerTick * Strength;
	BurnInstigator = InstigatorController;
	BurnCauser = DamageCauser;
	
	FTimerManager& Timers = GetWorld()->GetTimerManager();
	
	//Börja brinner (bara om det inte redan brinner)
	if (!bIsBurning)
	{
		bIsBurning = true;
		OnRep_IsBurning();
		Timers.SetTimer(BurnTickTimer, this, &UStatusEffectComponent::BurnTick, BurnTickInterval, true);
		
	}
	
	//Starta om sluttiden varje träff
	Timers.SetTimer(BurnEndTimer, this, &UStatusEffectComponent::StopBurn, BurnDuration, false);
}

void UStatusEffectComponent::BurnTick()
{
	if (GetOwner())
	{
		UGameplayStatics::ApplyDamage(GetOwner(), CurrentBurnDamage, BurnInstigator.Get(), BurnCauser.Get(), nullptr);
	}
}

void UStatusEffectComponent::StopBurn()
{
	GetWorld()->GetTimerManager().ClearTimer(BurnTickTimer);
	GetWorld()->GetTimerManager().ClearTimer(BurnEndTimer);
	bIsBurning = false;
	OnRep_IsBurning();
}

//Vatten: Lägre friktion och broms en stund (slide)
void UStatusEffectComponent::StartSlippery()
{
	if (!bIsSlippery)
	{
		bIsSlippery = true;
		OnRep_IsSlippery();
	}
	GetWorld()->GetTimerManager().SetTimer(SlipperyTimer, this, &UStatusEffectComponent::StopSlippery, SlipperyDuration, false);
}

void UStatusEffectComponent::StopSlippery()
{
	bIsSlippery = false;
	OnRep_IsSlippery();
}

//Natur på spelaren: Lägre maxfart en stund
void UStatusEffectComponent::StartSlow()
{
	if (!bIsSlowed)
	{
		bIsSlowed = true;
		OnRep_IsSlowed();
	}
	GetWorld()->GetTimerManager().SetTimer(SlowTimer, this, &UStatusEffectComponent::StopSlow, SlowDuration, false);
}

void UStatusEffectComponent::StopSlow()
{
	bIsSlowed = false;
	OnRep_IsSlowed();
}

// Natur på enemies; pausa AI:n och stoppa rörelsen en stund
void UStatusEffectComponent::StartStop()
{
	if (!bIsStopped)
	{
		if (UBrainComponent* Brain = GetOwnerBrain())
		{
			Brain->PauseLogic(TEXT("Nature"));
		}
		if (UCharacterMovementComponent* Move = GetOwnerMovement())
		{
			Move->StopMovementImmediately();
		}
		bIsStopped = true;
		OnRep_IsStopped();
	}
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &UStatusEffectComponent::EndStop, StopDuration, false);
}

void UStatusEffectComponent::EndStop()
{
	bIsStopped = false;
	OnRep_IsStopped();
	
	//Starta inte AIn igen om fienden hann dö
	if (const ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetOwner()))
	{
		if (Enemy->IsDead())
		{
			return;
		}
	}
	
	if (UBrainComponent* Brain = GetOwnerBrain())
	{
		Brain->ResumeLogic(TEXT("Nature"));
	}
}

//Vind: knockback från träffpunkten + lite uppåt 
void UStatusEffectComponent::ApplyKnockback(AActor* DamageCauser, float Strength)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}
	
	FVector Direction = DamageCauser
		?Character->GetActorLocation() - DamageCauser->GetActorLocation()
		: -Character->GetActorForwardVector();
	Direction.Z = 0.f; 
	Direction = Direction.GetSafeNormal();
	
	const FVector LaunchVelocity = Direction * KnockbackForce* Strength + FVector(0.f, 0.f, KnockbackUp);
	Character->LaunchCharacter(LaunchVelocity, true, true); 
	
}

//OnRep: Körs på alla datorer, movements effekten ändras här så spelarens egna dator stämmer med server
void UStatusEffectComponent::OnRep_IsBurning()
{
	OnEffectChanged.Broadcast(EMagicType::Fire, bIsBurning);
}

void UStatusEffectComponent::OnRep_IsSlippery()
{
	if (UCharacterMovementComponent* Move = GetOwnerMovement())
	{
		Move->GroundFriction = bIsSlippery ? SlipperyFriction : DefaultFriction;
		Move->BrakingDecelerationWalking = bIsSlippery ? SlipperyBraking : DefaultBraking;
	}
	OnEffectChanged.Broadcast(EMagicType::Water, bIsSlippery);
}

void UStatusEffectComponent::OnRep_IsSlowed()
{
	if (UCharacterMovementComponent* Move = GetOwnerMovement())
	{
		Move->MaxWalkSpeed = bIsSlowed ? DefaultMaxWalkSpeed * SlowMultiplier : DefaultMaxWalkSpeed;
		
	}
	OnEffectChanged.Broadcast(EMagicType::Nature, bIsSlowed);
}

void UStatusEffectComponent::OnRep_IsStopped()
{
	OnEffectChanged.Broadcast(EMagicType::Nature, bIsStopped);
}

//Hjälparen 
bool UStatusEffectComponent::IsOwnerPlayer() const
{
	return GetOwner() && GetOwner()->IsA<AMyCharacter>();
}

UCharacterMovementComponent* UStatusEffectComponent::GetOwnerMovement() const
{
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		return Character->GetCharacterMovement();
	}
	return nullptr;
}

UBrainComponent* UStatusEffectComponent::GetOwnerBrain() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const AAIController* AI = Cast<AAIController>(Pawn->GetController()))
		{
			return AI->GetBrainComponent();
		}
	}
	return nullptr;
}


