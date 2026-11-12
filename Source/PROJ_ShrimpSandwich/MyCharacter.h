// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class PROJ_SHRIMPSANDWICH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AMyCharacter();
	
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	UFUNCTION(BlueprintCallable, Category="Damage")
	void TakeDamage(float DamageAmount);
	
protected:
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* DashAction;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category ="Dash")
	float DashCooldown = 3.0f;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoDash();
	
	UFUNCTION(Server, Reliable)
	void Server_DoDash();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoSprint();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void StopSprint();
	
private:
	UPROPERTY(EditAnywhere, Category="Health")
	float PlayerMaxHealth = 100.0f;

	UPROPERTY()
	float CurHealth;
	
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashSpeed = 4000.0f;
	
	UPROPERTY(EditAnywhere, Category="Sprint")
	float SprintSpeed = 1000.0f;
	
	float WalkSpeed;
	
	// dash cooldown helper variable
	float LastDashTime = 0.0f;
};