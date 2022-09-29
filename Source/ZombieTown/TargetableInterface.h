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
    virtual FTargetInfo GetTargetInfo(const FVector& targetedFrom);
};