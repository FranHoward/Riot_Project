#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "LootDropComponent.generated.h"

class APickupPool;

USTRUCT(BlueprintType)
struct RIOT_PROJECT_API FLootDropEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot",
		meta = (RowType = "/Script/Riot_Project.ItemData"))
	FDataTableRowHandle ItemRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot",
		meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DropChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "1", UIMin = "1"))
	int32 Count = 1;
};

/** Reusable loot producer for enemies, chests, and other gameplay actors. */
UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class RIOT_PROJECT_API ULootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULootDropComponent();

	/** Roll the configured entries and acquire their pickup actors from the pool. */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	int32 DropLoot();

	/** Allows a reused owner, such as a resettable chest, to drop again. */
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void ResetLoot();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TArray<FLootDropEntry> LootEntries;

	/** Local-space offset from the owning actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	FVector DropOffset = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ScatterRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	bool bDropOnlyOnce = true;

	/** Optional explicit pool; when unset the first APickupPool in the level is used. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Loot")
	TObjectPtr<APickupPool> PickupPoolOverride;

private:
	APickupPool* ResolvePickupPool();

	TWeakObjectPtr<APickupPool> CachedPickupPool;
	bool bHasDropped = false;
};
