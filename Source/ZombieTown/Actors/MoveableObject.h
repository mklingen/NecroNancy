#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieInteractiveObject.h"
#include "MoveableObject.generated.h"

USTRUCT(BlueprintType)
struct FMoveTarget {
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (MakeEditWidget = true))
	FTransform Target;
};

UCLASS()
class ZOMBIETOWN_API AMoveableObject : public AZombieInteractiveObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMoveableObject();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool IsMoving = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FMoveTarget MoveTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MoveTime = 5.0f;

	virtual void OnInteractionComplete() override;
	virtual void OnEnoughZombies() override;
	virtual void OnNotEnoughZombies() override;

	virtual bool CanInteract() const override;

	UFUNCTION(BlueprintCallable)
		bool GetIsDoneMoving() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float timeStartedMoving = -1;

	FTransform transformOnStart;
	FTransform transformOnEnd;
	
	bool doneMoving = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
