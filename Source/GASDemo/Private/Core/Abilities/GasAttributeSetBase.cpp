// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GasAttributeSetBase.h"

#include <set>

#include "AbilitySystemComponent.h"
#include "Actors/GasCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UGasAttributeSetBase::UGasAttributeSetBase()
{
}

void UGasAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGasAttributeSetBase, Health);
	DOREPLIFETIME(UGasAttributeSetBase, MaxHealth);
	DOREPLIFETIME(UGasAttributeSetBase, Damage);
}



void UGasAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGasAttributeSetBase,Health,OldValue);
}

void UGasAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGasAttributeSetBase,MaxHealth,OldValue);
}

void UGasAttributeSetBase::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGasAttributeSetBase,Damage,OldValue);
}

void UGasAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health,MaxHealth,NewValue,GetHealthAttribute());
	}
}



void UGasAttributeSetBase::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
													   const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue,NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		//it's calculate damage 
		const float NewDelta = (CurrentMaxValue > 0.f)
										? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
										: NewMaxValue;
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty,EGameplayModOp::Additive,NewDelta);
	}
}
void UGasAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);
	 
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue = 0.f;
	//Additive meaning is when negative 20 it's taking true
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	AGasCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor = nullptr;
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetCharacter = Cast<AGasCharacterBase>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		AActor* SourceActor{nullptr};
		AGasCharacterBase* BaseCharacter{nullptr};
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			AController* SourceController{nullptr};
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();

			if (!SourceController && !SourceActor)
			{
				if (const APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				BaseCharacter = Cast<AGasCharacterBase>(SourceController->GetPawn());
			}
			else
			{
				BaseCharacter = Cast<AGasCharacterBase>(SourceActor);
			}

			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		const float localDamageDone = GetDamage();
		SetDamage(0.f);

		if (localDamageDone > 0)
		{
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - localDamageDone,0.f,GetMaxHealth()));
			
			if (TargetCharacter)
			{
				TargetCharacter->HandleDamage(localDamageDone,HitResult,SourceTags,BaseCharacter,SourceActor);

				TargetCharacter->HandleHealthChanged(-localDamageDone,SourceTags);
			}
		}
		
	} 

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
		if (TargetCharacter)
		{
			TargetCharacter->HandleHealthChanged(DeltaValue,SourceTags);
		}
	}
}
