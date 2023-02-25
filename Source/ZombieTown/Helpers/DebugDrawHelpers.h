#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING
#define DRAW_STRING(text, drawPos, drawColor, drawDuration, drawShadow, ...)\
DrawDebugString(GetWorld(), drawPos, *FString::Printf(TEXT(text), ##__VA_ARGS__), NULL, drawColor, drawDuration, drawShadow)
#endif
#define SAY(text, drawColor, drawDuration, drawShadow, ...) DRAW_STRING(text, GetCharacter()->GetActorLocation(), drawColor, drawDuration, drawShadow, ##__VA_ARGS__)
#define SAY_E(text, ...) SAY(text, FColor::Red, 1.0, true, ##__VA_ARGS__)
#define SAY_W(text, ...) SAY(text, FColor::Yellow, 1.0, true, ##__VA_ARGS__)
#define SAY_I(text, ...) SAY(text, FColor::White, 1.0, true, ##__VA_ARGS__)
#define SAY_D(text, ...) SAY(text, FColor::Green, 1.0, true, ##__VA_ARGS__)
#define NOTIFY(text, drawColor, drawDuration, drawShadow, ...) DRAW_STRING(text, GetActorLocation(), drawColor, drawDuration, drawShadow, ##__VA_ARGS__)
#define NOTIFY_E(text, ...) NOTIFY(text, FColor::Red, 1.0, true, ##__VA_ARGS__)
#define NOTIFY_W(text, ...) NOTIFY(text, FColor::Yellow, 1.0, true, ##__VA_ARGS__)
#define NOTIFY_I(text, ...) NOTIFY(text, FColor::White, 1.0, true, ##__VA_ARGS__)
#define NOTIFY_D(text, ...) NOTIFY(text, FColor::Green, 1.0, true, ##__VA_ARGS__)
#if UE_BUILD_SHIPPING
#define DRAW_STRING(fmt, drawPos, drawColor, ...);
#endif

void DebugDrawPose(UWorld* world, const FTransform& pose, const float size = 100);