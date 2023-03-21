// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GasCharacterBase.h"

#include "Components/SphereComponent.h"
#include "Core/Abilities/GasAttributeSetBase.h"
#include "Core/Abilities/GasGameplayAbility.h"
#include "Core/Components/GasAbilitySystemComponent.h"
#include "GASDemo/GASDemo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/GasEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGasCharacterBase::AGasCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAbilitiesInitialized = false;
	
	
	AbilitySystemComponent = CreateDefaultSubobject<UGasAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	//If you want do single player you can full, but minimal more optimal
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UGasAttributeSetBase>(TEXT("AttributeSet"));

	DeadTag = FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead");
	
}

float AGasCharacterBase::GetHealth() const
{
	if (AttributeSet)
	{
		return  AttributeSet->GetHealth();
	}
	return 1.f;
}

float AGasCharacterBase::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return  AttributeSet->GetMaxHealth();
	}
	return 1.f;
}



// Called when the game starts or when spawned
void AGasCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGasCharacterBase::ServerAnimPlay_Implementation()
{
	multiCastAnimPlay();
}

void AGasCharacterBase::multiCastAnimPlay_Implementation()
{
	int AttackAnim = FMath::RandRange(0, AttackAnims.Num() - 1);
	if (AttackAnims[AttackAnim])
	{
		PlayAnimMontage(AttackAnims[AttackAnim]);
		
	}
}

void AGasCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Server GAS init, Server Only

	if (AbilitySystemComponent)
	{
		//We are sending to server avatar and owner information.
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
		AddStartupGameplayAbilities();
	}
}

void AGasCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Client Only

	//We are sending to client avatar and owner information.
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			"EGASAbilityInputID",
			static_cast<int32>(EGASAbilityInputID::Confirm),
			static_cast<int32>(EGASAbilityInputID::Cancel));
		
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,Binds);
	}
}

void AGasCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
	const FGameplayTagContainer& DamageTags, AGasCharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(DamageAmount,HitInfo,DamageTags,InstigatorCharacter,DamageCauser);
}

void AGasCharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue,EventTags);

		if (GetHealth() <= 0)
		{
			AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		}
	}
}


void AGasCharacterBase::HandlePunch_Implementation()
{
	ServerAnimPlay();
	
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &AGasCharacterBase::StartHandle, 0.3f);
	

}

void AGasCharacterBase::StartHandle()
{
	TArray<AActor*> GasEnemy;
	for (FHitResult Hit: WeaponHits)
	{
		GasEnemy.Add(Hit.GetActor());
	}
	
	if (GasEnemy.Num() > 0 )
	{
		for (AActor* Enemy:GasEnemy)
		{
			
			if (Cast<AGasEnemy>(Enemy))
			{
				if (Cast<IAbilitySystemInterface>(Enemy)->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead")))
				{
					continue;
				}
				
				Cast<AGasEnemy>(Enemy)->ApplyPunchForceToCharacter(GetActorLocation(),500.f);
				
				//SendGameplayEventToActor'ün c++ versiyonu
				const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.Hit");
				FGameplayEventData Payload = FGameplayEventData();
				Payload.Instigator = GetInstigator();
				Payload.Target = Enemy;
				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Enemy);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(),Tag,Payload);
			}
		}
	}
	else
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.Hit");
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(),Tag,Payload);
	}
	

}


void AGasCharacterBase::ApplyPunchForceToCharacter(const FVector FromLocation, const float AttackForce)
{
	
	FVector TargetLocation = GetActorLocation();
	FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation,TargetLocation);
	GetCharacterMovement()->Launch(FVector(
		Direction.X * AttackForce,
		Direction.Y * AttackForce,
		abs(Direction.Z + 1) * AttackForce));
	
}

// Called every frame
void AGasCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	
}

// Called to bind functionality to input
void AGasCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
				"Confirm",
				"Cancel",
				"EGASAbilityInputID",
				static_cast<int32>(EGASAbilityInputID::Confirm),
				static_cast<int32>(EGASAbilityInputID::Cancel));
		
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,Binds);
	}
}

UAbilitySystemComponent* AGasCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGasCharacterBase::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent)
	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
		//Grant Abilities, Only on the Server
		for(TSubclassOf<UGasGameplayAbility> StartupAbility:GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				StartupAbility,1,
				static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputId),
			this));
		}

		//Apply Passives
		for(TSubclassOf<UGameplayEffect> PassiveEffect:PassiveGameplayEffect)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			//Created FGameplayEffectSpecHandle, now we can add what we want to add GameplayEffect
			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				PassiveEffect,1,EffectContext);

			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
					AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
						*NewHandle.Data.Get(),AbilitySystemComponent);
			}
		}
		bAbilitiesInitialized = true;
	}
}

void AGasCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGasCharacterBase, AttackAnims);
	
	
}
