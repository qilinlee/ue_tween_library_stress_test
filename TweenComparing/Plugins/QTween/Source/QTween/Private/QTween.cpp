#include "QTween.h"
#include "QTweenEasing.h"
#include "QTweenEngineSubsystem.h"
#include "Components/Widget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/Actor.h"
#include "QTweenPropAccessor.h"



//-------------------------------------------------------------------------
//	UQTween
//-------------------------------------------------------------------------

float UQTween::val = 0.f;
float UQTween::dt = 0.f;
FVector UQTween::newVect = FVector::ZeroVector;
TArray<IQTweenEasing*> UQTween::_EasingFunc;

UQTween::UQTween(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bToggle(0)
	, bUseEstimatedTime(0)
	, bUseFrames(0)
	, bUseManualTime(0)
	, bUsesNormalDt(0)
	, bHasInitiliazed(0)
	, bHasExtraOnCompletes(0)
	, bHasPhysics(0)
	, bOnCompleteOnRepeat(0)
	, bOnCompleteOnStart(0)
	, bUseRecursion(0)
	, bHasUpdateCallback(0)
	, LoopCount(0)
	, Counter(-1)
	, RatioPassed(0.f)
	, Passed(0.f)
	, Delay(0.f)
	, Time(0.f)
	, Speed(1.f)
	, LastVal(0.f)
	, Direction(0.f)
	, DirectionLast(0.f)
	, Overshoot(0.f)
	, Period(0.f)
	, Scale(1.f)
	, Type(EQTweenAction::ALPHA)
	, LoopType(EQTweenLoopType::Once)
	, EaseType(EQTweenType::NotUsed)
	, Id(0)
	, CurEasing(nullptr)
	, CurEasingType(EQTweenEasingType::EasingNone)
	, EaseMethod(nullptr)
{

}

void UQTween::InitEasingMothod()
{
	_EasingFunc.Reserve((int32)EQTweenEasingFunc::Max);
	_EasingFunc.Empty();
	_EasingFunc.Add(&QTween::Easing::FLinearEasing::Linear);
	_EasingFunc.Add(&QTween::Easing::FQuadraticEasing::Quard);
	_EasingFunc.Add(&QTween::Easing::FCubicInEasing::Cubic);
	_EasingFunc.Add(&QTween::Easing::FQuarticEasing::Quart);
	_EasingFunc.Add(&QTween::Easing::FQuinticEasing::Quintic);
	_EasingFunc.Add(&QTween::Easing::FSinEasing::Sin);
	_EasingFunc.Add(&QTween::Easing::FExponentialEasing::Exponential);
	_EasingFunc.Add(&QTween::Easing::FCircularInEasing::Circluar);
	_EasingFunc.Add(&QTween::Easing::FBounceEasing::Bounce);
	_EasingFunc.Add(&QTween::Easing::FElasticEasing::Elastic);
	_EasingFunc.Add(&QTween::Easing::FBackEasing::Back);
	_EasingFunc.Add(&QTween::Easing::FSpringEasing::Spring);
}

uint64 UQTween::GetUniqueId()
{
	uint64 t = (uint64)Id;
	t |= (Counter << 16);
	return t;
}

void UQTween::Reset()
{
	bToggle = true;
	bUseRecursion = true;
	bUsesNormalDt = true;
	Passed = 0.f;
	Delay = 0.f;
	LastVal = 0.f;
	bHasUpdateCallback = false;
	bUseEstimatedTime = false;
	bUseFrames = false;
	bHasInitiliazed = false;
	bOnCompleteOnRepeat = false;
	bOnCompleteOnStart = false;
	bUseManualTime = false;
	bHasExtraOnCompletes = false;
	bToggle = false;
	Owner = nullptr;
	EaseType = EQTweenType::Linear;
	LoopType = EQTweenLoopType::Once;
	LoopCount = 0;
	Direction = DirectionLast = Overshoot = Scale = 1.f;
	Period = 0.3f;
	Speed = -1.f;
	EaseMethod = &UQTween::TweenByEasingType;
	From = To = FVector::ZeroVector;
	SetEase(EaseType);
	Optional.Reset();
}

void UQTween::CallOnCompletes()
{
	if (Optional.OnComplete.IsBound())
	{
		Optional.OnComplete.Broadcast();
	}
}

UQTween* UQTween::Cancel(UObject* Obj)
{
	if (Owner == Obj)
	{
		UQTweenEngineSubsystem::Get()->RemoveTween(Id, GetUniqueId());
	}
	return this;
}

UQTween* UQTween::SetFollow()
{
	Type = EQTweenAction::FOLLOW;
	return this;
}

UQTween* UQTween::SetOffset(const FVector& offset)
{
	To = offset;
	return this;
}

UQTween* UQTween::SetCallback()
{
	Type = EQTweenAction::CALL_BACK;
	InitInternal = []() {};
	EaseInternal = [this]() { this->Callback(); };
	return this;
}

UQTween* UQTween::SetTarget(UObject* obj)
{
	Optional.Owner = obj;
	return this;
}

UQTween* UQTween::UpdateNow()
{
	UpdateInternal();
	return this;
}

UQTween* UQTween::Pause()
{
	if (Direction != 0.0f)
	{
		DirectionLast = Direction;
		Direction = 0.f;
	}
	return this;
}

UQTween* UQTween::Resume()
{
	Direction = DirectionLast;
	return this;
}

UQTween* UQTween::SetAxis(const FVector& axis)
{
	Optional.Axis = axis;
	return this;
}

UQTween* UQTween::SetDelay(float delayTime)
{
	Delay = delayTime;
	return this;
}

UQTween* UQTween::SetEase(EQTweenType InEaseType)
{
	this->EaseType = InEaseType;
	switch (InEaseType)
	{
	case EQTweenType::EaseOutQuad:
	case EQTweenType::EaseInQuad:
	case EQTweenType::EaseInOutQuad:
	case EQTweenType::EaseInCubic:
	case EQTweenType::EaseOutCubic:
	case EQTweenType::EaseInOutCubic:
	case EQTweenType::EaseInQuart:
	case EQTweenType::EaseOutQuart:
	case EQTweenType::EaseInOutQuart:
	case EQTweenType::EaseInQuint:
	case EQTweenType::EaseOutQuint:
	case EQTweenType::EaseInOutQuint:
	case EQTweenType::EaseInSine:
	case EQTweenType::EaseOutSine:
	case EQTweenType::EaseInOutSine:
	case EQTweenType::EaseInExpo:
	case EQTweenType::EaseOutExpo:
	case EQTweenType::EaseInOutExpo:
	case EQTweenType::EaseInCirc:
	case EQTweenType::EaseOutCirc:
	case EQTweenType::EaseInOutCirc:
	case EQTweenType::EaseInBounce:
	case EQTweenType::EaseOutBounce:
	case EQTweenType::EaseInOutBounce:
	case EQTweenType::EaseInBack:
	case EQTweenType::EaseOutBack:
	case EQTweenType::EaseInOutBack:
	case EQTweenType::EaseInElastic:
	case EQTweenType::EaseOutElastic:
	case EQTweenType::EaseInOutElastic:
	case EQTweenType::EaseSpring:
	{
		uint8 ease = (uint8)InEaseType;
		EQTweenEasingType t = (EQTweenEasingType)(ease & (uint8)EQTweenEasingType::EasingInOut);
		EQTweenEasingFunc func = (EQTweenEasingFunc)((ease - (uint8)t) >> 2);
		SetEaseInternal(func, t);
		break;
	}
	case EQTweenType::Punch:
	{
		SetEasePunch();
		break;
	}
	case EQTweenType::EaseShake:
	{
		SetEaseShake();
		break;
	}
	default:
	{
		SetEaseInternal(EQTweenEasingFunc::EasingLinear, EQTweenEasingType::EasingNone);
		break;
	}
	}

	return this;
}

UQTween* UQTween::SetEaseCurve(const UCurveFloat* curve)
{
	Optional.AnimCurve = curve;
	EaseType = EQTweenType::AnimationCurve;
	EaseMethod = &UQTween::TweenOnCurve;
	return this;
}

UQTween* UQTween::SetScale(float fScale)
{
	this->Scale = fScale;
	return this;
}

UQTween* UQTween::SetScale()
{
	Type = EQTweenAction::SCALE;
	InitInternal = [this]()
		{
			if (Owner != nullptr && Owner.IsValid())
			{
				this->From = FQTweenPropAccessor::GetScale(Owner.Get());
			}
		};

	EaseInternal = [this]()
		{
			newVect = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetScale(Owner.Get(), newVect);
			}
		};
	return this;
}

UQTween* UQTween::SetCallbackColor()
{
	Type = EQTweenAction::CALLBACK_COLOR;
	InitInternal = [this]()
		{
			this->Diff = FVector(1, 0, 0);
		};

	EaseInternal = [this]()
		{
			newVect = (this->*EaseMethod)();
			float fractor = newVect.X;
			FLinearColor toColor = TweenColor(this, fractor);
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetColor(Owner.Get(), toColor);
			}
		};

	return this;
}

UQTween* UQTween::SetFromColor(const FLinearColor& col)
{
	From = FVector(0.0f, col.A, 0.0f);
	Diff = FVector(1.0f, 0.0f, 0.0f);
	Optional.Axis = FVector(col.R, col.G, col.B);
	return this;
}

UQTween* UQTween::SetTo(const FVector& vTo)
{
	if (bHasInitiliazed)
	{
		this->To = vTo;
		Diff = vTo - From;
	}
	else
	{
		this->To = vTo;
	}
	return this;
}

UQTween* UQTween::SetFrom(const FVector& vFrom)
{
	if (Owner != nullptr && Owner.IsValid())
		Init();
	this->From = vFrom;
	Diff = To - vFrom;
	DiffDiv2 = Diff * 0.5f;
	return this;
}

UQTween* UQTween::SetOvershoot(float over)
{
	this->Overshoot = over;
	return this;
}

UQTween* UQTween::SetPeriod(float fPeriod)
{
	this->Period = fPeriod;
	return this;
}

UQTween* UQTween::SetDiff(const FVector& vDiff)
{
	this->Diff = vDiff;
	this->DiffDiv2 = vDiff * 0.5f;
	To = From + Diff;
	return this;
}

UQTween* UQTween::SetHasInitialized(bool InInitialized)
{
	bHasInitiliazed = InInitialized;
	return this;
}

UQTween* UQTween::SetId(uint32 id, uint32 GlobalCounter)
{
	Id = id;
	Counter = GlobalCounter;
	return this;
}

UQTween* UQTween::SetPassed(float fPassed)
{
	Passed = fPassed;
	return this;
}

UQTween* UQTween::SetTime(float fTime)
{
	float PassedTimeRatio = Passed / fTime;
	Passed = fTime * PassedTimeRatio;
	Time = fTime;
	return this;
}

UQTween* UQTween::SetSpeed(float fTime)
{
	this->Speed = fTime;
	
	if (bHasInitiliazed)
	{
		InitSpeed();
	}

	return this;
}

UQTween* UQTween::SetRepeat(int32 nRepeat)
{
	LoopCount = nRepeat;
	if (nRepeat > 1 && LoopType == EQTweenLoopType::Once
		|| (nRepeat < 0 && LoopType == EQTweenLoopType::Once))
	{
		LoopType = EQTweenLoopType::Clamp;
	}

	if (Type == EQTweenAction::CALL_BACK || Type == EQTweenAction::CALLBACK_COLOR)
		SetOnCompleteOnRepeat(true);
	return this;
}

UQTween* UQTween::SetLoopType(EQTweenLoopType eLoopType)
{
	this->LoopType = eLoopType;
	return this;
}

UQTween* UQTween::SetUseEstimatedTime(bool InUseEstimatedTime)
{
	this->bUseEstimatedTime = InUseEstimatedTime;
	bUsesNormalDt = false;
	return this;
}

UQTween* UQTween::SetIgnoreTimeScale(bool bIignoreTimeScale)
{
	bUseEstimatedTime = bIignoreTimeScale;
	bUsesNormalDt = false;
	return this;
}

UQTween* UQTween::SetUseFrames(bool InUseFrames)
{
	this->bUseFrames = InUseFrames;
	bUsesNormalDt = false;
	return this;
}

UQTween* UQTween::SetUseManualTime(bool InUseManualTime)
{
	bUseManualTime = InUseManualTime;
	bUsesNormalDt = false;
	return this;
}

UQTween* UQTween::SetLoopCount(int32 count)
{
	LoopCount = count;
	return this;
}

UQTween* UQTween::SetLoopOnce()
{
	LoopType = EQTweenLoopType::Once;
	return this;
}

UQTween* UQTween::SetLoopClamp(int32 loops)
{
	LoopType = EQTweenLoopType::Clamp;
	LoopCount = loops == 0 ? 1 : loops;
	return this;
}

UQTween* UQTween::SetLoopPingPong(int32 loops)
{
	LoopType = EQTweenLoopType::PingPong;
	LoopCount = loops < 1 ? -1 : loops * 2;
	return this;
}

UQTween* UQTween::SetPoint(const FVector& point)
{
	Optional.Point = point;
	return this;
}

UQTween* UQTween::SetOnCompleteOnRepeat(bool isOn)
{
	bOnCompleteOnRepeat = isOn;
	return this;
}

UQTween* UQTween::SetOnCompleteOnStart(bool isOn)
{
	bOnCompleteOnStart = isOn;
	return this;
}

UQTween* UQTween::SetDirection(float dir)
{
	if (Direction != -1.f && Direction != 1.f)
	{
		return this;
	}

	if (Direction != dir)
	{
		//if (bHasInitiliazed)
		Direction = dir;
	}
	return this;
}

UQTween* UQTween::SetRecursive(bool InUseRecursive)
{
	this->bUseRecursion = InUseRecursive;
	return this;
}

UQTween* UQTween::SetAlpha()
{
	Type = EQTweenAction::ALPHA;
	InitInternal = [this]()
		{
			if (Owner != nullptr && Owner.IsValid())
			{
				this->From = FVector(FQTweenPropAccessor::GetAlpha(Owner.Get()), 0, 0);
			}
		};

	EaseInternal = [this]()
		{
			newVect = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetAlpha(Owner.Get(), newVect.X);
			}
		};

	return this;
}

UQTween* UQTween::SetColor()
{
	Type = EQTweenAction::COLOR;
	InitInternal = [this]()
		{
			if (Owner != nullptr && Owner.IsValid())
			{
				SetFromColor(FQTweenPropAccessor::GetColor(Owner.Get()));
			}
		};

	EaseInternal = [this]()
		{
			newVect = (this->*EaseMethod)();
			float fractor = newVect.X;
			FLinearColor toColor = TweenColor(this, fractor);
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetColor(Owner.Get(), toColor);
			}
		};
	return this;
}

UQTween* UQTween::SetMove()
{
	Type = EQTweenAction::MOVE;
    InitInternal = [this]() {
        if (Owner != nullptr && Owner.IsValid())
        {
            From = FQTweenPropAccessor::GetPosition(Owner.Get());
        }
        };
    EaseInternal = [this]() {
        newVect = (this->*EaseMethod)();
        if (Owner != nullptr && Owner.IsValid())
        {
			FQTweenPropAccessor::SetPosition(Owner.Get(), newVect);
        }
        };
    return this;
}

UQTween* UQTween::SetRotate()
{
	Type = EQTweenAction::ROTATE;
	InitInternal = [this]()
		{
			if (this->Owner != nullptr && this->Owner.IsValid())
			{
				this->From = FQTweenPropAccessor::GetRotate(this->Owner.Get());
				this->To = FVector(UQTweenEngineSubsystem::ClosestRot(this->From.X, this->To.X),
					UQTweenEngineSubsystem::ClosestRot(this->From.Y, this->To.Y),
					UQTweenEngineSubsystem::ClosestRot(this->From.Z, this->To.Z));
			}
		};

	EaseInternal = [this]()
		{
			newVect = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetRotate(Owner.Get(), newVect);
			}
		};
	return this;
}

float UQTween::GetDeltaTime()
{
	float DeltaTime = 0.f;
	if (bUsesNormalDt)
	{
		DeltaTime = UQTweenEngineSubsystem::Get()->DtActual;
	}
	else if (bUseEstimatedTime)
	{
		DeltaTime = UQTweenEngineSubsystem::Get()->DtEstimated;
	}
	else if (bUseFrames)
	{
		DeltaTime = Optional.InitFrameCount == 0 ? 0 : 1;
		Optional.InitFrameCount = GFrameNumber;
	}
	else if (bUseManualTime)
	{
		DeltaTime = UQTweenEngineSubsystem::Get()->DtManual;
	}

	return DeltaTime;
}

void UQTween::Init()
{
	bHasInitiliazed = true;
	bUsesNormalDt = !(bUseEstimatedTime || bUseManualTime || bUseFrames);
	if (bUseFrames)
	{
		Optional.InitFrameCount = GFrameNumber;
	}

	if (Time <= 0.f)
	{
		Time = 0.f;
	}

	if (InitInternal != nullptr)
	{
		InitInternal();
	}

	Diff = To - From;
	DiffDiv2 = Diff * 0.5f;

	if (Optional.OnStart.IsBound())
	{
		Optional.OnStart.Broadcast();
	}

	if (bOnCompleteOnStart)
	{
		CallOnCompletes();
	}

	if (Speed > 0.f)
	{
		InitSpeed();
	}
}

void UQTween::InitFromInternal()
{
	From.X = 0.f;
}

void UQTween::InitSpeed()
{
	Time = (To - From).Size() / Speed;
}

void UQTween::Callback()
{
	newVect = (this->*EaseMethod)();
	val = newVect.X;
}

bool UQTween::UpdateInternal()
{
	if (Owner == nullptr || !Owner.IsValid())
	{
		return true;
	}

	float DirectionLocal = Direction;
	float DeltaTime = GetDeltaTime();

	if (Delay <= 0.f && DirectionLocal != 0.f)
	{
		// initialize if has not done so yet
		if (!bHasInitiliazed)
		{
			Init();
		}

		DeltaTime = DeltaTime * DirectionLocal;
		Passed += DeltaTime;

		Passed = FMath::Clamp(Passed, 0.f, Time);

		RatioPassed = Time != 0 ? (Passed / Time) : 1.f; // need To clamp when finished so it will finish at the exact spot and not overshoot

		if (EaseInternal != nullptr)
		{
			EaseInternal();
		}

		if (bHasUpdateCallback && Optional.OnUpdate.IsBound())
		{
			Optional.OnUpdate.Broadcast(RatioPassed);
		}

		bool bIsTweenFinished = DirectionLocal > 0.f ? Passed >= Time : Passed <= 0.f;
		if (bIsTweenFinished)
		{ // increment or flip tween
			LoopCount--;
			if (LoopType == EQTweenLoopType::PingPong)
			{
				Direction = 0.0f - DirectionLocal;
				bIsTweenFinished = false;
			}
			else
			{
				Passed = 0.f;
				bIsTweenFinished = LoopCount == 0 || LoopType == EQTweenLoopType::Once; // only return true if it is fully complete
			}


			if (bIsTweenFinished == false && bOnCompleteOnRepeat && bHasExtraOnCompletes)
			{
				CallOnCompletes(); // this only gets called if onCompleteOnRepeat is set To true, otherwise LeanTween class takes care of calling it
			}

			return bIsTweenFinished;
		}
	}
	else
	{
		Delay -= DeltaTime;
	}

	return false;
}

bool UQTween::IsValid(const UQTween* Tween)
{
	if (nullptr == Tween || !Tween->IsValidLowLevel())
		return false;

	if (nullptr == Tween->Owner || !Tween->Owner.IsValid())
		return false;

	return true;
}

void UQTween::SetEaseInternal(EQTweenEasingFunc func, EQTweenEasingType InType)
{
	int32 index = (int32)func;
	if (0 <= index && index < (int32)EQTweenEasingFunc::Max)
		CurEasing = _EasingFunc[index];
	else
		CurEasing = nullptr;
	CurEasingType = InType;
	EaseMethod = &UQTween::TweenByEasingType;
}

void UQTween::SetEasePunch()
{
	Optional.AnimCurve = UQTweenEngineSubsystem::Get()->CurvePunch;
	To.X = From.X + To.X;
	EaseMethod = &UQTween::TweenOnCurve;
}

void UQTween::SetEaseShake()
{
	Optional.AnimCurve = UQTweenEngineSubsystem::Get()->CurveShake;
	To.X = From.X + To.X;
	EaseMethod = &UQTween::TweenOnCurve;
}


FVector UQTween::TweenOnCurve()
{
	float k = RatioPassed;
	if (Optional.AnimCurve && Optional.AnimCurve->IsValidLowLevel())
		k = Optional.AnimCurve->GetFloatValue(RatioPassed);

	return From + Diff * k;
}

FVector UQTween::TweenByEasingType()
{
	float k = RatioPassed;

	if (CurEasing != nullptr)
	{
		switch (CurEasingType)
		{
		case EQTweenEasingType::EasingIn:
			k = CurEasing->EaseIn(RatioPassed, 0, 1);
			break;
		case EQTweenEasingType::EasingOut:
			k = CurEasing->EaseOut(RatioPassed, 0, 1);
			break;
		case EQTweenEasingType::EasingInOut:
			k = CurEasing->EaseInOut(RatioPassed, 0, 1);
			break;
		default:
			break;
		}
	}

	return From + Diff * k;
}

FLinearColor UQTween::TweenColor(UQTween* Tween, float Fractor)
{
	FVector rgb = Tween->Optional.Point * Fractor + Tween->Optional.Axis * (1 - Fractor);
	float a = Tween->To.Y * Fractor + Tween->From.Y * (1 - Fractor);
	return FLinearColor(rgb.X, rgb.Y, rgb.Z, a);
}
