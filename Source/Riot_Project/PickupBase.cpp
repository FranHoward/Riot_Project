#include "PickupBase.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APickupBase::Collect()
{
	UE_LOG(LogTemp, Log, TEXT("Pickup collected. Value: %d"), PickupValue);
}

