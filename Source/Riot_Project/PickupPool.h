#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "PickupPool.generated.h"

class APickupBase;

/**
 * Pre-spawns generic pickup actors and reuses them with different item rows.
 * Place one instance in a level and call AcquirePickup instead of SpawnActor.
 */
UCLASS(Blueprintable)
class RIOT_PROJECT_API APickupPool : public AActor
{
	GENERATED_BODY()

public:
	APickupPool();

	/** Take an inactive pickup, configure it from ItemRow, and place it in the world. */
	UFUNCTION(BlueprintCallable, Category = "Pickup Pool")
	APickupBase* AcquirePickup(
		const FDataTableRowHandle& ItemRow,
		const FTransform& SpawnTransform);

	/** Return an owned pickup to the inactive list. */
	UFUNCTION(BlueprintCallable, Category = "Pickup Pool")
	void ReleasePickup(APickupBase* Pickup);

	UFUNCTION(BlueprintPure, Category = "Pickup Pool")
	int32 GetAvailableCount() const { return AvailablePickups.Num(); }

	UFUNCTION(BlueprintPure, Category = "Pickup Pool")
	int32 GetTotalCount() const { return AllPickups.Num(); }

protected:
	virtual void BeginPlay() override;

	/** Usually BP_PickupBase, so one Blueprint can handle inventory delivery for every row. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Pool")
	TSubclassOf<APickupBase> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Pool", meta = (ClampMin = "1", UIMin = "1"))
	int32 InitialPoolSize = 20;

	/** If false, AcquirePickup returns null while every pooled instance is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Pool")
	bool bExpandWhenEmpty = true;

private:
	APickupBase* CreatePooledPickup();

	UPROPERTY(Transient)
	TArray<TObjectPtr<APickupBase>> AvailablePickups;

	UPROPERTY(Transient)
	TArray<TObjectPtr<APickupBase>> AllPickups;
};
