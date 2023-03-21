// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GasCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGasCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASDEMO_API IGasCharacterInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION()
	 virtual void ApplyPunchForceToCharacter(const FVector FromLocation, const float AttackForce) = 0;
};
