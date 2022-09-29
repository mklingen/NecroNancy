#pragma once

#include "OnLevelExitedInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UOnLevelExitedInterface : public UInterface
{
    GENERATED_BODY()
};

class IOnLevelExitedInterface
{
    GENERATED_BODY()

public:
    virtual void OnLevelExited();
};