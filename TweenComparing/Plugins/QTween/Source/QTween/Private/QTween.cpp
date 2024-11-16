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

TSharedRef<FQTweenInstance> FQTweenInstance::SetOffset(const FVector& InOffset)
{
	To = InOffset;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetCallback()
{
	Type = EQTweenAction::CALL_BACK;
	InitInternal = []() {};
	EaseInternal = [this]() { this->Callback(); };
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetTarget(UObject* Obj)
{
	Owner = Obj;
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

TSharedRef<FQTweenInstance> FQTweenInstance::SetAxis(const FVector& InAxis)
{
	Axis = InAxis;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDelay(float IndelayTime)
{
	Delay = IndelayTime;
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

TSharedRef<FQTweenInstance> FQTweenInstance::SetEaseCurve(UCurveFloat* InCurve)
{
	AnimCurve = InCurve;
	EaseType = EQTweenType::AnimationCurve;
	EaseMethod = &FQTweenInstance::TweenOnCurve;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetScale(float InScale)
{
	this->Scale = InScale;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetScale()
{
	Type = EQTweenAction::SCALE;
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);
	InitInternal = [Instance]()
		{
			if (Instance.IsValid() && Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				Instance.Pin()->From = FQTweenPropAccessor::GetScale(Instance.Pin()->Owner.Get());
			}
		};
	
	EaseInternal = [Instance]()
	{
		if (Instance.IsValid() && Instance.Pin()->Owner.IsValid())
		{
			FQTweenInstance* Ins = Instance.Pin().Get();
			const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
			const FVector NewVector = (Ins->*EaseMethodFunc)();
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				FQTweenPropAccessor::SetScale(Instance.Pin()->Owner.Get(), NewVector);
			}
		}
	};
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetCallbackColor()
{
	Type = EQTweenAction::CALLBACK_COLOR;
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);
	InitInternal = [Instance]()
		{
			if(Instance.IsValid())
			{
				Instance.Pin()->Diff = FVector(1, 0, 0);
			}
		};

	EaseInternal = [Instance]()
		{
			if(Instance.IsValid())
			{
				FQTweenInstance* Ins = Instance.Pin().Get();
				const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
				FVector NewVector = (Ins->*EaseMethodFunc)();
				float Factor = NewVector.X;
				FLinearColor toColor = TweenColor(Instance.Pin().ToSharedRef(), Factor);
				if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
				{
					FQTweenPropAccessor::SetColor(Instance.Pin()->Owner.Get(), toColor);
				}
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

TSharedRef<FQTweenInstance> FQTweenInstance::SetTo(const FVector& InTo)
{
	if (bHasInitialized)
	{
		this->To = InTo;
		Diff = InTo - From;
	}
	else
	{
		this->To = InTo;
	}
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetFrom(const FVector& InFrom)
{
	if (Owner != nullptr && Owner.IsValid())
	{
		Init();
	}
	this->From = InFrom;
	Diff = To - InFrom;
	DiffDiv2 = Diff * 0.5f;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOvershoot(float bInOver)
{
	this->Overshoot = bInOver;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPeriod(float InPeriod)
{
	this->Period = InPeriod;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDiff(const FVector& InDiff)
{
	this->Diff = InDiff;
	this->DiffDiv2 = InDiff * 0.5f;
	To = From + Diff;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetHasInitialized(bool InInitialized)
{
	bHasInitialized = InInitialized;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPassed(float InPassed)
{
	Passed = InPassed;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetTime(float InTime)
{
	float PassedTimeRatio = Passed / InTime;
	Passed = InTime * PassedTimeRatio;
	Time = InTime;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetSpeed(float InTime)
{
	this->Speed = InTime;
	
	if (bHasInitialized)
	{
		InitSpeed();
	}

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetRepeat(int32 InRepeat)
{
	LoopCount = InRepeat;
	if (InRepeat > 1 && LoopType == EQTweenLoopType::Once
		|| (InRepeat < 0 && LoopType == EQTweenLoopType::Once))
	{
		LoopType = EQTweenLoopType::Clamp;
	}

	if (Type == EQTweenAction::CALL_BACK || Type == EQTweenAction::CALLBACK_COLOR)
		SetOnCompleteOnRepeat(true);
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopType(EQTweenLoopType ELoopType)
{
	this->LoopType = ELoopType;
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

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopCount(int32 InCount)
{
	LoopCount = InCount;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopOnce()
{
	LoopType = EQTweenLoopType::Once;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopClamp(int32 InLoops)
{
	LoopType = EQTweenLoopType::Clamp;
	LoopCount = InLoops == 0 ? 1 : InLoops;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetLoopPingPong(int32 InLoops)
{
	LoopType = EQTweenLoopType::PingPong;
	LoopCount = InLoops < 1 ? -1 : InLoops * 2;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetPoint(const FVector& InPoint)
{
	Point = InPoint;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOnCompleteOnRepeat(bool bIsOn)
{
	bOnCompleteOnRepeat = bIsOn;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetOnCompleteOnStart(bool bIsOn)
{
	bOnCompleteOnStart = bIsOn;
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetDirection(float InDir)
{
	if (Direction != -1.f && Direction != 1.f)
	{
		return SharedThis<FQTweenInstance>(this);
	}

	if (Direction != InDir)
	{
		Direction = InDir;
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
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);
	InitInternal = [Instance]()
	{
		if (Instance.IsValid())
		{
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				Instance.Pin()->From = FVector(FQTweenPropAccessor::GetAlpha(Instance.Pin()->Owner.Get()), 0, 0);
			}
		}
	};

	EaseInternal = [Instance]()
	{
		if (Instance.IsValid())
		{
			FQTweenInstance* Ins = Instance.Pin().Get();
			const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
			FVector NewVector = (Ins->*EaseMethodFunc)();
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				FQTweenPropAccessor::SetAlpha(Instance.Pin()->Owner.Get(), NewVector.X);
			}
		}
	};

	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetColor()
{
	Type = EQTweenAction::COLOR;
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);
	
	InitInternal = [Instance]()
		{
			if(Instance.IsValid())
			{
				if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
				{
					Instance.Pin()->SetFromColor(FQTweenPropAccessor::GetColor(Instance.Pin()->Owner.Get()));
				}
			}
		};

	EaseInternal = [Instance]()
		{
			if(Instance.IsValid())
			{
				FQTweenInstance* Ins = Instance.Pin().Get();
				const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
				FVector NewVector = (Ins->*EaseMethodFunc)();
				float Factor = NewVector.X;
				FLinearColor toColor = TweenColor(Instance.Pin().ToSharedRef(), Factor);
				if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
				{
					FQTweenPropAccessor::SetColor(Instance.Pin()->Owner.Get(), toColor);
				}
			}
		};
	return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetMove()
{
	Type = EQTweenAction::MOVE;
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);
	
    InitInternal = [Instance]()
    {
	    if (Instance.IsValid())
	    {
		    if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
		    {
			    Instance.Pin()->From = FQTweenPropAccessor::GetPosition(Instance.Pin()->Owner.Get());
		    }
	    }
    };
	EaseInternal = [Instance]()
	{
		if (Instance.IsValid())
		{
			FQTweenInstance* Ins = Instance.Pin().Get();
			const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
			FVector NewVector = (Ins->*EaseMethodFunc)();
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				FQTweenPropAccessor::SetPosition(Instance.Pin()->Owner.Get(), NewVector);
			}
		}
	};
    return SharedThis<FQTweenInstance>(this);
}

TSharedRef<FQTweenInstance> FQTweenInstance::SetRotate()
{
	Type = EQTweenAction::ROTATE;
	TWeakPtr<FQTweenInstance> Instance = SharedThis<FQTweenInstance>(this);

	InitInternal = [Instance]()
	{
		if (Instance.IsValid())
		{
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				Instance.Pin()->From = FQTweenPropAccessor::GetRotate(Instance.Pin()->Owner.Get());
				Instance.Pin()->To = FVector(
					UQTweenEngineSubsystem::ClosestRot(Instance.Pin()->From.X, Instance.Pin()->To.X),
					UQTweenEngineSubsystem::ClosestRot(Instance.Pin()->From.Y, Instance.Pin()->To.Y),
					UQTweenEngineSubsystem::ClosestRot(Instance.Pin()->From.Z, Instance.Pin()->To.Z));
			}
		}
	};

	EaseInternal = [Instance]()
	{
		if (Instance.IsValid())
		{
			FQTweenInstance* Ins = Instance.Pin().Get();
			const auto& EaseMethodFunc = Instance.Pin()->EaseMethod;
			FVector NewVector = (Ins->*EaseMethodFunc)();
			if (Instance.Pin()->Owner != nullptr && Instance.Pin()->Owner.IsValid())
			{
				FQTweenPropAccessor::SetRotate(Instance.Pin()->Owner.Get(), NewVector);
			}
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
	FVector NewVector = (this->*EaseMethod)();
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

bool FQTweenInstance::IsValid(const TSharedPtr<FQTweenInstance> Instance)
{
	if(!Instance.IsValid())
	{
		return false;
	}
	
	if (nullptr == Instance->Owner || !Instance->Owner.IsValid())
	{
		return false;
	}

	return true;
}

bool FQTweenInstance::IsValid(const FQTweenInstance* Instance)
{
	if(Instance == nullptr)
	{
		return false;
	}
	
	if (nullptr == Instance->Owner || !Instance->Owner.IsValid())
	{
		return false;
	}

	return true;
}

void FQTweenInstance::SetEaseInternal(EQTweenEasingFunc EasingFunc, EQTweenEasingType InType)
{
	if (int32 Index = static_cast<int32>(EasingFunc); 0 <= Index && Index < static_cast<int32>(EQTweenEasingFunc::Max))
		CurEasing = EasingFuncList[Index];
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
	SCOPED_NAMED_EVENT(QTween_TweenOnCurve, FColor::Cyan);
	float Ratio = RatioPassed;
	if (AnimCurve.IsValid() && AnimCurve->IsValidLowLevel())
	{
		Ratio = AnimCurve->GetFloatValue(RatioPassed);
	}

	return From + Diff * Ratio;
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
	SCOPED_NAMED_EVENT(QTween_TweenbyEasingType, FColor::Orange);
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

FLinearColor FQTweenInstance::TweenColor(TSharedRef<FQTweenInstance> Instance, float Factor)
{
	FVector RGB = Instance->Point * Factor + Instance->Axis * (1 - Factor);
	float Alpha = Instance->To.Y * Factor + Instance->From.Y * (1 - Factor);
	return FLinearColor(RGB.X, RGB.Y, RGB.Z, Alpha);
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
