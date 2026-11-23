// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellUpgradeComponent.generated.h"


enum class EUpgradeModifierType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_SHRIMPSANDWICH_API USpellUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellUpgradeComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyUpgrades(EUpgradeModifierType StatToUpgrade, float DeltaValue);

#pragma region MODIFIERS
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Spell")
	float SpellDamageModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Spell")
	float SpellSpeedModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Spell")
	float SpellLifetimeModifier = 1.0f;
	
#pragma endregion MODIFIERS	
	
#pragma region BASE_STATS
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseSpellDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseSpellSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseSpellLifetime;
	
#pragma endregion BASE_STATS	
};
