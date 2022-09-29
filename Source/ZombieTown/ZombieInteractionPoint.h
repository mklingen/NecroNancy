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
		bool Attach(AActor* zombie);

	UFUNCTION(BlueprintCallable)
		bool Detach();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachedCallback, AActor*, AttachedActor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetachedCallback, AActor*, AttachedActor);

	UPROPERTY()
		FOnAttachedCallback attachedCallback;

	UPROPERTY()
		FOnDetachedCallback detachedCallback;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		AActor* attachedZombie = nullptr;

};
