// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicTurret.generated.h"

UCLASS()
class PROJ_SHRIMPSANDWICH_API AMagicTurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicTurret();
	
	/**
	 * Creates the magic spell and shoots it
	 * 
	 * @param SpawnTransform The position and direction the magic is shot towards
	 */
	UFUNCTION(BlueprintCallable, Category = "MagicTurret|Shooting")
	void ShootMagic();
	
	/**
	 * Retrieves the players position
	 */
	UFUNCTION(BlueprintCallable, Category = "MagicTurret|Aiming")
	void GetClosestPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "MagicTurret|Aiming")
	void Aim();
	
	UFUNCTION(Reliable, Server, BlueprintCallable, Category = "MagicTurret|Aiming")
	void Server_Aim();
	
	UPROPERTY(ReplicatedUsing = OnRep_TargetTowerRotation)
	FRotator TargetTowerRotation = FRotator(90.f, 0.f, 0.f);

	UFUNCTION()
	void OnRep_TargetTowerRotation();

	void ApplyTowerRotation(const FRotator& NewRotation);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** How big radius the turret should check in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicTurret|Aiming")
	float DetectionRadius = 512.0f;

	/** How long between shots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicTurret|Shooting")
	float ShotDelay = 5.0f;
	
	/** How long it takes for the turret to charge up a shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicTurret|Shooting")
	float ShotChargeTime = 2.0f;
	
	/** What spell the turret shoots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicTurret|Magic")
	TSubclassOf<class ASpell> SpellType = nullptr;
	
	/** How much damage the turret does */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicTurret|Magic")
	float Damage = 15.0f;
	
	UPROPERTY(BlueprintReadWrite, Category = "MagicTurret|Aiming")
	TSubclassOf<class AMyCharacter> PlayerClass = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "MagicTurret|Aiming")
	TObjectPtr<AMyCharacter> ClosestPlayer = nullptr;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "MagicTurret|Aiming")
	TArray<TObjectPtr<AMyCharacter>> AllPlayersInRange;
	
#pragma region COMPONENTS
	
	UPROPERTY()
	class USphereComponent* DetectionSphere = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MagicTurret")
	UStaticMeshComponent* TurretBaseMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MagicTurret")
	UStaticMeshComponent* TurretTowerMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MagicTurret")
	USceneComponent* SpellSpawnPoint = nullptr;
	
#pragma endregion COMPONENTS
	
	FTimerHandle DistanceCheckTimer;
	
	FTimerHandle ShotDelayTimer;
	
	FTimerHandle ShotTimerHandle;
	
	UPROPERTY(EditAnywhere)
	float DistanceCheckInterval = 0.05f;
};
