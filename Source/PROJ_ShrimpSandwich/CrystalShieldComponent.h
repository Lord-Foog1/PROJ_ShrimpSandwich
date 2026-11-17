// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrystalShieldComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChanged, float, NewShieldHealth, float, MaxShieldHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBroken);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_SHRIMPSANDWICH_API UCrystalShieldComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UCrystalShieldComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	float AbsorbDamage(float DamageAmount);

	//Getters för BP
	UFUNCTION(BlueprintPure, Category = "Shield")
	bool HasShield() const { return ShieldHealth > 0.f; }
	
	UFUNCTION(BlueprintPure, Category = "Shield")
	float GetShieldPercent() const { return MaxShieldHealth > 0.f ? ShieldHealth / MaxShieldHealth : 0.f; }
	
	//Händelser för BP (Körs på alla datorer)
	UPROPERTY(BlueprintAssignable, Category = "Shield")
	FOnShieldChanged OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Shield")
	FOnShieldBroken OnShieldBroken;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	float MaxShieldHealth = 20.f;
	
	//Ändras bara på server
	UPROPERTY(ReplicatedUsing=OnRep_ShieldHealth, BlueprintReadOnly, Category = "Shield")
	float ShieldHealth = 0.f;
	
	UFUNCTION()
	void OnRep_ShieldHealth(float OldShieldHealth);
}; 


