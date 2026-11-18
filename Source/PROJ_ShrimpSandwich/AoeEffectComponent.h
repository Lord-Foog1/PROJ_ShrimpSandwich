// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AoeEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_SHRIMPSANDWICH_API UAoeEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAoeEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Explode(AActor* HitActor);
	
	UFUNCTION()
	void DoDamage(AActor* DamagedActor);
	
	UFUNCTION()
	void OnExplosion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:	
	
	UPROPERTY()
	class USphereComponent* ExplosionCollider;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess, ClampMin = 0, UIMin = 0), BlueprintReadOnly)
	float ExplosionDamage = 15.0f;
	
	UPROPERTY(meta = (AllowPrivateAccess), BlueprintReadWrite)
	AActor* ShotActor;
};
