// Fill out your copyright notice in the Description page of Project Settings.
// Elementeffekter vid träff: Eld = tick-skada, Vatten = glid, Natur = långsam (spelare) / stopp (fiender), Vind = knockback.
// Allt bestäms på servern. Tillstånden replikeras så alla datorer visar samma sak och spelarens rörelse inte rycker.
// Läggs på BP_MyCharacter och BP_BaseEnemy.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicType.h"
#include "StatusEffectComponent.generated.h"

//Event för VFX: en effekt startar (true) eller slutar (false) 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEffectChanged, EMagicType, Element, bool, bActive);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_SHRIMPSANDWICH_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//Konstruktor och replikering
	UStatusEffectComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Starta en effekt vid träff (anropas av Spell::DoHit på servern). Strength = 1 nu, senare från essence
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ApplyElementEffect(EMagicType Element, AController* InstigatorController, AActor* DamageCauser, float Strength = 1.f);

	//Immunitet: ägaren påverkas inte av sitt eget element. None = påverkas av allt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	EMagicType ImmuneElement = EMagicType::None;
	
	UPROPERTY(BlueprintAssignable, Category = "Status")
	FOnEffectChanged OnEffectChanged;

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Status|Fire")
	float BurnDamagePerTick = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Fire")
	float BurnTickInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Fire")
	float BurnDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Water")
	float SlipperyFriction = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Water")
	float SlipperyBraking = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Water")
	float SlipperyDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Nature")
	float SlowMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Nature")
	float SlowDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Nature")
	float StopDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Wind")
	float KnockbackForce = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Status|Wind")
	float KnockbackUp = 300.f;

	//Replikerat: ändras BARA på servern OnRep visar/applicerar på alla datorer
	UPROPERTY(ReplicatedUsing = OnRep_IsBurning)
	bool bIsBurning = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsSlippery)
	bool bIsSlippery = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsSlowed)
	bool bIsSlowed = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsStopped)
	bool bIsStopped = false;

	UFUNCTION()
	void OnRep_IsBurning();

	UFUNCTION()
	void OnRep_IsSlippery();

	UFUNCTION()
	void OnRep_IsSlowed();

	UFUNCTION()
	void OnRep_IsStopped();

private:
	//En funktion per effekt (start och slut) 
	void StartBurn(AController* InstigatorController, AActor* DamageCauser, float Strength);
	void BurnTick();
	void StopBurn();

	void StartSlippery();
	void StopSlippery();

	void StartSlow();
	void StopSlow();

	void StartStop();
	void EndStop();

	void ApplyKnockback(AActor* DamageCauser, float Strength);

	//Hjälpare
	bool IsOwnerPlayer() const;
	class UCharacterMovementComponent* GetOwnerMovement() const;
	class UBrainComponent* GetOwnerBrain() const;
	
	FTimerHandle BurnTickTimer;
	FTimerHandle BurnEndTimer;
	FTimerHandle SlipperyTimer;
	FTimerHandle SlowTimer;
	FTimerHandle StopTimer;

	float CurrentBurnDamage = 0.f;
	TWeakObjectPtr<AController> BurnInstigator;
	TWeakObjectPtr<AActor> BurnCauser;

	float DefaultFriction = 8.f;
	float DefaultBraking = 2048.f;
	float DefaultMaxWalkSpeed = 600.f;
};
