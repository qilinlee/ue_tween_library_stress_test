//Copyright Jared Therriault and Bob Berkbile 2014

#include "iTSpline.h"

AiTSpline::AiTSpline(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	spline = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("Spline"));

	RootComponent = spline;
}


