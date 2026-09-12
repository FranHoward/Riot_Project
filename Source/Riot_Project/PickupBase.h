// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UNiagaraComponent;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;
class APickupPool;

UCLASS(Blueprintable)
class RIOT_PROJECT_API APickupBase : public AActor
{
	GENERATED_BODY()

public:
	APickupBase();

	/** Resolve ItemRow and apply its mesh, scale, and persistent visual effect. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool InitializeFromItemData();

	/** Legacy Blueprint entry point. Prefer CollectBy when a collector is available. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Collect();

	/** Give the configured item to Collector, play feedback, and remove this actor. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void CollectBy(AActor* Collector);

	/** Configure and enable an instance acquired from a pickup pool. */
	UFUNCTION(BlueprintCallable, Category = "Pickup|Pool")
	bool ActivateFromPool(const FDataTableRowHandle& NewItemRow, const FTransform& SpawnTransform);

	/** Hide the instance and disable all pickup behavior until it is acquired again. */
	UFUNCTION(BlueprintCallable, Category = "Pickup|Pool")
	void DeactivateToPool();

	/** Assign the pool that owns this instance. Called before deferred spawning finishes. */
	void SetOwningPool(APickupPool* Pool);

protected:
	virtual void BeginPlay() override;

	/** The only per-instance setting needed by a placed or spawned pickup. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup",
		meta = (ExposeOnSpawn = "true", RowType = "/Script/Riot_Project.ItemData"))
	FDataTableRowHandle ItemRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Components")
	TObjectPtr<USphereComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Components")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Components")
	TObjectPtr<UNiagaraComponent> IdleEffectComponent;

	/** Resolved row data. Gameplay systems can read this without loading the table again. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Pickup")
	FItemData CachedItemData;

	/** Kept for existing Blueprints; its value is now populated from ItemRow. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	int32 PickupValue = 0;

	/** Implement once in BP_PickupBase to forward the item to an inventory/pickup component. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "Give Item To Collector"))
	void GiveItemToCollector(AActor* Collector, const FItemData& ItemData);

private:
	UFUNCTION()
	void HandleOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool bHasValidItemData = false;
	bool bCollected = false;
	bool bIsPickupActive = false;

	TWeakObjectPtr<APickupPool> OwningPool;
};
