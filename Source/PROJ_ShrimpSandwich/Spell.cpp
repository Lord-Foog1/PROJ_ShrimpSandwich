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
	
	// if (!HasAuthority())
	// {
	// 	return;
	// }
	
	ParticleSystem = GetComponentByClass<UNiagaraComponent>();
	Collider = GetComponentByClass<USphereComponent>();
	Movement = GetComponentByClass<UProjectileMovementComponent>();
	AOEComponent = GetComponentByClass<UAoeEffectComponent>();
	
	if (Collider)
	{
		Collider->SetNotifyRigidBodyCollision(true);
		Collider->OnComponentHit.AddDynamic(this, &ASpell::OnHit);
		Collider->IgnoreActorWhenMoving(GetInstigator(), true);
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
	// Checks that it didn't hit the owner
	if (GetOwner() == HitActor)
	{
		return;
	}

	// Create the impact VFX
	if (SpellImpactSystem)
	{
		CreateExplosionParticles(HitInfo);
	}
	
	// Damage hit actor if possible
	//EnemySPells skadar bara spelare, så slime inte skadar varandra
	const bool bCanDamage = !bOnlyDamagePlayers || (HitActor && HitActor->IsA<AMyCharacter>());
	
	if (IsValid(HitActor) && HasAuthority())
	{
		DealDamage(HitActor);
	}
	
	//Creates AOE damage bara om spellen har en AOE komponent :p 
	if (AOEComponent)
	{
		AOEComponent->Explode(HitActor);
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

void ASpell::DealDamage(AActor* DamagedActor)
{
	UGameplayStatics::ApplyDamage(
		DamagedActor,
		DamageAmount,
		GetInstigatorController(),
		this,
		nullptr
		);
}

void ASpell::CreateExplosionParticles(FHitResult Hit)
{
	UNiagaraComponent* SpawnedParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		SpellImpactSystem,
		Hit.ImpactPoint,
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
		SpawnedParticle->SetNiagaraVariableVec3(FString(TEXT("User_ImpactNormal")), Hit.ImpactNormal);
		SpawnedParticle->Activate();
	}
}

void ASpell::DestroyAfterParticles(class UNiagaraComponent* PSystem)
{
	this->Destroy();
}
