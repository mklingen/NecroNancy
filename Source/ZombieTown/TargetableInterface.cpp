#include "TargetableInterface.h"

FTargetInfo ITargetableInterface::GetTargetInfo(const FVector& targetedFrom)
{
	FTargetInfo info;
	info.targetActor = nullptr;
	return info;
}