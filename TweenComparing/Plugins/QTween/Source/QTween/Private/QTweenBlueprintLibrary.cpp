#include "QTweenBlueprintLibrary.h"
#include "QTween.h"
#include "QTweenEngineSubsystem.h"

bool UQTweenBlueprintLibrary::IsValidTween(const FQTweenHandle& InHandle)
{
	return InHandle.IsValid();
}

FQTweenHandle UQTweenBlueprintLibrary::Reset(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->ResetTween();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::Cancel(const FQTweenHandle& InHandle, UObject* Obj)
{
	if(InHandle.IsValid())
	{
		InHandle->Cancel(Obj);
	}
	return InHandle;
}

bool UQTweenBlueprintLibrary::IsToggled(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		return InHandle->IsToggled();
	}
	return false;
}

FQTweenHandle UQTweenBlueprintLibrary::SetToggle(const FQTweenHandle& InHandle, bool bToggle)
{
	if(InHandle.IsValid())
	{
		InHandle->SetToggle(bToggle);
	}

	return InHandle;
}

UObject* UQTweenBlueprintLibrary::GetOwner(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		return InHandle->GetOwner();
	}
	return nullptr;
}

FQTweenHandle UQTweenBlueprintLibrary::SetOwner(const FQTweenHandle& InHandle, UObject* InOwner)
{
	if(InHandle.IsValid())
	{
		InHandle->SetOwner(InOwner);
	}

	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetFollow(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->SetFollow();
	}
	
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetOffset(const FQTweenHandle& InHandle, const FVector& Offset)
{
	if(InHandle.IsValid())
	{
		InHandle->SetOffset(Offset);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetCallback(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->SetCallback();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetTarget(const FQTweenHandle& InHandle, UObject* Obj)
{
	if(InHandle.IsValid())
	{
		InHandle->SetTarget(Obj);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::UpdateNow(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->UpdateNow();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::Pause(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->Pause();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::Resume(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->Resume();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetAxis(const FQTweenHandle& InHandle, const FVector& Axis)
{
	if(InHandle.IsValid())
	{
		InHandle->SetAxis(Axis);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetDelay(const FQTweenHandle& InHandle, float DelayTime)
{
	if(InHandle.IsValid())
	{
		InHandle->SetDelay(DelayTime);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetEase(const FQTweenHandle& InHandle, EQTweenType EaseType)
{
	if(InHandle.IsValid())
	{
		InHandle->SetEase(EaseType);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetEaseCurve(const FQTweenHandle& InHandle, UCurveFloat* Curve)
{
	if(Curve && InHandle.IsValid())
	{
		InHandle->SetEaseCurve(Curve);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetOvershoot(const FQTweenHandle& InHandle, float Over)
{
	if(InHandle.IsValid())
	{
		InHandle->SetOvershoot(Over);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetPeriod(const FQTweenHandle& InHandle, float InPeriod)
{
	if(InHandle.IsValid())
	{
		InHandle->SetPeriod(InPeriod);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetScale(const FQTweenHandle& InHandle, float fScale)
{
	if(InHandle.IsValid())
	{
		InHandle->SetScale(fScale);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetTo(const FQTweenHandle& InHandle, const FVector& InTo)
{
	if(InHandle.IsValid())
	{
		InHandle->SetTo(InTo);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetFrom(const FQTweenHandle& InHandle, const FVector& InFrom)
{
	if(InHandle.IsValid())
	{
		InHandle->SetFrom(InFrom);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetDiff(const FQTweenHandle& InHandle, const FVector& Diff)
{
	if(InHandle.IsValid())
	{
		InHandle->SetDiff(Diff);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetHasInitialized(const FQTweenHandle& InHandle, bool bInitialized)
{
	if(InHandle.IsValid())
	{
		InHandle->SetHasInitialized(bInitialized);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetPassed(const FQTweenHandle& InHandle, float InPassed)
{
	if(InHandle.IsValid())
	{
		InHandle->SetPassed(InPassed);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetTime(const FQTweenHandle& InHandle, float InTime)
{
	if(InHandle.IsValid())
	{
		InHandle->SetTime(InTime);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetSpeed(const FQTweenHandle& InHandle, float InSpeed)
{
	if(InHandle.IsValid())
	{
		InHandle->SetSpeed(InSpeed);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetRepeat(const FQTweenHandle& InHandle, int32 Repeat)
{
	if(InHandle.IsValid())
	{
		InHandle->SetRepeat(Repeat);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetLoopType(const FQTweenHandle& InHandle, EQTweenLoopType eLoopType)
{
	if(InHandle.IsValid())
	{
		InHandle->SetLoopType(eLoopType);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetUseEstimatedTime(const FQTweenHandle& InHandle, bool InUseEstimatedTime)
{
	if(InHandle.IsValid())
	{
		InHandle->SetUseEstimatedTime(InUseEstimatedTime);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetIgnoreTimeScale(const FQTweenHandle& InHandle, bool bIignoreTimeScale)
{
	if(InHandle.IsValid())
	{
		InHandle->SetIgnoreTimeScale(bIignoreTimeScale);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetUseFrames(const FQTweenHandle& InHandle, bool InUseFrames)
{
	if(InHandle.IsValid())
	{
		InHandle->SetUseFrames(InUseFrames);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetUseManualTime(const FQTweenHandle& InHandle, bool InUseManualTime)
{
	if(InHandle.IsValid())
	{
		InHandle->SetUseManualTime(InUseManualTime);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetLoopCount(const FQTweenHandle& InHandle, int32 count)
{
	if(InHandle.IsValid())
	{
		InHandle->SetLoopCount(count);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetLoopOnce(const FQTweenHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		InHandle->SetLoopOnce();
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetLoopClamp(const FQTweenHandle& InHandle, int32 Loops)
{
	if(InHandle.IsValid())
	{
		InHandle->SetLoopClamp(Loops);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetLoopPingPong(const FQTweenHandle& InHandle, int32 Loops)
{
	if(InHandle.IsValid())
	{
		InHandle->SetLoopPingPong(Loops);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetPoint(const FQTweenHandle& InHandle, const FVector& Point)
{
	if(InHandle.IsValid())
	{
		InHandle->SetPoint(Point);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetOnCompleteOnRepeat(const FQTweenHandle& InHandle, bool bIsOn)
{
	if(InHandle.IsValid())
	{
		InHandle->SetOnCompleteOnRepeat(bIsOn);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetOnCompleteOnStart(const FQTweenHandle& InHandle, bool bIsOn)
{
	if(InHandle.IsValid())
	{
		InHandle->SetOnCompleteOnStart(bIsOn);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetDirection(const FQTweenHandle& InHandle, float InDir)
{
	if(InHandle.IsValid())
	{
		InHandle->SetDirection(InDir);
	}
	return InHandle;
}

FQTweenHandle UQTweenBlueprintLibrary::SetRecursive(const FQTweenHandle& InHandle, bool bUseRecursive)
{
	if(InHandle.IsValid())
	{
		InHandle->SetRecursive(bUseRecursive);
	}
	return InHandle;
}

float UQTweenBlueprintLibrary::TweenOnCurve(const FQTweenHandle& InHandle, float ratioPassed)
{
	if(InHandle.IsValid())
	{
		return InHandle->TweenOnCurve(ratioPassed);
	}
	return 0.0f;
}

void UQTweenBlueprintLibrary::TweenOnCurveVector(const FQTweenHandle& InHandle, float ratioPassed, FVector& outResult)
{
	if(InHandle.IsValid())
	{
		InHandle->TweenOnCurve(ratioPassed, outResult);
	}
}