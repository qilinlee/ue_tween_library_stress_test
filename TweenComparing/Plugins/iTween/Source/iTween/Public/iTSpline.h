//Copyright Jared Therriault and Bob Berkbile 2014

#pragma once

#include "Components/SplineComponent.h"
#include "CoreMinimal.h"

#include "iTSpline.generated.h"

UCLASS()
class AiTSpline : public AActor
{
	GENERATED_BODY()

	AiTSpline(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline")
	USplineComponent* spline; 
};
