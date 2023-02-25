// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrintHelper.generated.h"

UCLASS()
class ZOMBIETOWN_API UPrintHelper : public UObject
{
	GENERATED_BODY()
};

#define _LOG_(verbosity, txt, ...) UE_LOG(LogTemp, verbosity, TEXT(txt),  ##__VA_ARGS__);
#define LOGI(txt, ...) _LOG_(Display, txt,  ##__VA_ARGS__);
#define LOGD(txt, ...) _LOG_(Verbose, txt,  ##__VA_ARGS__);
#define LOGW(txt, ...) _LOG_(Warning, txt,  ##__VA_ARGS__);
#define LOGE(txt, ...) _LOG_(Error, txt,  ##__VA_ARGS__);
#define PANIC(txt, ...) _LOG_(Fatal, txt,  ##__VA_ARGS__);