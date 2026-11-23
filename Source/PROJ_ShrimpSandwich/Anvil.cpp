// Fill out your copyright notice in the Description page of Project Settings.


#include "Anvil.h"

// Sets default values
AAnvil::AAnvil()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAnvil::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAnvil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

