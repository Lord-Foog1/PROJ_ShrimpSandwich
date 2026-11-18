// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"

#include "AoeEffectComponent.h"
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
	
	if (!HasAuthority())
	{
		return;
	}
	
	ParticleSystem = GetComponentByClass<UNiagaraComponent>();
	Collider = GetComponentByClass<USphereComponent>();
	Movement = GetComponentByClass<UProjectileMovementComponent>();
	AOEComponent = GetComponentByClass<UAoeEffectComponent>();
	
	if (Collider)
	{
		Collider->SetNotifyRigidBodyCollision(true);
		Collider->OnComponentHit.AddDynamic(this, &ASpell::OnHit);
	}
	
	if (ParticleSystem)
	{
		ParticleSystem->OnSystemFinished.AddDynamic(this, &ASpell::DestroyAfterParticles);
		ParticleSystem->SetNiagaraVariableFloat("User_Lifetime", Lifetime);
	}
	
	if (Movement)
	{
		Movement->InitialSpeed = SpellSpeed;
		Movement->MaxSpeed = SpellMaxSpeed;
	}
	
	// Handles the object after its lifetime is over
	FTimerHandle ParticleTimerHandle;
	TWeakObjectPtr<UNiagaraComponent> WeakParticleSystem = ParticleSystem;
	TWeakObjectPtr<USphereComponent> WeakCollider = Collider;
	TWeakObjectPtr<UProjectileMovementComponent> WeakMovement = Movement;
	GetWorldTimerManager().SetTimer(
		ParticleTimerHandle,
		[WeakParticleSystem, WeakCollider, WeakMovement]()
		{
			if (WeakParticleSystem.IsValid())
			{
				WeakParticleSystem->Deactivate();
			}
			if (WeakCollider.IsValid())
			{
				WeakCollider->Deactivate();
			}
			if (WeakMovement.IsValid())
			{
				WeakMovement->Deactivate();
			}
		},
		Lifetime,
		false
	);
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
	// Checks the authority of the caller of the method
	if (!HasAuthority())
	{
		return;
	}
	
	// Checks that it didn't hit the owner
	if (GetOwner() == HitActor)
	{
		return;
	}

	// Create the impact VFX
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
	
	// Damage hit actor if possible
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
	
	// Creates AOE damage
	AOEComponent->Explode(HitActor);
	
	// Turn off necessary stuff for it to not look weird
	if (ParticleSystem) { ParticleSystem->Deactivate(); }
	if (Collider) 
	{
		Collider->Deactivate();
		Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (Movement) { Movement->Deactivate(); }
}

void ASpell::DestroyAfterParticles(class UNiagaraComponent* PSystem)
{
	this->Destroy();
}
