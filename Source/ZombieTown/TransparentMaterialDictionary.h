// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TransparentMaterialDictionary.generated.h"


UCLASS()
class ZOMBIETOWN_API UTransparentMaterialDictionary : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<UMaterialInterface*> Materials;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray< UMaterialInterface*> TransparentMaterials;

	UFUNCTION()
		UMaterialInterface* GetTransparentOfOrNull(UMaterialInterface* material) const;

};
