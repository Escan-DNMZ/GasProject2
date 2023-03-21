// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GasAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



UCLASS()
class GASDEMO_API UGasAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
public:
	UGasAttributeSetBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//When character health changed this function will be start
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Health, Category= "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGasAttributeSetBase,Health);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_MaxHealth, Category= "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGasAttributeSetBase,MaxHealth);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Damage, Category= "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGasAttributeSetBase,Damage);
	
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty);
	
	//İt works every health changed
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);
};
