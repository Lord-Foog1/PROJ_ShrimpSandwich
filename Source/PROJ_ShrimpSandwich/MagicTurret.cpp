// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicTurret.h"

// Sets default values
AMagicTurret::AMagicTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMagicTurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagicTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

