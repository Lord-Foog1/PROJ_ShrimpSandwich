// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicTurret.h"

#include "Spell.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/MovieSceneTracks/Private/MovieSceneTracksCustomAccessors.h"

// Sets default values
AMagicTurret::AMagicTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMagicTurret::BeginPlay()
{
	Super::BeginPlay();
	
	DetectionSphere = GetComponentByClass<USphereComponent>();
	if (!DetectionSphere) UE_LOG(LogTemp, Error, TEXT("SphereComponent Not Found On '%s'"), *GetOwner()->GetName());
	
	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(DetectionRadius);
	}
	
	ApplyTowerRotation(TargetTowerRotation);
	
	GetWorldTimerManager().SetTimer(DistanceCheckTimer, this, &AMagicTurret::GetClosestPlayer, DistanceCheckInterval, true);
	GetWorldTimerManager().SetTimer(ShotDelayTimer, this, &AMagicTurret::ShootMagic, (ShotDelay + ShotChargeTime), true);
}

// Called every frame
void AMagicTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
	{
		Aim();
	}
	else
	{
		ApplyTowerRotation(TargetTowerRotation);
	}
}

void AMagicTurret::ShootMagic()
{
	if (!IsValid(ClosestPlayer) || !IsValid(SpellSpawnPoint) || !SpellType || !GetWorld() || !bCanShot)
	{
		return;
	}
	
	const FTransform SpawnTransform = SpellSpawnPoint->GetComponentTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	TWeakObjectPtr<AMagicTurret> WeakThis(this);
	TSubclassOf<ASpell> SpellClassToSpawn = SpellType;

	GetWorldTimerManager().SetTimer(
		ShotTimerHandle,
		[WeakThis, SpellClassToSpawn, SpawnTransform, SpawnParams]()
		{
			if (!WeakThis.IsValid() || !SpellClassToSpawn)
			{
				return;
			}

			UWorld* TimerWorld = WeakThis->GetWorld();
			if (!IsValid(TimerWorld))
			{
				return;
			}

			TimerWorld->SpawnActor<ASpell>(
				SpellClassToSpawn,
				SpawnTransform.GetLocation(),
				SpawnTransform.Rotator(),
				SpawnParams
			);
		},
		ShotDelay,
		false
	);
}

void AMagicTurret::GetClosestPlayer()
{
	if (AllPlayersInRange.Num() <= 0) return;

	float MinDistance = TNumericLimits<float>::Max();
	const FVector TurretLocation = GetActorLocation();

	for (const TObjectPtr<AMyCharacter>& Player : AllPlayersInRange)
	{
		if (!IsValid(Player)) continue;

		const float Distance = FVector::Dist(TurretLocation, Player->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestPlayer = Player;
		}
	}
}

void AMagicTurret::Aim()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(ClosestPlayer) || !IsValid(TurretTowerMesh))
	{
		return;
	}

	const FVector StartLocation = TurretTowerMesh->GetComponentLocation();
	const FVector TargetLocation = ClosestPlayer->GetActorLocation();

	FRotator LookAtRotation = (TargetLocation - StartLocation).Rotation();
	LookAtRotation.Pitch += 90.0f;

	TargetTowerRotation = LookAtRotation;
	ApplyTowerRotation(TargetTowerRotation);
}

void AMagicTurret::OnRep_TargetTowerRotation()
{
	ApplyTowerRotation(TargetTowerRotation);
}

void AMagicTurret::ApplyTowerRotation(const FRotator& NewRotation)
{
	if (IsValid(TurretTowerMesh))
	{
		TurretTowerMesh->SetWorldRotation(NewRotation);
	}
}

void AMagicTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMagicTurret, TargetTowerRotation);
	DOREPLIFETIME(AMagicTurret, AllPlayersInRange);
}

void AMagicTurret::Server_Aim_Implementation()
{
	Aim();
}

