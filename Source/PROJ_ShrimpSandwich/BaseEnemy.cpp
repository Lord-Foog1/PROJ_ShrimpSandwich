// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; 

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Health = MaxHealth;
		OnRep_Health();
	}
}

void ABaseEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABaseEnemy, Health);
	DOREPLIFETIME(ABaseEnemy, bIsDead);
}

float ABaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, 
	AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead )
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
		Die();
	}
	return ActualDamage;
}

void ABaseEnemy::Die()
{
	if (!HasAuthority() || bIsDead )
	{
		return; 
	}
	
	bIsDead = true;
	OnRep_IsDead(); 
	
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Dead"));
		}
	}
	
	SetLifeSpan(CorpseLifeSpan);
}

void ABaseEnemy::OnRep_Health()
{
	OnHealthChanged(Health, MaxHealth);
}

void ABaseEnemy::OnRep_IsDead()
{
	if (!bIsDead)
	{
		return;
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	
	OnDeath();
}


