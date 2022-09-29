#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieInteractionPoint.h"
#include "TargetableInterface.h"
#include "ZombieInteractiveObject.generated.h"

UCLASS()
class ZOMBIETOWN_API AZombieInteractiveObject : public AActor
{
	GENERATED_BODY()
	
public:
	AZombieInteractiveObject();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int MinNumZombies = 4;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		int CurrentZombies = 0;

	UFUNCTION(BlueprintCallable)
		bool HasEnoughZombies() const { return CurrentZombies > MinNumZombies;  }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnoughZombiesCallback);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotEnoughZombiesCallback);

	UPROPERTY(BlueprintReadWrite)
		FEnoughZombiesCallback enoughZombiesCallback;

	UPROPERTY(BlueprintReadWrite)
		FNotEnoughZombiesCallback notEnoughZombiesCallback;

	virtual void OnEnoughZombies();
	virtual void OnNotEnoughZombies();

	AZombieInteractionPoint* GetClosestFreeInteractionPointOrNull(const FVector& location);

	FTargetInfo GetTargetInfo(const FVector& targetedFrom);

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	TArray<AZombieInteractionPoint*> interactionPoints;

	UFUNCTION()
		void OnZombieAttached(AActor* zombie);

	UFUNCTION()
		void OnZombieDetached(AActor* zombie);

	FScriptDelegate onZombieAttachedDelegate;
	FScriptDelegate onZombieDetachedDelegate;
};
