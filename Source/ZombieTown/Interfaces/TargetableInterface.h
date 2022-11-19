#pragma once

#include "CoreMinimal.h"
#include "TargetableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTargetableInterface : public UInterface
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FTargetInfo
{
    GENERATED_BODY()
    AActor* targetActor;
    FTransform targetOffset;
};

class ITargetableInterface
{
    GENERATED_BODY()

public:
    // Call when trying to target.
    virtual FTargetInfo GetTargetInfo(const FVector& targetedFrom);
    // If the object can even be targeted.
    virtual bool CanTarget();
    // Called when targeted, instigator is the thing doing the targeting. Actor is the actor being targeted.
    virtual void OnTargeted(AActor* instigator, AActor* actor);
    // Called when targeted, instigator is the thing doing the targeting. Actor is the actor being targeted.
    virtual void OnTargetReleased(AActor* instigator, AActor* actor);
};