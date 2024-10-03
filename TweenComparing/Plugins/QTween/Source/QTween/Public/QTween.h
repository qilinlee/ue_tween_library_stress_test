#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "Curves/CurveFloat.h"
#include "QTweenEasing.h"
#include "QTween.generated.h"


UENUM(BlueprintType)
enum class EQTweenAction : uint8
{
	ALPHA,
	COLOR,
	CALLBACK_COLOR,
	CALL_BACK,
	MOVE,
	ROTATE,
	SCALE,
	FOLLOW,
	NONE
};

UENUM(BlueprintType)
enum class EQTweenType : uint8
{
	NotUsed,
	Linear,
	EaseInQuad = (1 << 2 | 1),
	EaseOutQuad = (1 << 2 | 2),
	EaseInOutQuad = (1 << 2 | 3),
	EaseInCubic = (2 << 2 | 1),
	EaseOutCubic = (2 << 2 | 2),
	EaseInOutCubic = (2 << 2 | 3),
	EaseInQuart = (3 << 2 | 1),
	EaseOutQuart = (3 << 2 | 2),
	EaseInOutQuart = (3 << 2 | 3),
	EaseInQuint = (4 << 2 | 1),
	EaseOutQuint = (4 << 2 | 2),
	EaseInOutQuint = (4 << 2 | 3),
	EaseInSine = (5 << 2 | 1),
	EaseOutSine = (5 << 2 | 2),
	EaseInOutSine = (5 << 2 | 3),
	EaseInExpo = (6 << 2 | 1),
	EaseOutExpo = (6 << 2 | 2),
	EaseInOutExpo = (6 << 2 | 3),
	EaseInCirc = (7 << 2 | 1),
	EaseOutCirc = (7 << 2 | 2),
	EaseInOutCirc = (7 << 2 | 3),
	EaseInBounce = (8 << 2 | 1),
	EaseOutBounce = (8 << 2 | 2),
	EaseInOutBounce = (8 << 2 | 3),
	EaseInElastic = (9 << 2 | 1),
	EaseOutElastic = (9 << 2 | 2),
	EaseInOutElastic = (9 << 2 | 3),
	EaseInBack = (10 << 2 | 1),
	EaseOutBack = (10 << 2 | 2),
	EaseInOutBack = (10 << 2 | 3),
	EaseSpring,
	EaseShake,
	Punch,
	AnimationCurve
};

UENUM(BlueprintType)
enum class EQTweenLoopType : uint8
{
	Once,
	Clamp,
	PingPong,
};

#define QTWEEN_CALLBACK(UserObject, FuncName) UserObject, FuncName, STATIC_FUNCTION_FNAME(TEXT( #FuncName ))

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQTweenEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQTweenOnUpdateEvent, float, value);

USTRUCT(BlueprintType)
struct QTWEEN_API FQTweenDescrOptional
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	UObject* Owner;
	UPROPERTY()
	FVector Point;
	UPROPERTY()
	FVector Axis;
	UPROPERTY()
	FQuat OrigRotation;
	UPROPERTY()
	const UCurveFloat* AnimCurve;
	UPROPERTY()
	uint32 InitFrameCount;
	UPROPERTY()
	FLinearColor Color;

	UPROPERTY()
	FQTweenEvent OnStart;
	UPROPERTY()
	FQTweenEvent OnComplete;
	UPROPERTY()
	FQTweenOnUpdateEvent OnUpdate;

public:
	void Reset()
	{
		AnimCurve = nullptr;
		Point = FVector::ZeroVector;
		InitFrameCount = 0;

		OnStart.Clear();
		OnComplete.Clear();
		OnUpdate.Clear();
	}

	void CallOnUpdate(float val, float ratioPassed)
	{
		OnUpdate.Broadcast(val);
	}
};

UCLASS(BlueprintType)
class QTWEEN_API UACMTweenEmpty : public UObject
{
	GENERATED_BODY()
public:

};

UCLASS(BlueprintType)
class QTWEEN_API UQTween : public UObject
{
    GENERATED_UCLASS_BODY()

    typedef FVector(UQTween::* EaseMethodFunc)();
public:
	UPROPERTY()
	uint8 bToggle : 1;
	UPROPERTY()
	uint8 bUseEstimatedTime : 1;
	UPROPERTY()
	uint8 bUseFrames : 1;
	UPROPERTY()
	uint8 bUseManualTime : 1;
	UPROPERTY()
	uint8 bUsesNormalDt : 1;
	UPROPERTY()
	uint8 bHasInitiliazed : 1;
	UPROPERTY()
	uint8 bHasExtraOnCompletes : 1;
	UPROPERTY()
	uint8 bHasPhysics : 1;
	UPROPERTY()
	uint8 bOnCompleteOnRepeat : 1;
	UPROPERTY()
	uint8 bOnCompleteOnStart : 1;
	UPROPERTY()
	uint8 bUseRecursion : 1;
	UPROPERTY()
	uint8 bHasUpdateCallback : 1;

	UPROPERTY()
	int32 LoopCount;
	UPROPERTY()
	uint32 Counter;
	UPROPERTY()
	float RatioPassed;
	UPROPERTY()
	float Passed;
	UPROPERTY()
	float Delay;
	UPROPERTY()
	float Time;
	UPROPERTY()
	float Speed;
	UPROPERTY()
	float LastVal;
	UPROPERTY()
	float Direction;
	UPROPERTY()
	float DirectionLast;
	UPROPERTY()
	float Overshoot;
	UPROPERTY()
	float Period;
	UPROPERTY()
	float Scale;
	UPROPERTY()
	FVector From;
	UPROPERTY()
	FVector To;
	UPROPERTY()
	FQTweenDescrOptional Optional;

	UPROPERTY()
	EQTweenAction Type;
	UPROPERTY()
	EQTweenLoopType LoopType;

	static float val;
	static float dt;
	static FVector newVect;

	FVector Diff;
	TWeakObjectPtr<UObject> Owner;
private:
	FVector DiffDiv2;
	EQTweenType EaseType;
	uint32 Id;
	IQTweenEasing* CurEasing;
	EQTweenEasingType CurEasingType;

	EaseMethodFunc EaseMethod;

	TFunction<void(void)> InitInternal;
	TFunction<void(void)> EaseInternal;


	static TArray<IQTweenEasing*> _EasingFunc;
public:
	static void InitEasingMothod();
	uint64 GetUniqueId();
	void Reset();
	void CallOnCompletes();

	UFUNCTION(BlueprintCallable)
	UQTween* Cancel(UObject* Obj);
	UFUNCTION(BlueprintCallable)
	UQTween* SetFollow();
	UFUNCTION(BlueprintCallable)
	UQTween* SetOffset(const FVector& Offset);
	UFUNCTION(BlueprintCallable)
	UQTween* SetCallback();
	UFUNCTION(BlueprintCallable)
	UQTween* SetTarget(UObject* Obj);
	UFUNCTION(BlueprintCallable)
	UQTween* UpdateNow();
	UFUNCTION(BlueprintCallable)
	UQTween* Pause();
	UFUNCTION(BlueprintCallable)
	UQTween* Resume();
	UFUNCTION(BlueprintCallable)
	UQTween* SetAxis(const FVector& Axis);
	UFUNCTION(BlueprintCallable)
	UQTween* SetDelay(float DelayTime);
	UFUNCTION(BlueprintCallable)
	UQTween* SetEase(EQTweenType EaseType);
	UFUNCTION(BlueprintCallable)
	UQTween* SetEaseCurve(const UCurveFloat* Curve);
	UFUNCTION(BlueprintCallable)
	UQTween* SetOvershoot(float Over);
	UFUNCTION(BlueprintCallable)
	UQTween* SetPeriod(float InPeriod);
	UFUNCTION(BlueprintCallable)
	UQTween* SetScale(float fScale);
	UFUNCTION(BlueprintCallable)
	UQTween* SetTo(const FVector& InTo);
	UFUNCTION(BlueprintCallable)
	UQTween* SetFrom(const FVector& InFrom);
	UFUNCTION(BlueprintCallable)
	UQTween* SetDiff(const FVector& Diff);
	UFUNCTION(BlueprintCallable)
	UQTween* SetHasInitialized(bool bInitialized);

	UQTween* SetId(uint32 id, uint32 GlobalCounter);

	UFUNCTION(BlueprintCallable)
	UQTween* SetPassed(float InPassed);
	UFUNCTION(BlueprintCallable)
	UQTween* SetTime(float InTime);
	UFUNCTION(BlueprintCallable)
	UQTween* SetSpeed(float InSpeed);
	UFUNCTION(BlueprintCallable)
	UQTween* SetRepeat(int32 Repeat);
	UFUNCTION(BlueprintCallable)
	UQTween* SetLoopType(EQTweenLoopType eLoopType);

	UFUNCTION(BlueprintCallable)
	UQTween* SetUseEstimatedTime(bool InUseEstimatedTime);

	UFUNCTION(BlueprintCallable)
	UQTween* SetIgnoreTimeScale(bool bIignoreTimeScale);

	UFUNCTION(BlueprintCallable)
	UQTween* SetUseFrames(bool InUseFrames);
	UFUNCTION(BlueprintCallable)
	UQTween* SetUseManualTime(bool InUseManualTime);

	UFUNCTION(BlueprintCallable)
	UQTween* SetLoopCount(int32 count);
	UFUNCTION(BlueprintCallable)
	UQTween* SetLoopOnce();
	UFUNCTION(BlueprintCallable)
	UQTween* SetLoopClamp(int32 Loops = 0);
	UFUNCTION(BlueprintCallable)
	UQTween* SetLoopPingPong(int32 Loops = 0);
	UFUNCTION(BlueprintCallable)
	UQTween* SetPoint(const FVector& Point);

	UFUNCTION(BlueprintCallable)
	UQTween* SetOnCompleteOnRepeat(bool bIsOn);
	UFUNCTION(BlueprintCallable)
	UQTween* SetOnCompleteOnStart(bool bIsOn);
	UFUNCTION(BlueprintCallable)
	UQTween* SetDirection(float InDir);
	UFUNCTION(BlueprintCallable)
	UQTween* SetRecursive(bool bUseRecursive);

	UQTween* SetAlpha();

	UQTween* SetColor();

	UQTween* SetMove();

	UQTween* SetRotate();

	UQTween* SetScale();

	UQTween* SetCallbackColor();

	UQTween* SetFromColor(const FLinearColor& Color);

	template<typename UserClass>
	UQTween* SetOnStart(UserClass* Obj, void(UserClass::* OnStart)(), FName FuncName)
	{
		Optional.OnStart.__Internal_AddUniqueDynamic(Obj, OnStart, FuncName);
		return this;
	}

	template<typename UserClass>
	UQTween* SetOnComplete(UserClass* Obj, void(UserClass::* OnComplete)(), FName FuncName)
	{
		Optional.OnComplete.__Internal_AddUniqueDynamic(Obj, OnComplete, FuncName);
		bHasExtraOnCompletes = true;
		return this;
	}

	template<typename UserClass>
	UQTween* SetOnUpdate(UserClass* Obj, void(UserClass::* OnUpdate)(float), FName FuncName)
	{
		Optional.OnUpdate.__Internal_AddUniqueDynamic(Obj, OnUpdate, FuncName);
		bHasUpdateCallback = true;

		return this;
	}

	bool UpdateInternal();
	static bool IsValid(const UQTween* Tween);
private:
	float GetDeltaTime();
	void Init();
	void InitFromInternal();
	void InitSpeed();
	void Callback();
	void SetEaseInternal(EQTweenEasingFunc func, EQTweenEasingType type);
	void SetEasePunch();
	void SetEaseShake();
	void SetEaseSprint();

	FVector TweenOnCurve();
	FVector TweenByEasingType();
	FLinearColor TweenColor(UQTween* Tween, float Fractor);
};

