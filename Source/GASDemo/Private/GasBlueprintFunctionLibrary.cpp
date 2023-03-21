// Fill out your copyright notice in the Description page of Project Settings.


#include "GasBlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


bool UGasBlueprintFunctionLibrary::IsSquashingHit(const FVector Velocity, const float VelocityToKill)
{
	if ((Velocity.IsNormalized() ? Velocity.Z : Velocity.GetSafeNormal().Z) > VelocityToKill)
	{
		return  true;
	}
	return false;
}

//If you don't want to use c++ you can use in blueprint
void UGasBlueprintFunctionLibrary::ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target,
	const float AttackForce)
{
	if (IsValid(Target))
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation,TargetLocation);

		Target->GetCharacterMovement()->Launch(FVector(Direction.X * AttackForce,
			Direction.Y * AttackForce,
			abs(Direction.Z + 1)* AttackForce));
	}
}


void UGasBlueprintFunctionLibrary::OnDeath(ACharacter* Character)
{
	
	
	if (Character)
	{
		if (Character->GetLocalRole() == ROLE_Authority)
		{
			APlayerController* PC = Cast<APlayerController>(Character->GetController());
			Character->DisableInput(PC);
			
			// Character->Destroy();
		}
		
		
	}
	
}


// void UGasBlueprintFunctionLibrary::DelayedDeath_Implementation(AActor* TargetedActor)
// {
// 	OnDeath(Cast<ACharacter>(TargetedActor));
// }
