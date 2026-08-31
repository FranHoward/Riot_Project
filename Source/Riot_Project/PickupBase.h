// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

UCLASS(Blueprintable)
class RIOT_PROJECT_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:
	APickupBase();

	/** Value awarded when this pickup is collected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	int32 PickupValue = 10;

	/** Called from Blueprint when the pickup is collected. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Collect();
};
