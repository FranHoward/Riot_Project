#include "PickupPool.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PickupBase.h"

APickupPool::APickupPool()
{
	PrimaryActorTick.bCanEverTick = false;
	PickupClass = APickupBase::StaticClass();
}

void APickupPool::BeginPlay()
{
	Super::BeginPlay();

	for (int32 Index = 0; Index < InitialPoolSize; ++Index)
	{
		if (!CreatePooledPickup())
		{
			UE_LOG(LogTemp, Error,
				TEXT("Pickup pool '%s' stopped prewarming after creating %d of %d actors."),
				*GetName(), Index, InitialPoolSize);
			break;
		}
	}
}

APickupBase* APickupPool::CreatePooledPickup()
{
	if (!PickupClass || !GetWorld())
	{
		return nullptr;
	}

	APickupBase* Pickup = GetWorld()->SpawnActorDeferred<APickupBase>(
		PickupClass,
		FTransform::Identity,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Pickup)
	{
		return nullptr;
	}

	// Configure dormancy before components are registered so the warm-up cannot
	// accidentally overlap a pawn at the world origin.
	Pickup->SetOwningPool(this);
	Pickup->DeactivateToPool();
	UGameplayStatics::FinishSpawningActor(Pickup, FTransform::Identity);
	Pickup->DeactivateToPool();

	AllPickups.Add(Pickup);
	AvailablePickups.Add(Pickup);
	return Pickup;
}

APickupBase* APickupPool::AcquirePickup(
	const FDataTableRowHandle& ItemRow,
	const FTransform& SpawnTransform)
{
	APickupBase* Pickup = nullptr;

	// Ignore actors that may have been destroyed externally while inactive.
	while (!AvailablePickups.IsEmpty() && !IsValid(Pickup))
	{
		Pickup = AvailablePickups.Pop(EAllowShrinking::No);
	}

	if (!IsValid(Pickup) && bExpandWhenEmpty)
	{
		Pickup = CreatePooledPickup();
		if (Pickup)
		{
			AvailablePickups.RemoveSingleSwap(Pickup, EAllowShrinking::No);
		}
	}

	if (!IsValid(Pickup))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup pool '%s' is exhausted."), *GetName());
		return nullptr;
	}

	if (!Pickup->ActivateFromPool(ItemRow, SpawnTransform))
	{
		ReleasePickup(Pickup);
		return nullptr;
	}

	return Pickup;
}

void APickupPool::ReleasePickup(APickupBase* Pickup)
{
	if (!IsValid(Pickup) || !AllPickups.Contains(Pickup))
	{
		return;
	}

	Pickup->DeactivateToPool();
	AvailablePickups.AddUnique(Pickup);
}
