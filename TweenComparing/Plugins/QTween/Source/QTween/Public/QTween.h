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

class QTWEEN_API FQTweenBase : public TSharedFromThis<FQTweenBase>
{
public:
	virtual ~FQTweenBase() {}

	static uint64 GenUniqueID(uint32 ID, uint32 InCounter)
	{
		uint64 t = ID;
		t |= InCounter << 16;
		return t;
	}

	static void BreakUniqueID(uint64 UniqueID, uint32& OutId, uint32 OutCounter)
	{
		OutId = static_cast<uint32>(UniqueID & 0xFFFF);
		OutCounter = static_cast<uint32>(UniqueID >> 16);
	}
	
	uint64 GetUniqueID() const
	{
		return GenUniqueID(Id, Counter);
	}

	TSharedRef<FQTweenBase> SetID(uint32 InID, uint32 InGlobalCounter);

protected:
	uint32 Id = 0;
	uint32 Counter = 0;
};

class QTWEEN_API FQTweenInstance : public FQTweenBase
{
    typedef FVector(FQTweenInstance::* EaseMethodFunc)();
	friend class UQTweenEngineSubsystem;
	friend class FQTweenSequence;
protected:
	
	uint8 bToggle : 1;
	
	uint8 bUseEstimatedTime : 1;
	
	uint8 bUseFrames : 1;
	
	uint8 bUseManualTime : 1;
	
	uint8 bUsesNormalDt : 1;
	
	uint8 bHasInitialized : 1;
	
	uint8 bHasExtraOnCompletes : 1;
	
	uint8 bOnCompleteOnRepeat : 1;
	
	uint8 bOnCompleteOnStart : 1;
	
	uint8 bUseRecursion : 1;
	
	uint8 bHasUpdateCallback : 1;
	
	int32 LoopCount;
	
	float RatioPassed;
	
	float Passed;
	
	float Delay;
	
	float Time;
	
	float Speed;
	
	float Direction;
	
	float DirectionLast;
	
	float Overshoot;
	
	float Period;
	
	float Scale;
	
	FVector From;
	
	FVector To;
	
	EQTweenAction Type;
	
	EQTweenLoopType LoopType;

	FVector Diff;
	
	TWeakObjectPtr<UObject> Owner;

    FVector Point;

    FVector Axis;

	TWeakObjectPtr<UCurveFloat> AnimCurve;

    uint32 InitFrameCount;

    FQTweenEvent OnStart;

    FQTweenEvent OnComplete;

    FQTweenOnUpdateEvent OnUpdate;
private:
	static float Val;
	
	FVector DiffDiv2;
	EQTweenType EaseType;
	
	uint32 ObjUniqueID;
	IQTweenEasing* CurEasing;
	EQTweenEasingType CurEasingType;

	EaseMethodFunc EaseMethod;

	TFunction<void(void)> InitInternal;
	TFunction<void(void)> EaseInternal;

	static TArray<IQTweenEasing*> EasingFuncList;
public:
    FQTweenInstance();
	static void InitEasingMethod();
	
	void ResetTween();
	void CallOnCompletes() const;

	void SetObjUniqueID(uint32 uniqueID);
	uint32 GetObjUniqueID() const;
	
	
	TSharedRef<FQTweenInstance> Cancel(const UObject* Obj);
	
	TSharedRef<FQTweenInstance> SetFollow();
	
	TSharedRef<FQTweenInstance> SetOffset(const FVector& Offset);
	
	TSharedRef<FQTweenInstance> SetCallback();
	
	TSharedRef<FQTweenInstance> SetTarget(UObject* Obj);
	
	TSharedRef<FQTweenInstance> UpdateNow();
	
	TSharedRef<FQTweenInstance> Pause();
	
	TSharedRef<FQTweenInstance> Resume();
	
	TSharedRef<FQTweenInstance> SetAxis(const FVector& Axis);
	
	TSharedRef<FQTweenInstance> SetDelay(float DelayTime);
	
	TSharedRef<FQTweenInstance> SetEase(EQTweenType EaseType);
	
	TSharedRef<FQTweenInstance> SetEaseCurve(UCurveFloat* Curve);
	
	TSharedRef<FQTweenInstance> SetOvershoot(float Over);
	
	TSharedRef<FQTweenInstance> SetPeriod(float InPeriod);
	
	TSharedRef<FQTweenInstance> SetScale(float InScale);
	
	TSharedRef<FQTweenInstance> SetTo(const FVector& InTo);
	
	TSharedRef<FQTweenInstance> SetFrom(const FVector& InFrom);
	
	TSharedRef<FQTweenInstance> SetDiff(const FVector& Diff);
	
	TSharedRef<FQTweenInstance> SetHasInitialized(bool bInitialized);

	
	
	TSharedRef<FQTweenInstance> SetPassed(float InPassed);
	
	TSharedRef<FQTweenInstance> SetTime(float InTime);
	
	TSharedRef<FQTweenInstance> SetSpeed(float InSpeed);
	
	TSharedRef<FQTweenInstance> SetRepeat(int32 Repeat);
	
	TSharedRef<FQTweenInstance> SetLoopType(EQTweenLoopType ELoopType);
	
	TSharedRef<FQTweenInstance> SetUseEstimatedTime(bool InUseEstimatedTime);
	
	TSharedRef<FQTweenInstance> SetIgnoreTimeScale(bool bIgnoreTimeScale);
	
	TSharedRef<FQTweenInstance> SetUseFrames(bool InUseFrames);
	
	TSharedRef<FQTweenInstance> SetUseManualTime(bool InUseManualTime);
	
	TSharedRef<FQTweenInstance> SetLoopCount(int32 InCount);
	
	TSharedRef<FQTweenInstance> SetLoopOnce();
	
	TSharedRef<FQTweenInstance> SetLoopClamp(int32 Loops = 0);
	
	TSharedRef<FQTweenInstance> SetLoopPingPong(int32 Loops = 0);
	
	TSharedRef<FQTweenInstance> SetPoint(const FVector& Point);
	
	TSharedRef<FQTweenInstance> SetOnCompleteOnRepeat(bool bIsOn);
	
	TSharedRef<FQTweenInstance> SetOnCompleteOnStart(bool bIsOn);
	
	TSharedRef<FQTweenInstance> SetDirection(float InDir);
	
    TSharedRef<FQTweenInstance> SetRecursive(bool bUseRecursive);

	TSharedRef<FQTweenInstance> SetAlpha();

	TSharedRef<FQTweenInstance> SetColor();

	TSharedRef<FQTweenInstance> SetMove();

	TSharedRef<FQTweenInstance> SetRotate();

	TSharedRef<FQTweenInstance> SetScale();

	TSharedRef<FQTweenInstance> SetCallbackColor();

	TSharedRef<FQTweenInstance> SetFromColor(const FLinearColor& Color);

	
	template<typename UserClass>
	TSharedRef<FQTweenInstance> SetOnStart(UserClass* Obj, void(UserClass::* InOnStart)(), FName FuncName)
	{
		InOnStart.__Internal_AddUniqueDynamic(Obj, InOnStart, FuncName);
		return SharedThis<FQTweenInstance>(this);
	}

	template<typename UserClass>
	TSharedRef<FQTweenInstance> SetOnComplete(UserClass* Obj, void(UserClass::* InOnComplete)(), FName FuncName)
	{
		InOnComplete.__Internal_AddUniqueDynamic(Obj, InOnComplete, FuncName);
		bHasExtraOnCompletes = true;
		return SharedThis<FQTweenInstance>(this);
	}

	template<typename UserClass>
	TSharedRef<FQTweenInstance> SetOnUpdate(UserClass* Obj, void(UserClass::* InOnUpdate)(float), FName FuncName)
	{
		InOnUpdate.__Internal_AddUniqueDynamic(Obj, InOnUpdate, FuncName);
		bHasUpdateCallback = true;

		return SharedThis<FQTweenInstance>(this);
	}

	bool UpdateInternal();
    
    bool IsToggled() const;
    void SetToggle(bool bInToggle);
	
    UObject* GetOwner() const;
	void SetOwner(UObject* Obj);

	FVector TweenOnCurve();
	float TweenOnCurve(float ratioPassed) const;
	FVector TweenOnCurve(float ratioPassed, FVector& OutVector) const;
	
    static bool IsValid(const TSharedPtr<FQTweenInstance> Instance);
	static bool IsValid(const FQTweenInstance* Instance);
private:
	float GetDeltaTime();
	void Init();
	void InitFromInternal();
	void InitSpeed();
	void Callback();
	void SetEaseInternal(EQTweenEasingFunc EasingFunc, EQTweenEasingType type);
	void SetEasePunch();
	void SetEaseShake();
	void SetEaseSprint();

	
	FVector TweenByEasingType();
    static FLinearColor TweenColor(TSharedRef<FQTweenInstance> Instance, float Factor);
};

USTRUCT(BlueprintType)
struct QTWEEN_API FQTweenHandle
{
	GENERATED_BODY()

	static constexpr uint64 INVALID_UNIQUE_ID = -1;
	uint64 UniqueID = INVALID_UNIQUE_ID;
	TWeakPtr<FQTweenInstance> Instance = nullptr;

public:
	FQTweenHandle() : UniqueID(INVALID_UNIQUE_ID), Instance(nullptr) {}
	explicit FQTweenHandle(TSharedPtr<FQTweenInstance> Tween) : UniqueID(Tween->GetUniqueID()), Instance(Tween) {}

	bool IsValid() const;
	
	static FQTweenHandle Invalid;
	
	bool operator==(const FQTweenHandle& Other) const;
	operator bool() const;
	bool operator<(const FQTweenHandle& Other) const;
	bool operator<=(const FQTweenHandle& Other) const;
	TSharedPtr<FQTweenInstance> operator->() const;
};


UCLASS(BlueprintType)
class QTWEEN_API UACMTweenEmpty : public UObject
{
	GENERATED_BODY()
public:

};
