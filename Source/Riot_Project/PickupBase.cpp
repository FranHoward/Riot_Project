#include "PickupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ItemReceiver.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PickupPool.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->InitSphereRadius(80.0f);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(Trigger);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	IdleEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IdleEffect"));
	IdleEffectComponent->SetupAttachment(PickupMesh);
	IdleEffectComponent->SetAutoActivate(false);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::HandleOverlap);
}

void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	// Pool-owned instances begin dormant and receive their item row when acquired.
	if (OwningPool.IsValid())
	{
		DeactivateToPool();
		return;
	}

	if (!InitializeFromItemData())
	{
		UE_LOG(LogTemp, Error,
			TEXT("Pickup '%s' has no valid item data row (table: %s, row: %s)."),
			*GetName(),
			*GetNameSafe(ItemRow.DataTable),
			*ItemRow.RowName.ToString());

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		return;
	}

	bIsPickupActive = true;
}

bool APickupBase::InitializeFromItemData()
{
	bHasValidItemData = false;

	static const FString ContextString(TEXT("APickupBase::InitializeFromItemData"));
	const FItemData* Row = ItemRow.GetRow<FItemData>(ContextString);
	if (!Row)
	{
		return false;
	}

	CachedItemData = *Row;
	PickupValue = CachedItemData.Value;

	UStaticMesh* LoadedMesh = CachedItemData.Mesh.LoadSynchronous();
	PickupMesh->SetStaticMesh(LoadedMesh);
	PickupMesh->SetRelativeScale3D(CachedItemData.MeshScale);

	IdleEffectComponent->Deactivate();
	IdleEffectComponent->SetAsset(nullptr);
	if (UNiagaraSystem* IdleSystem = CachedItemData.IdleEffect.LoadSynchronous())
	{
		IdleEffectComponent->SetAsset(IdleSystem);
		IdleEffectComponent->Activate(true);
	}

	bHasValidItemData = true;
	return true;
}

void APickupBase::Collect()
{
	CollectBy(nullptr);
}

void APickupBase::CollectBy(AActor* Collector)
{
	if (bCollected || !bIsPickupActive)
	{
		return;
	}

	if (!bHasValidItemData && !InitializeFromItemData())
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup '%s' cannot be collected because its item row is invalid."), *GetName());
		return;
	}

	bCollected = true;
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bHandledByReceiver = false;
	if (Collector && Collector->GetClass()->ImplementsInterface(UItemReceiver::StaticClass()))
	{
		bHandledByReceiver = true;
		if (!IItemReceiver::Execute_ReceiveItem(Collector, CachedItemData, this))
		{
			// For example, leave a health pickup available while the player is at full health.
			bCollected = false;
			Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			return;
		}
	}

	if (Collector && !bHandledByReceiver)
	{
		GiveItemToCollector(Collector, CachedItemData);
	}

	if (UNiagaraSystem* CollectSystem = CachedItemData.CollectEffect.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			CollectSystem,
			GetActorLocation(),
			GetActorRotation());
	}

	if (USoundBase* Sound = CachedItemData.CollectSound.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}

	UE_LOG(LogTemp, Log, TEXT("Pickup '%s' collected. Row: %s, Value: %d"),
		*GetName(), *ItemRow.RowName.ToString(), PickupValue);

	if (APickupPool* Pool = OwningPool.Get())
	{
		Pool->ReleasePickup(this);
	}
	else
	{
		Destroy();
	}
}

bool APickupBase::ActivateFromPool(
	const FDataTableRowHandle& NewItemRow,
	const FTransform& SpawnTransform)
{
	// Keep the pickup inert until its transform and visual data are ready.
	DeactivateToPool();

	ItemRow = NewItemRow;
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

	if (!InitializeFromItemData())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Pooled pickup '%s' could not resolve row '%s' from table '%s'."),
			*GetName(),
			*NewItemRow.RowName.ToString(),
			*GetNameSafe(NewItemRow.DataTable));
		return false;
	}

	bCollected = false;
	bIsPickupActive = true;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	return true;
}

void APickupBase::DeactivateToPool()
{
	bCollected = false;
	bIsPickupActive = false;

	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	IdleEffectComponent->Deactivate();
}

void APickupBase::SetOwningPool(APickupPool* Pool)
{
	OwningPool = Pool;
}

void APickupBase::HandleOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		CollectBy(OtherActor);
	}
}

