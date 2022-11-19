#include "TargetableInterface.h"

FTargetInfo ITargetableInterface::GetTargetInfo(const FVector& targetedFrom)
{
	FTargetInfo info;
	info.targetActor = nullptr;
	return info;
}

bool ITargetableInterface::CanTarget()
{
	 return true;
}

void ITargetableInterface::OnTargeted(AActor* instigator, AActor* actor)
{

}

void ITargetableInterface::OnTargetReleased(AActor* instigator, AActor* actor)
{
}
