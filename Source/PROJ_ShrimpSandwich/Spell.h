// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicType.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Spell.generated.h"

UCLASS()
class PROJ_SHRIMPSANDWICH_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
#pragma region GETTER_SETTER
	
	UFUNCTION(BlueprintCallable, Category = "Spell")
	void SetDamageAmount(const float NewDamageAmount) { DamageAmount = NewDamageAmount; }
	
	UFUNCTION(BlueprintCallable, Category = "Spell")
	float GetDamageAmount() { return DamageAmount; }
	
	UFUNCTION(BlueprintCallable, Category = "Spell")
	void SetSpellSpeed(float NewSpellSpeed) { SpellSpeed = NewSpellSpeed; }
	
	UFUNCTION(BlueprintCallable, Category = "Spell")
	float GetSpellSpeed() { return SpellSpeed; }

#pragma endregion GETTER_SETTER	

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, Category = "Spell")
	void DoHit(AActor* HitActor, FHitResult HitInfo);
	
	UFUNCTION()
	void DestroyAfterParticles(class UNiagaraComponent* PSystem);
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (ClampMin = 0, UIMin = 0, AllowPrivateAccess), BlueprintReadWrite)
	float DamageAmount = 25;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (ClampMin = 0, UIMin = 0, AllowPrivateAccess), BlueprintReadWrite)
	float SpellSpeed = 1400;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (ClampMin = 0, UIMin = 0, AllowPrivateAccess), BlueprintReadWrite)
	float SpellMaxSpeed = 1400;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (AllowPrivateAccess), BlueprintReadWrite)
	EMagicType SpellType;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (AllowPrivateAccess, ClampMin = 0.1f, UIMin = 0.1f), BlueprintReadWrite)
	float Lifetime = 1.5f;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (AllowPrivateAccess), BlueprintReadWrite)
	class UNiagaraSystem* SpellImpactSystem;
	
	UPROPERTY()
	class UNiagaraComponent* ParticleSystem;
	
	UPROPERTY()
	class USphereComponent* Collider;
	
	UPROPERTY()
	class UProjectileMovementComponent* Movement;
	
	UPROPERTY()
	class UAoeEffectComponent* AOEComponent;
};
