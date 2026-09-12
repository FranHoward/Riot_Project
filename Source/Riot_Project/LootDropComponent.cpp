#include "LootDropComponent.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "PickupPool.h"

ULootDropComponent::ULootDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APickupPool* ULootDropComponent::ResolvePickupPool()
{
	if (IsValid(PickupPoolOverride))
	{
		return PickupPoolOverride;
	}

	if (CachedPickupPool.IsValid())
	{
		return CachedPickupPool.Get();
	}

	CachedPickupPool = Cast<APickupPool>(
		UGameplayStatics::GetActorOfClass(this, APickupPool::StaticClass()));
	return CachedPickupPool.Get();
}

int32 ULootDropComponent::DropLoot()
{
	if ((bDropOnlyOnce && bHasDropped) || !GetOwner())
	{
		return 0;
	}

	APickupPool* PickupPool = ResolvePickupPool();
	if (!PickupPool)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("'%s' cannot drop loot because the level has no APickupPool."),
			*GetNameSafe(GetOwner()));
		return 0;
	}

	bHasDropped = true;
	int32 SpawnedCount = 0;

	const FTransform OwnerTransform = GetOwner()->GetActorTransform();
	const FVector DropOrigin = OwnerTransform.TransformPosition(DropOffset);

	for (const FLootDropEntry& Entry : LootEntries)
	{
		if (!Entry.ItemRow.DataTable || Entry.ItemRow.RowName.IsNone()
			|| FMath::FRand() > Entry.DropChance)
		{
			continue;
		}

		for (int32 Index = 0; Index < Entry.Count; ++Index)
		{
			const float Angle = FMath::FRandRange(0.0f, 2.0f * UE_PI);
			const float Radius = ScatterRadius * FMath::Sqrt(FMath::FRand());
			const FVector Scatter(
				FMath::Cos(Angle) * Radius,
				FMath::Sin(Angle) * Radius,
				0.0f);

			const FTransform SpawnTransform(
				GetOwner()->GetActorRotation(),
				DropOrigin + Scatter);

			if (PickupPool->AcquirePickup(Entry.ItemRow, SpawnTransform))
			{
				++SpawnedCount;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("'%s' dropped %d pickup(s)."),
		*GetNameSafe(GetOwner()), SpawnedCount);
	return SpawnedCount;
}

void ULootDropComponent::ResetLoot()
{
	bHasDropped = false;
}
