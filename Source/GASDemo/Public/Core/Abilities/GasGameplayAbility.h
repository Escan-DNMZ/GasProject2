// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASDemo/GASDemo.h"
#include "GasGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGasGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGasGameplayAbility();

	//Default Ability None
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Abilities")
	EGASAbilityInputID AbilityInputId = EGASAbilityInputID::None ;
	
};
