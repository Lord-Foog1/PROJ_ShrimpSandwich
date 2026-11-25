// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MagicType.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyCharacter.generated.h"

#pragma region PREDECLARATIONS
class UPlayerUpgradeComponent;
enum class EUpgradeModifierType : uint8;
class UBoxComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
#pragma endregion

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractPromptUpdated, FText, InteractText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHealthChanged, float, NewHealth, float, InMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerManaChanged, float, NewMana, float, InMaxMana);

UCLASS(abstract, Blueprintable, BlueprintType)
class PROJ_SHRIMPSANDWICH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region COMPONENTS
	
	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* MeleeBoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UPlayerUpgradeComponent* UpgradeComponent;

#pragma endregion
	
public:
	AMyCharacter();
	
	virtual void Tick(float DeltaSeconds) override;

#pragma region GETTERS_SETTERS
	
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetHealthPercent() const {return MaxHealth > 0.f ? Health / MaxHealth : 0.f;}
	
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetManaPercent() const { return MaxMana > 0.f ? CurMana / MaxMana : 0.f; }
	
#pragma endregion	
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(BlueprintAssignable, Category = "Input")
	FOnInteractPromptUpdated OnInteractPromptUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnPlayerHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Player|Magic|Mana")
	FOnPlayerManaChanged OnManaChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Player|Magic|Mana")
	void OnRep_Mana();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Upgrades")
	void Server_UpgradeStat(EUpgradeModifierType StatToUpgrade, float DeltaValue);

	void ApplyUpgradeStat(EUpgradeModifierType StatToUpgrade, float DeltaValue);
	
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void ApplyStatChange();
	
	UFUNCTION()
	void SaveBaseStats() const;
	
protected:
	virtual void BeginPlay() override;
	
#pragma region INPUT_ACTIONS
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
#pragma endregion

#pragma region INPUT_METHODS
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
	
	UFUNCTION(Server, Reliable)
	void SendServerInteract(FHitResult Hit);
#pragma endregion
	
	FHitResult DoInteractionSweep();
	
#pragma region HEALTH
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Stats")
	float MaxHealth = 100.f;
	
	//Ändras bara på server
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Player")
	float Health = 0.f;
	
	UFUNCTION()
	void OnRep_Health();
	
	//Körs bara på server när health når 0 (Downed) 
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void OnHealthDepleted();
	
	UFUNCTION(BlueprintCallable, Category = "Player|Magic")
	void SelectMagicType(EMagicType ChosenMagicType);
	
	UFUNCTION(Reliable, Server)
	void Server_SelectMagicType(EMagicType ChosenMagicType);
	
#pragma endregion
	
private:

#pragma region INPUT_VARIABLES
	
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashSpeed = 4000.0f;
	
	UPROPERTY(EditAnywhere, Category="Sprint")
	float SprintSpeed = 1000.0f;
	
	float WalkSpeed;
	
	UPROPERTY(EditAnywhere, Category ="Dash", meta = (AllowPrivateAccess = true))
	float DashCooldown = 3.0f;
	
	// dash cooldown helper variable
	float LastDashTime = 0.0f;
	
	UPROPERTY(EditAnywhere, Category="Input")
	int InteractionRange = 200;
	
#pragma endregion
	
#pragma region MAGIC_VARIABLES
	
	/** Type of magic the player has */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EMagicType MagicType;
	
#pragma region MANA
	
	/** Cost of a spell */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SpellCost = 20;
	
	/** Max amount of mana a player has (Can be upgraded) */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxMana = 100;
	
	/** The current amount of mana */
	UPROPERTY(ReplicatedUsing = OnRep_Mana, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CurMana;
	
	/** Amount of mana the player gets back after ManaRegenInterval seconds */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ManaRegenAmount = 5;
	
	/** How often the player gets mana back */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ManaRegenInterval = 2.0f;
	
	/** Cost of a charged shot */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Mana", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ChargedSpellCost = 50;
	
#pragma endregion
	
#pragma region SPELL
	
	/** The amount of time between magic shots */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Spell", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SpellCooldown = 1.0f;
	
	/** The amount of time it takes for the charged spell shot to charge up */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Magic|Spell", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ChargeUpTime = 3.0f;

#pragma endregion
#pragma endregion
	
#pragma region MELEE
	
	/** Amount of time between allowed mêlée attacks */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Melee", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MeleeCooldown = 1.0f;
	
	/** Amount of damage dealt by mêlée attack */
	UPROPERTY(Replicated, EditAnywhere, Category = "Player|Melee", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MeleeDamage = 10.0f;
	
#pragma endregion
};