#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieTown/Actors/ZombieInteractionPoint.h"
#include "ZombieTown/Interfaces/TargetableInterface.h"
#include "ZombieInteractiveObject.generated.h"

UCLASS()
class ZOMBIETOWN_API AZombieInteractiveObject : public AActor, public ITargetableInterface
{
	GENERATED_BODY()
	
public:
	AZombieInteractiveObject();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int MinNumZombies = 4;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		int CurrentZombies = 0;

	UFUNCTION(BlueprintCallable)
		bool HasEnoughZombies() const { return CurrentZombies >= MinNumZombies;  }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnoughZombiesCallback);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotEnoughZombiesCallback);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionCompleteCallback);

	UPROPERTY(BlueprintReadWrite)
		FEnoughZombiesCallback enoughZombiesCallback;

	UPROPERTY(BlueprintReadWrite)
		FNotEnoughZombiesCallback notEnoughZombiesCallback;

	UPROPERTY(BlueprintReadWrite)
		FInteractionCompleteCallback InteractionCompleteCallback;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool BecomesInActiveOnComplete = false;

	virtual void OnEnoughZombies();
	virtual void OnNotEnoughZombies();
	virtual void OnInteractionComplete();
	virtual bool CanInteract() const;

	AZombieInteractionPoint* GetClosestFreeInteractionPointOrNull(const FVector& location);

	// Call when trying to target.
	FTargetInfo GetTargetInfo(const FVector& targetedFrom);
	// Called when targeted, instigator is the thing doing the targeting. Actor is the actor being targeted.
	void OnTargeted(AActor* instigator, AActor* actor);
	// Called when targeted, instigator is the thing doing the targeting. Actor is the actor being targeted.
	void OnTargetReleased(AActor* instigator, AActor* actor);
	
	UPROPERTY()
	TArray<AActor*> Zombies;

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
	bool wasEverComplete = false;
};
