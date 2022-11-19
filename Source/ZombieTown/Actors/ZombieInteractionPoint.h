#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Delegates/DelegateCombinations.h"
#include "ZombieInteractionPoint.generated.h"

UCLASS()
class ZOMBIETOWN_API AZombieInteractionPoint : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AZombieInteractionPoint();

	UFUNCTION(BlueprintCallable)
		AActor* GetAttachedZombieOrNull() const { return attachedZombie;  }

	UFUNCTION(BlueprintCallable)
		AActor* GetReservedZombieOrNull() const {
		AActor* attached = GetAttachedZombieOrNull();
		if (attached) {
			return attached;
		}
		return reservedZombie;
	}

	UFUNCTION(BlueprintCallable)
		bool Reserve(AActor* zombie);

	UFUNCTION(BlueprintCallable)
		bool UnReserve();

	UFUNCTION(BlueprintCallable)
		bool Attach(AActor* zombie);

	UFUNCTION(BlueprintCallable)
		bool Detach();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachedCallback, AActor*, AttachedActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetachedCallback, AActor*, AttachedActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReservedCallback, AActor*, AttachedActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnreservedCallback, AActor*, AttachedActor);

	UPROPERTY()
		FOnAttachedCallback attachedCallback;

	UPROPERTY()
		FOnDetachedCallback detachedCallback;

	UPROPERTY()
		FOnReservedCallback reservedCallback;

	UPROPERTY()
		FOnUnreservedCallback unReservedCallback;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsActive = true;

	void Deactivate() {
		IsActive = false;
		Detach();
	}

	void Activate() {
		IsActive = true;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		AActor* attachedZombie = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		AActor* reservedZombie = nullptr;

};
