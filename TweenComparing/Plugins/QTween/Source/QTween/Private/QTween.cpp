#include "QTween.h"

#include "QTweenEasing.h"
#include "QTweenEngineSubsystem.h"
#include "GameFramework/Actor.h"
#include "QTweenPropAccessor.h"


static const TCHAR* LexToString(EQTweenAction Action)
{
	static const TCHAR* Strings[] = {
		TEXT("ALPHA"),
		TEXT("COLOR"),
		TEXT("CALLBACK_COLOR"),
		TEXT("CALL_BACK"),
		TEXT("MOVE"),
		TEXT("ROTATE"),
		TEXT("SCALE"),
		TEXT("FOLLOW"),
		TEXT("NONE")
	};
	
	if(EQTweenAction::ALPHA <= Action && Action <= EQTweenAction::NONE)
	{
		return Strings[static_cast<int>(Action)];
	}

	return TEXT("UNKNOWN");
}


FQTweenHandle FQTweenHandle::Invalid;

TSharedRef<FQTweenBase> FQTweenBase::SetID(uint32 InID, uint32 InGlobalCounter)
{
	Id = InID;
	Counter = InGlobalCounter;
	return SharedThis(this);
}


//-------------------------------------------------------------------------
//	FQTweenInstance
//-------------------------------------------------------------------------

float FQTweenInstance::Val = 0.f;
float FQTweenInstance::DT = 0.f;
FVector FQTweenInstance::NewVector = FVector::ZeroVector;
TArray<IQTweenEasing*> FQTweenInstance::EasingFuncList;


FQTweenInstance::FQTweenInstance()
	: FQTweenBase()
	  , bToggle(0)
	  , bUseEstimatedTime(0)
	  , bUseFrames(0)
	  , bUseManualTime(0)
	  , bUsesNormalDt(0)
	  , bHasInitialized(0)
	  , bHasExtraOnCompletes(0)
	  , bHasPhysics(0)
	  , bOnCompleteOnRepeat(0)
	  , bOnCompleteOnStart(0)
	  , bUseRecursion(0)
	  , bHasUpdateCallback(0)
	  , LoopCount(0)
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
      , InitFrameCount(0)
	  , EaseType(EQTweenType::NotUsed)
	  , ObjUniqueID(0)
	  , CurEasing(nullptr)
	  , CurEasingType(EQTweenEasingType::EasingNone)
	  , EaseMethod(nullptr)
{
}

void FQTweenInstance::InitEasingMethod()
{
	EasingFuncList.Reserve(static_cast<int32>(EQTweenEasingFunc::Max));
	EasingFuncList.Empty();
	EasingFuncList.Add(&QTween::Easing::FLinearEasing::Linear);
	EasingFuncList.Add(&QTween::Easing::FQuadraticEasing::Quad);
	EasingFuncList.Add(&QTween::Easing::FCubicInEasing::Cubic);
	EasingFuncList.Add(&QTween::Easing::FQuarticEasing::Quart);
	EasingFuncList.Add(&QTween::Easing::FQuinticEasing::Quintic);
	EasingFuncList.Add(&QTween::Easing::FSinEasing::Sin);
	EasingFuncList.Add(&QTween::Easing::FExponentialEasing::Exponential);
	EasingFuncList.Add(&QTween::Easing::FCircularInEasing::Circular);
	EasingFuncList.Add(&QTween::Easing::FBounceEasing::Bounce);
	EasingFuncList.Add(&QTween::Easing::FElasticEasing::Elastic);
	EasingFuncList.Add(&QTween::Easing::FBackEasing::Back);
	EasingFuncList.Add(&QTween::Easing::FSpringEasing::Spring);
}

void FQTweenInstance::ResetTween()
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
	bHasInitialized = false;
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
	EaseMethod = &FQTweenInstance::TweenByEasingType;
	From = To = FVector::ZeroVector;
	SetEase(EaseType);
	AnimCurve = nullptr;
	Point = FVector::ZeroVector;
	InitFrameCount = 0;

	OnStart.Clear();
	OnComplete.Clear();
	OnUpdate.Clear();
}

void FQTweenInstance::CallOnCompletes() const
{
	if (OnComplete.IsBound())
	{
		SCOPED_NAMED_EVENT(OnCompleteCallback, FColor::Magenta);
		OnComplete.Broadcast();
	}
}

void FQTweenInstance::SetObjUniqueID(uint32 InUniqueID)
{
	ObjUniqueID = InUniqueID;
}

uint32 FQTweenInstance::GetObjUniqueID() const
{
	return ObjUniqueID;
}

TSharedRef<FQTweenInstance> FQTweenInstance::Cancel(const UObject* Obj)
{
	if (Owner == Obj)
	{
		UQTweenEngineSubsystem::Get()->RemoveTween(Id, GetUniqueID());
	}
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetFollow()
{
	Type = EQTweenAction::FOLLOW;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOffset(const FVector& offset)
{
	To = offset;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetCallback()
{
	Type = EQTweenAction::CALL_BACK;
	InitInternal = []() {};
	EaseInternal = [this]() { this->Callback(); };
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetTarget(UObject* obj)
{
	Owner = obj;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::UpdateNow()
{
	UpdateInternal();
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::Pause()
{
	if (Direction != 0.0f)
	{
		DirectionLast = Direction;
		Direction = 0.f;
	}
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::Resume()
{
	Direction = DirectionLast;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetAxis(const FVector& axis)
{
	Axis = axis;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDelay(float delayTime)
{
	Delay = delayTime;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetEase(EQTweenType InEaseType)
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
		uint8 ease = static_cast<uint8>(InEaseType);
		EQTweenEasingType t = static_cast<EQTweenEasingType>(ease & static_cast<uint8>(EQTweenEasingType::EasingInOut));
		EQTweenEasingFunc func = static_cast<EQTweenEasingFunc>((ease - static_cast<uint8>(t)) >> 2);
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

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetEaseCurve(UCurveFloat* curve)
{
	AnimCurve = curve;
	EaseType = EQTweenType::AnimationCurve;
	EaseMethod = &FQTweenInstance::TweenOnCurve;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetScale(float fScale)
{
	this->Scale = fScale;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetScale()
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
			NewVector = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetScale(Owner.Get(), NewVector);
			}
		};
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetCallbackColor()
{
	Type = EQTweenAction::CALLBACK_COLOR;
	InitInternal = [this]()
		{
			this->Diff = FVector(1, 0, 0);
		};

	EaseInternal = [this]()
		{
			NewVector = (this->*EaseMethod)();
			float Factor = NewVector.X;
			FLinearColor toColor = TweenColor(SharedThis(this), Factor);
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetColor(Owner.Get(), toColor);
			}
		};

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetFromColor(const FLinearColor& col)
{
	From = FVector(0.0f, col.A, 0.0f);
	Diff = FVector(1.0f, 0.0f, 0.0f);
	Axis = FVector(col.R, col.G, col.B);
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetTo(const FVector& vTo)
{
	if (bHasInitialized)
	{
		this->To = vTo;
		Diff = vTo - From;
	}
	else
	{
		this->To = vTo;
	}
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetFrom(const FVector& vFrom)
{
	if (Owner != nullptr && Owner.IsValid())
	{
		Init();
	}
	this->From = vFrom;
	Diff = To - vFrom;
	DiffDiv2 = Diff * 0.5f;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOvershoot(float over)
{
	this->Overshoot = over;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPeriod(float fPeriod)
{
	this->Period = fPeriod;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDiff(const FVector& vDiff)
{
	this->Diff = vDiff;
	this->DiffDiv2 = vDiff * 0.5f;
	To = From + Diff;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetHasInitialized(bool InInitialized)
{
	bHasInitialized = InInitialized;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPassed(float fPassed)
{
	Passed = fPassed;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetTime(float fTime)
{
	float PassedTimeRatio = Passed / fTime;
	Passed = fTime * PassedTimeRatio;
	Time = fTime;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetSpeed(float fTime)
{
	this->Speed = fTime;
	
	if (bHasInitialized)
	{
		InitSpeed();
	}

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetRepeat(int32 nRepeat)
{
	LoopCount = nRepeat;
	if (nRepeat > 1 && LoopType == EQTweenLoopType::Once
		|| (nRepeat < 0 && LoopType == EQTweenLoopType::Once))
	{
		LoopType = EQTweenLoopType::Clamp;
	}

	if (Type == EQTweenAction::CALL_BACK || Type == EQTweenAction::CALLBACK_COLOR)
		SetOnCompleteOnRepeat(true);
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopType(EQTweenLoopType eLoopType)
{
	this->LoopType = eLoopType;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetUseEstimatedTime(bool InUseEstimatedTime)
{
	this->bUseEstimatedTime = InUseEstimatedTime;
	bUsesNormalDt = false;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetIgnoreTimeScale(bool bIgnoreTimeScale)
{
	bUseEstimatedTime = bIgnoreTimeScale;
	bUsesNormalDt = false;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetUseFrames(bool InUseFrames)
{
	this->bUseFrames = InUseFrames;
	bUsesNormalDt = false;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetUseManualTime(bool InUseManualTime)
{
	bUseManualTime = InUseManualTime;
	bUsesNormalDt = false;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopCount(int32 count)
{
	LoopCount = count;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopOnce()
{
	LoopType = EQTweenLoopType::Once;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopClamp(int32 loops)
{
	LoopType = EQTweenLoopType::Clamp;
	LoopCount = loops == 0 ? 1 : loops;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopPingPong(int32 loops)
{
	LoopType = EQTweenLoopType::PingPong;
	LoopCount = loops < 1 ? -1 : loops * 2;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPoint(const FVector& point)
{
	Point = point;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOnCompleteOnRepeat(bool isOn)
{
	bOnCompleteOnRepeat = isOn;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOnCompleteOnStart(bool isOn)
{
	bOnCompleteOnStart = isOn;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDirection(float dir)
{
	if (Direction != -1.f && Direction != 1.f)
	{
		return SharedThis<FQTweenInstance>(this);
	}

	if (Direction != dir)
	{
		Direction = dir;
	}
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetRecursive(bool InUseRecursive)
{
	this->bUseRecursion = InUseRecursive;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetAlpha()
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
			NewVector = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetAlpha(Owner.Get(), NewVector.X);
			}
		};

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetColor()
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
			NewVector = (this->*EaseMethod)();
			float Factor = NewVector.X;
			FLinearColor toColor = TweenColor(SharedThis(this), Factor);
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetColor(Owner.Get(), toColor);
			}
		};
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetMove()
{
	Type = EQTweenAction::MOVE;
    InitInternal = [this]() {
        if (Owner != nullptr && Owner.IsValid())
        {
            From = FQTweenPropAccessor::GetPosition(Owner.Get());
        }
        };
    EaseInternal = [this]() {
        NewVector = (this->*EaseMethod)();
        if (Owner != nullptr && Owner.IsValid())
        {
			FQTweenPropAccessor::SetPosition(Owner.Get(), NewVector);
        }
        };
    return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetRotate()
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
			NewVector = (this->*EaseMethod)();
			if (Owner != nullptr && Owner.IsValid())
			{
				FQTweenPropAccessor::SetRotate(Owner.Get(), NewVector);
			}
		};
	return SharedThis<FQTweenInstance>(this);
}

float FQTweenInstance::GetDeltaTime()
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
		DeltaTime = InitFrameCount == 0 ? 0 : 1;
		InitFrameCount = GFrameNumber;
	}
	else if (bUseManualTime)
	{
		DeltaTime = UQTweenEngineSubsystem::Get()->DtManual;
	}

	return DeltaTime;
}

void FQTweenInstance::Init()
{
	bHasInitialized = true;
	bUsesNormalDt = !(bUseEstimatedTime || bUseManualTime || bUseFrames);
	if (bUseFrames)
	{
		InitFrameCount = GFrameNumber;
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

	if (OnStart.IsBound())
	{
		OnStart.Broadcast();
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

void FQTweenInstance::InitFromInternal()
{
	From.X = 0.f;
}

void FQTweenInstance::InitSpeed()
{
	Time = (To - From).Size() / Speed;
}

void FQTweenInstance::Callback()
{
	NewVector = (this->*EaseMethod)();
	Val = NewVector.X;
}

bool FQTweenInstance::UpdateInternal()
{
	if (Owner == nullptr || !Owner.IsValid())
	{
		return true;
	}

	SCOPED_NAMED_EVENT_F(TEXT("QTween UpdateInternal %s %u"), FColor::Cyan, LexToString(Type), Id);

	float DirectionLocal = Direction;
	float DeltaTime = GetDeltaTime();

	if (Delay <= 0.f && DirectionLocal != 0.f)
	{
		if (!bHasInitialized)
		{
			Init();
		}

		DeltaTime = DeltaTime * DirectionLocal;
		Passed += DeltaTime;

		Passed = FMath::Clamp(Passed, 0.f, Time);

		RatioPassed = Time != 0 ? (Passed / Time) : 1.f; // need To clamp when finished so it will finish at the exact spot and not overshoot

		if (EaseInternal != nullptr)
		{
			SCOPED_NAMED_EVENT(EaseFuncCalculate, FColor::Silver);
			EaseInternal();
		}

		if (bHasUpdateCallback && OnUpdate.IsBound())
		{
			SCOPED_NAMED_EVENT(OnUpdateCallback, FColor::Orange);
			OnUpdate.Broadcast(RatioPassed);
		}

		if (bool bIsTweenFinished = DirectionLocal > 0.f ? Passed >= Time : Passed <= 0.f)
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


			if (!bIsTweenFinished && bOnCompleteOnRepeat && bHasExtraOnCompletes)
			{
				SCOPED_NAMED_EVENT(OnCompleteOnRepeatCallback, FColor::Turquoise);
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

void FQTweenInstance::SetOwner(UObject* Obj)
{
	Owner = Obj;
}

bool FQTweenInstance::IsToggled() const
{
	return bToggle;
}

void FQTweenInstance::SetToggle(bool bInToggle)
{
	bToggle = bInToggle;
}

UObject* FQTweenInstance::GetOwner() const
{
	if(Owner.IsValid() && ::IsValid(Owner.Get()))
	{
		return Owner.Get();
	}
	return nullptr;
}

bool FQTweenInstance::IsValid(const TSharedPtr<FQTweenInstance> Tween)
{
	if(!Tween.IsValid())
	{
		return false;
	}
	
	if (nullptr == Tween->Owner || !Tween->Owner.IsValid())
	{
		return false;
	}

	return true;
}

bool FQTweenInstance::IsValid(const FQTweenInstance* Tween)
{
	if(Tween == nullptr)
	{
		return false;
	}
	
	if (nullptr == Tween->Owner || !Tween->Owner.IsValid())
	{
		return false;
	}

	return true;
}

void FQTweenInstance::SetEaseInternal(EQTweenEasingFunc func, EQTweenEasingType InType)
{
	int32 index = static_cast<int32>(func);
	if (0 <= index && index < static_cast<int32>(EQTweenEasingFunc::Max))
		CurEasing = EasingFuncList[index];
	else
		CurEasing = nullptr;
	CurEasingType = InType;
	EaseMethod = &FQTweenInstance::TweenByEasingType;
}

void FQTweenInstance::SetEasePunch()
{
	AnimCurve = UQTweenEngineSubsystem::Get()->CurvePunch;
	To.X = From.X + To.X;
	EaseMethod = &FQTweenInstance::TweenOnCurve;
}

void FQTweenInstance::SetEaseShake()
{
	AnimCurve = UQTweenEngineSubsystem::Get()->CurveShake;
	To.X = From.X + To.X;
	EaseMethod = &FQTweenInstance::TweenOnCurve;
}

void FQTweenInstance::SetEaseSprint()
{

}

FVector FQTweenInstance::TweenOnCurve()
{
	float k = RatioPassed;
	if (AnimCurve.IsValid() && AnimCurve->IsValidLowLevel())
	{
		k = AnimCurve->GetFloatValue(RatioPassed);
	}

	return From + Diff * k;
}

float FQTweenInstance::TweenOnCurve(float ratioPassed) const
{
	if(AnimCurve.IsValid())
	{
		float f = AnimCurve->GetFloatValue(ratioPassed);
		return From.X + Diff.X * f; 
	}

	return 0.f;
}

FVector FQTweenInstance::TweenOnCurve(float ratioPassed, FVector& OutVector) const
{
	OutVector = FVector::ZeroVector;
	if(AnimCurve.IsValid())
	{
		float f = AnimCurve->GetFloatValue(ratioPassed);
		OutVector = From + Diff * f;
	}
	return OutVector;
}

FVector FQTweenInstance::TweenByEasingType()
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

FLinearColor FQTweenInstance::TweenColor(TSharedRef<FQTweenInstance> Tween, float Factor)
{
	FVector rgb = Tween->Point * Factor + Tween->Axis * (1 - Factor);
	float a = Tween->To.Y * Factor + Tween->From.Y * (1 - Factor);
	return FLinearColor(rgb.X, rgb.Y, rgb.Z, a);
}

bool FQTweenHandle::IsValid() const
{
	if(UniqueID == -1 || !Instance.IsValid() || !FQTweenInstance::IsValid(Instance.Pin()))
	{
		return false;
	}

	return UniqueID == Instance.Pin()->GetUniqueID();
}

bool FQTweenHandle::operator==(const FQTweenHandle& Other) const
{
	return UniqueID == Other.UniqueID;
}

FQTweenHandle::operator bool() const
{
	return UniqueID != -1;
}

bool FQTweenHandle::operator<(const FQTweenHandle& Other) const
{
	return UniqueID < Other.UniqueID;
}

bool FQTweenHandle::operator<=(const FQTweenHandle& Other) const
{
	if(UniqueID != - 1 && Other.UniqueID != -1)
	{
		return UniqueID <= Other.UniqueID;
	}

	if(UniqueID == -1 && Other.UniqueID == -1)
	{
		return reinterpret_cast<uint64>(this) <= reinterpret_cast<uint64>(&Other);
	}

	return UniqueID <= Other.UniqueID;
}

TSharedPtr<FQTweenInstance> FQTweenHandle::operator->() const
{
	if(IsValid())
	{
		return Instance.Pin();
	}

	return nullptr;
}
