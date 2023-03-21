// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/AbilityTask_SuccessFailEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UAbilityTask_SuccessFailEvent::UAbilityTask_SuccessFailEvent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	
}
UAbilityTask_SuccessFailEvent* UAbilityTask_SuccessFailEvent::WaitSuccessGameplayEvent(UGameplayAbility* OwningAbility,
                                                                                   FGameplayTag SuccessTag, FGameplayTag FailedTag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce,
                                                                                   bool OnlyMatchExact)
{
	UAbilityTask_SuccessFailEvent* MyObj = NewAbilityTask<UAbilityTask_SuccessFailEvent>(OwningAbility);
	MyObj->SuccessTag = SuccessTag;
	MyObj->FailedTag = FailedTag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;

	return MyObj;
}


// Wait Until Tag Triggerd, Allows success or failure.

void UAbilityTask_SuccessFailEvent::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* UAbilityTask_SuccessFailEvent::GetTargetAsc()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilitySystemComponent;
}

void UAbilityTask_SuccessFailEvent::Activate()
{

	UAbilitySystemComponent* ASC = GetTargetAsc();
	if (ASC)
	{
		if (OnlyMatchExact)
		{
			SuccessHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).AddUObject(this, &UAbilityTask_SuccessFailEvent::SuccessEventCallback);

			FailedHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).AddUObject(this, &UAbilityTask_SuccessFailEvent::FailedEventCallback);
		}
		else
		{
			SuccessHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag),
										FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this,
										&UAbilityTask_SuccessFailEvent::SuccessEventContainerCallback));

			FailedHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag),FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this,
																					&UAbilityTask_SuccessFailEvent::FailedEventContainerCallback));
		}	
	}

	Super::Activate();
}

void UAbilityTask_SuccessFailEvent::SuccessEventCallback(const FGameplayEventData* Payload)
{
	SuccessEventContainerCallback(SuccessTag, Payload);
}

void UAbilityTask_SuccessFailEvent::FailedEventCallback(const FGameplayEventData* Payload)
{
	FailedEventContainerCallback(SuccessTag, Payload);
}

void UAbilityTask_SuccessFailEvent::SuccessEventContainerCallback(FGameplayTag MatchingTag,const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		SuccessEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAbilityTask_SuccessFailEvent::FailedEventContainerCallback(FGameplayTag MatchingTag,const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		FailedEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAbilityTask_SuccessFailEvent::OnDestroy(bool AbilityEnding)
{
	Super::OnDestroy(AbilityEnding);

	UAbilitySystemComponent* ASC = GetTargetAsc();
	if (ASC && SuccessHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).Remove(SuccessHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag), SuccessHandle);
		}
		
	}

	if (ASC && FailedHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).Remove(FailedHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag), FailedHandle);
		}
		
	}
}
