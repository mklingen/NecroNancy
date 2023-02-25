#include "DebugDrawHelpers.h"

#include "DrawDebugHelpers.h"

void DebugDrawPose(UWorld* world, const FTransform& pose, const float size)
{
	const FVector p = pose.GetLocation();
	DrawDebugLine(world, p, p + pose.Rotator().RotateVector(FVector::UnitX()) * size, FColor::Red);
	DrawDebugLine(world, p, p + pose.Rotator().RotateVector(FVector::UnitY()) * size, FColor::Green);
	DrawDebugLine(world, p, p + pose.Rotator().RotateVector(FVector::UnitZ()) * size, FColor::Blue);
}