// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicTurret.h"

#include "Spell.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMagicTurret::AMagicTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMagicTurret::BeginPlay()
{
	Super::BeginPlay();
	
	DetectionSphere = GetComponentByClass<USphereComponent>();
	if (!DetectionSphere) UE_LOG(LogTemp, Error, TEXT("SphereComponent Not Found On '%s'"), *GetOwner()->GetName());
	
	SpellSpawnPoint = GetComponentByClass<USceneComponent>();
	if (!SpellSpawnPoint) UE_LOG(LogTemp, Error, TEXT("SpellSpawnPoint Not Found On '%s'"), *GetOwner()->GetName());
	
	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(DetectionRadius);
	}
	
	GetWorldTimerManager().SetTimer(DistanceCheckTimer, this, &AMagicTurret::GetClosestPlayer, DistanceCheckInterval, true);
}

// Called every frame
void AMagicTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Aim();
}

void AMagicTurret::ShootMagic(FTransform SpawnTransform)
{
	UWorld* World = GetWorld();
	
	if (!World || !SpellType) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	World->SpawnActor<ASpell>(
		SpellType,
		SpawnTransform.GetLocation(),
		SpawnTransform.Rotator(),
		SpawnParams
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
		Server_Aim();
		return;
	}
	
	if (!IsValid(ClosestPlayer) || !IsValid(TurretTowerMesh)) return;
	
	const FVector StartLocation = TurretTowerMesh->GetComponentLocation();
	const FVector TargetLocation = ClosestPlayer->GetActorLocation();
	
	FRotator LookAtRotation = (TargetLocation - StartLocation).Rotation();
	LookAtRotation.Pitch += 90.0f;
	
	TurretTowerMesh->SetWorldRotation(LookAtRotation);
}

void AMagicTurret::Server_Aim_Implementation()
{
	Aim();
}

