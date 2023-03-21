// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "Core/Interfaces/GasCharacterInterface.h"
#include "GasCharacterBase.generated.h"

class UGasGameplayAbility;
class UGasAbilitySystemComponent;
class UGasAttributeSetBase;
class USphereComponent;

UCLASS()
class GASDEMO_API AGasCharacterBase : public ACharacter, public IAbilitySystemInterface,public IGasCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGasCharacterBase();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetMaxHealth() const;

	UPROPERTY(Replicated,EditAnywhere)
	TArray<class UAnimMontage*> AttackAnims;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UFUNCTION(Server, Unreliable)
	void ServerAnimPlay();
	
	UFUNCTION(NetMulticast, Unreliable)
	void multiCastAnimPlay();
	
	
	//Sets the owner of the Pawn every PlayerState owner is controller
	virtual  void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*************
	Ability System
	**************/

	virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void AddStartupGameplayAbilities();
	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Abilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffect;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Abilities")
	TArray<TSubclassOf<UGasGameplayAbility>> GameplayAbilities;

	UPROPERTY()
	uint8 bAbilitiesInitialized:1;

	/*************
	 Health
	**************/
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount,const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
		AGasCharacterBase* InstigatorCharacter, AActor* DamageCauser);

	virtual void HandleDamage(float DamageAmount,const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
		AGasCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

	virtual void HandleHealthChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

	friend UGasAttributeSetBase;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= "Config")
	uint8 bAttackGate:1;


	UFUNCTION(Server,Reliable,BlueprintCallable,Category="Attack")
	void HandlePunch();

	

	UFUNCTION(BlueprintCallable)
virtual  void ApplyPunchForceToCharacter(const FVector FromLocation, const float AttackForce) override;


	void StartHandle();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//TObjectPtr can't do the referance, you can't use &
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UGasAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UGasAttributeSetBase> AttributeSet;
	
	FGameplayTag DeadTag;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FHitResult> WeaponHits;
	
	
	
	
	
};


