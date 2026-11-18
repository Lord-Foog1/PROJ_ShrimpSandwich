// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASpell::ASpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
	
	ParticleSystem = GetComponentByClass<UNiagaraComponent>();
	Collider = GetComponentByClass<USphereComponent>();
	Movement = GetComponentByClass<UProjectileMovementComponent>();
	
	if (Collider)
	{
		Collider->SetNotifyRigidBodyCollision(true);
		Collider->OnComponentHit.AddDynamic(this, &ASpell::OnHit);
	}
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	DoHit(OtherActor, Hit);
}

void ASpell::DoHit(AActor* HitActor, FHitResult HitInfo)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (GetOwner() == HitActor)
	{
		return;
	}

	if (SpellImpactSystem)
	{
		UNiagaraComponent* SpawnedParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SpellImpactSystem,
			HitInfo.ImpactPoint,
			FRotator::ZeroRotator,
			FVector(1.0f, 1.0f, 1.0f),
			true,
			false,
			ENCPoolMethod::None,
			true
		);
		
		if (SpawnedParticle)
		{
			SpawnedParticle->SetRenderCustomDepth(true);
			SpawnedParticle->SetCustomDepthStencilValue(1);
			SpawnedParticle->SetNiagaraVariableVec3(FString(TEXT("User_ImpactNormal")), HitInfo.ImpactNormal);
			SpawnedParticle->Activate();
		}
	}
	
	if (IsValid(HitActor))
	{
		UGameplayStatics::ApplyDamage(
			HitActor,
			DamageAmount,
			GetInstigatorController(),
			this,
			nullptr
		);
	}
	
	if (ParticleSystem) { ParticleSystem->Deactivate(); }
	if (Collider) 
	{
		Collider->Deactivate();
		Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (Movement) { Movement->Deactivate(); }
	
	SetCanBeDamaged(false);
	
	if (AoeEffect && GetWorld())
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = GetOwner();
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
		
		GetWorld()->SpawnActor<AActor>(AoeEffect, GetActorTransform(), SpawnInfo);
	}
	
	TWeakObjectPtr<ASpell> WeakThis(this);
	FTimerHandle DamageTimerHandle;
	GetWorldTimerManager().SetTimer(
		DamageTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->SetCanBeDamaged(true);
			}
		},
		0.5f,  
		false  
	);
}
