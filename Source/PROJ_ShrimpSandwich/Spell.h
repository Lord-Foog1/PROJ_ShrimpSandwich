// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (ClampMin = 0, UIMin = 0, AllowPrivateAccess), BlueprintReadWrite)
	float DamageAmount;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (ClampMin = 0, UIMin = 0, AllowPrivateAccess), BlueprintReadWrite)
	float SpellSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Spell", meta = (AllowPrivateAccess), BlueprintReadWrite)
	FString SpellType;
};
