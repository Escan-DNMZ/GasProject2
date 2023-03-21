// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GasBlueprintFunctionLibrary.generated.h"

/**
 * 
 */

UCLASS()
class GASDEMO_API UGasBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	static bool IsSquashingHit(const FVector Velocity,const float VelocityToKill = 0.f );
	
	UFUNCTION(BlueprintCallable)
	static  void ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target, const float AttackForce);

	UFUNCTION(BlueprintCallable)
	 void OnDeath(ACharacter* Character);

	
	// UFUNCTION(Server, Reliable)
	// void DelayedDeath(AActor* TargetedActor);




};
