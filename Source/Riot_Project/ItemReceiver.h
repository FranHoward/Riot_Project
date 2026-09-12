#pragma once

#include "CoreMinimal.h"
#include "Data/ItemData.h"
#include "UObject/Interface.h"
#include "ItemReceiver.generated.h"

UINTERFACE(BlueprintType)
class RIOT_PROJECT_API UItemReceiver : public UInterface
{
	GENERATED_BODY()
};

/** Implemented by actors that can apply the gameplay effect of a collected item. */
class RIOT_PROJECT_API IItemReceiver
{
	GENERATED_BODY()

public:
	/** Return false when the item cannot currently be accepted. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	bool ReceiveItem(const FItemData& ItemData, AActor* SourceActor);
};
