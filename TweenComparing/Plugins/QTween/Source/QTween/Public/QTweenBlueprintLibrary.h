#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QTweenBlueprintLibrary.generated.h"


struct FQTweenHandle;


UCLASS(meta=(ScriptName="QTweenLibrary"), MinimalAPI)
class  UQTweenBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
/*
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetAlpha(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetMove(const FQTweenHandle& InHandle);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetRotate(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetScale(const FQTweenHandle& InHandle);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetColor(const FQTweenHandle& InHandle);
*/
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API bool IsValidTween(const FQTweenHandle& InHandle);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle Reset(const FQTweenHandle& InHandle);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle Cancel(const FQTweenHandle& InHandle, UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API bool IsToggled(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetToggle(const FQTweenHandle& InHandle, bool bToggle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API UObject* GetOwner(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetOwner(const FQTweenHandle& InHandle, UObject* InOwner);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetFollow(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetOffset(const FQTweenHandle& InHandle, const FVector& Offset);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetCallback(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetTarget(const FQTweenHandle& InHandle, UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle UpdateNow(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle Pause(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle Resume(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetAxis(const FQTweenHandle& InHandle, const FVector& Axis);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetDelay(const FQTweenHandle& InHandle, float DelayTime);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetEase(const FQTweenHandle& InHandle, EQTweenType EaseType);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetEaseCurve(const FQTweenHandle& InHandle, UCurveFloat* Curve);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetOvershoot(const FQTweenHandle& InHandle, float Over);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetPeriod(const FQTweenHandle& InHandle, float InPeriod);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetScale(const FQTweenHandle& InHandle, float fScale);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetTo(const FQTweenHandle& InHandle, const FVector& InTo);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetFrom(const FQTweenHandle& InHandle, const FVector& InFrom);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetDiff(const FQTweenHandle& InHandle, const FVector& Diff);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetHasInitialized(const FQTweenHandle& InHandle, bool bInitialized);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetPassed(const FQTweenHandle& InHandle, float InPassed);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetTime(const FQTweenHandle& InHandle, float InTime);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetSpeed(const FQTweenHandle& InHandle, float InSpeed);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetRepeat(const FQTweenHandle& InHandle, int32 Repeat);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetLoopType(const FQTweenHandle& InHandle, EQTweenLoopType eLoopType);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetUseEstimatedTime(const FQTweenHandle& InHandle, bool InUseEstimatedTime);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetIgnoreTimeScale(const FQTweenHandle& InHandle, bool bIignoreTimeScale);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetUseFrames(const FQTweenHandle& InHandle, bool InUseFrames);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetUseManualTime(const FQTweenHandle& InHandle, bool InUseManualTime);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetLoopCount(const FQTweenHandle& InHandle, int32 count);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetLoopOnce(const FQTweenHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetLoopClamp(const FQTweenHandle& InHandle, int32 Loops = 0);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetLoopPingPong(const FQTweenHandle& InHandle, int32 Loops = 0);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetPoint(const FQTweenHandle& InHandle, const FVector& Point);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetOnCompleteOnRepeat(const FQTweenHandle& InHandle, bool bIsOn);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetOnCompleteOnStart(const FQTweenHandle& InHandle, bool bIsOn);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetDirection(const FQTweenHandle& InHandle, float InDir);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API FQTweenHandle SetRecursive(const FQTweenHandle& InHandle, bool bUseRecursive);

	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API float TweenOnCurve(const FQTweenHandle& InHandle, float ratioPassed);
	
	UFUNCTION(BlueprintCallable, Category = "QTween")
	static QTWEEN_API void TweenOnCurveVector(const FQTweenHandle& InHandle, float ratioPassed, FVector& outResult);
};
