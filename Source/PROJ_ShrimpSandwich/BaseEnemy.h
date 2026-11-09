 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class PROJ_SHRIMPSANDWICH_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,AController* EventInstigator, 
		AActor* DamageCauser) override;  

	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDead() const { return bIsDead; }
	
	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }
	
	UFUNCTION(BlueprintPure, Category = "Enemy") 
	float GetAttackDamage() const { return AttackDamage; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float MaxHealth = 50.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float AttackDamage = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	float CorpseLifeSpan = 3.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Enemy")
	float Health = 0.f; 
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "Enemy")
	bool bIsDead = false;
	
	UFUNCTION()
	void OnRep_Health();
	
	UFUNCTION()
	void OnRep_IsDead();
	
	void Die();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnHealthChanged(float NewHealth, float InMaxHealth);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDeath();
};
	
