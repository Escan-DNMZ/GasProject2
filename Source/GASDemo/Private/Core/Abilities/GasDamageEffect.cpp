// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/GasDamageEffect.h"

#include "GameplayTagsManager.h"
#include "Core/Abilities/GasAttributeSetBase.h"

UGasDamageEffect::UGasDamageEffect()
{
	
	
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UGasAttributeSetBase::GetDamageAttribute();
	DamageInfo.ModifierOp = EGameplayModOp::Override;
	DamageInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(20.f);
	Modifiers.Add(DamageInfo);

	FGameplayEffectCue DamageCue;
	FGameplayTagContainer DamageTagContainer;
	TArray<FString> Tags;
	Tags.Add("GameplayCue.Punched");
	
	UGameplayTagsManager::Get().RequestGameplayTagContainer(Tags,DamageTagContainer);
	DamageCue.GameplayCueTags = DamageTagContainer;
	GameplayCues.Add(DamageCue);
}
