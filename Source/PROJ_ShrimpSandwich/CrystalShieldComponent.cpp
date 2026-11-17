// Fill out your copyright notice in the Description page of Project Settings.


#include "CrystalShieldComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
UCrystalShieldComponent::UCrystalShieldComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

// Replicating: ShieldHealth skickas till Klienten
void UCrystalShieldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCrystalShieldComponent, ShieldHealth);
	
}

// Start: server fyller shield, klienten får värdet
void UCrystalShieldComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const float OldShieldHealth = ShieldHealth;
		ShieldHealth = MaxShieldHealth;
		OnRep_ShieldHealth(OldShieldHealth);
	}

}

//Skölden tar all damage 
float UCrystalShieldComponent::AbsorbDamage(float DamageAmount)
{
	//Ingen sköld? = all skada går vidare till livet
	if (!GetOwner() || !GetOwner()->HasAuthority() || ShieldHealth <= 0.f || DamageAmount <= 0.f)
	{
		return DamageAmount; 
	}
	
	const float OldShieldHealth = ShieldHealth; 
	ShieldHealth = FMath::Max(ShieldHealth - DamageAmount, 0.f);
	OnRep_ShieldHealth(OldShieldHealth);
	
	return 0.f;
}

void UCrystalShieldComponent::OnRep_ShieldHealth(float OldShieldHealth)
{
	OnShieldChanged.Broadcast(ShieldHealth, MaxShieldHealth);
	
	if (OldShieldHealth > 0.f && ShieldHealth <= 0.f)
	{
		OnShieldBroken.Broadcast();
	}
}