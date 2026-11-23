// Fill out your copyright notice in the Description page of Project Settings.


#include "AoeEffectComponent.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAoeEffectComponent::UAoeEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ExplosionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Explosion Collider"));
	ExplosionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionCollider->SetSphereRadius(48);
}

// Called when the game starts
// AoeEffectComponent.cpp
void UAoeEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			ExplosionCollider->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}

	ExplosionCollider->SetGenerateOverlapEvents(true);
	ExplosionCollider->OnComponentBeginOverlap.AddDynamic(this, &UAoeEffectComponent::OnExplosion);
}


// Called every frame
void UAoeEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
 * Enables the collision so that the explosion can damage people
 * 
 * @param HitActor The actor hit by the magic before explosion
 */
void UAoeEffectComponent::Explode(AActor* HitActor)
{
	ShotActor = HitActor;
	
	ExplosionCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

/**
 * Damages the actor caught in explosion
 * 
 * @param DamagedActor Actor who is caught in the explosion
 */
void UAoeEffectComponent::DoDamage(AActor* DamagedActor)
{
	// Check to make sure it is not the player who shot, the bullet or the actor who got shot
	if (GetOwner() == DamagedActor) return;
	if (DamagedActor == ShotActor) return;
	if (DamagedActor == GetOwner()->GetOwner()) return;
	if (!GetOwner()->HasAuthority()) return;
	
	UGameplayStatics::ApplyDamage(
		DamagedActor,
		ExplosionDamage,
		GetOwner()->GetInstigatorController(),
		GetOwner(),
		nullptr);
}

void UAoeEffectComponent::OnExplosion(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}
