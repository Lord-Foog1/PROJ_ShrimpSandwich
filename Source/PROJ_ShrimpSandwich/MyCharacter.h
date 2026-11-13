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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractPromptUpdated, FText, InteractText);

UCLASS(abstract, Blueprintable, BlueprintType)
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
	
	virtual void Tick(float DeltaSeconds) override;
	
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetHealthPercent() const {return MaxHealth > 0.f ? Health / MaxHealth : 0.f;}
	
	UPROPERTY(BlueprintAssignable, Category = "Input")
	
	FOnInteractPromptUpdated OnInteractPromptUpdated;
	
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
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;
	
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
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoInteract();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Stats")
	float MaxHealth = 100.f;
	
	//Ändras bara på server
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Player")
	float Health = 0.f;
	
	UFUNCTION()
	void OnRep_Health();
	
	//Körs på alla datorer när health ändras 
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void OnHealthChanged(float NewHealth, float InMaxHealth);
	
	//Körs bara på server när health når 0 (Downed) 
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void OnHealthDepleted();
	
	UFUNCTION(Server, Reliable)
	void SendServerInteract(FHitResult Hit);
	
	FHitResult DoInteractionSweep();
	
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
	
	UPROPERTY(EditAnywhere, Category="Input")
	int InteractionRange = 200;
};