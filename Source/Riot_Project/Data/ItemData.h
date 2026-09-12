#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

class UTexture2D;
class UStaticMesh;
class UNiagaraSystem;
class USoundBase;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable UMETA(DisplayName = "消耗品"),
	Currency   UMETA(DisplayName = "货币"),
	Weapon     UMETA(DisplayName = "武器")
};

USTRUCT(BlueprintType)
struct RIOT_PROJECT_API FItemData : public FTableRowBase
{
	GENERATED_BODY()

	/** 道具显示名称 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	/** 道具类型 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType Type = EItemType::Consumable;

	/** 道具数值，例如恢复量、金币价值或攻击力 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Value = 0;

	/** UI 图标 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UTexture2D> Icon;

	/** 场景中的静态网格 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSoftObjectPtr<UStaticMesh> Mesh;

	/** 道具存在于场景中时持续播放的特效 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	TSoftObjectPtr<UNiagaraSystem> IdleEffect;

	/** 道具被拾取时播放一次的特效 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	TSoftObjectPtr<UNiagaraSystem> CollectEffect;

	/** 道具被拾取时播放的声音 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	TSoftObjectPtr<USoundBase> CollectSound;

	/** 场景模型的缩放，不影响拾取碰撞范围 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World")
	FVector MeshScale = FVector::OneVector;

	/** 道具描述 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;
};
