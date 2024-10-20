// Fill out your copyright notice in the Description page of Project Settings.


#include "QTweenEngineSubsystem.h"
#include "Curves/CurveFloat.h"
#include "Curves/RichCurve.h"
#include "QTween.h"
#include "QTweenSequence.h"
#include "Components/widget.h"
#include "GameFramework/Actor.h"
#include "Styling/UMGCoreStyle.h"
#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#endif

DEFINE_LOG_CATEGORY(LogQTween);

UQTweenEngineSubsystem* UQTweenEngineSubsystem::Get()
{
    UQTweenEngineSubsystem* Subsystem = GEngine->GetEngineSubsystem<UQTweenEngineSubsystem>();
    check(Subsystem);
    return Subsystem;
}

void UQTweenEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    check(!bInitialized);
    bInitialized = true;

    Super::Initialize(Collection);

	FQTweenInstance::InitEasingMethod();

	CurvePunch = NewObject<UCurveFloat>(this);
	auto ListPunch = CurvePunch->GetCurves();
	if(ListPunch.Num() > 0)
	{
		ListPunch[0].CurveName = TEXT("Punch"); 
		if(FRichCurve* RichCurvePunch = StaticCast<FRichCurve*>(ListPunch[0].CurveToEdit))
		{
			RichCurvePunch->AddKey(0.0f, 0.0f);
			RichCurvePunch->AddKey(0.112586f, 0.9976035f);
			RichCurvePunch->AddKey(0.3120486f, -0.1720615f);
			RichCurvePunch->AddKey(0.4316337f, 0.07030682f);
			RichCurvePunch->AddKey(0.5524869f, -0.03141804f);
			RichCurvePunch->AddKey(0.6549395f, 0.003909959f);
			RichCurvePunch->AddKey(0.770987f, -0.009817753f);
			RichCurvePunch->AddKey(0.8838775f, 0.001939224f);
			RichCurvePunch->AddKey(1.0f, 0.0f);
		}
	}

	CurveShake = NewObject<UCurveFloat>(this);
	TArray<FRichCurveEditInfo> ListShake = CurveShake->GetCurves();
	if(ListShake.Num() > 0)
	{
		ListShake[0].CurveName = TEXT("Shake");
		if(FRichCurve* RichCurveShake = StaticCast<FRichCurve*>(ListShake[0].CurveToEdit))
		{
			RichCurveShake->AddKey(0.0f, 0.0f);
			RichCurveShake->AddKey(0.25f, 1.f);
			RichCurveShake->AddKey(0.75f, -1.f);
			RichCurveShake->AddKey(1.f, 0.f);
		}
	}

	DtActual = 0.f;
	DtEstimated = -1.f;
	DtManual = 0.f;

	MaxTweenSearch = -1;
	MaxTweens = 200;
	MaxSequences = 50;

	ThrowErrors = 0;
	TweenEmpty = nullptr;
	FinishedCnt = 0;
	MaxTweenReached = 0;
	StartSearch = 0;
}

void UQTweenEngineSubsystem::Deinitialize()
{

    Super::Deinitialize();
}

ETickableTickType UQTweenEngineSubsystem::GetTickableTickType() const
{
    // By default (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
    return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Conditional;
}

bool UQTweenEngineSubsystem::IsAllowedToTick() const
{
    return bInitialized && !IsTemplate();
}

void UQTweenEngineSubsystem::Tick(float DeltaTime)
{
    checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));

	Update(DeltaTime);
}

TStatId UQTweenEngineSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(QTween, STATGROUP_Tickables);
}

bool UQTweenEngineSubsystem::IsTickableInEditor() const
{
    return true;
}

void UQTweenEngineSubsystem::Init()
{
	Init(MaxTweens);
}

void UQTweenEngineSubsystem::Init(int32 maxSimultaneousTweens)
{
	Init(maxSimultaneousTweens, MaxSequences);
}

void UQTweenEngineSubsystem::Init(int32 MaxSimultaneousTweens, int32 MaxSimultaneousSeqs)
{
	if (PoolTweens.Num() > 0 || HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	TweenEmpty = NewObject<UACMTweenEmpty>(this);

	MaxTweens = MaxSimultaneousTweens;
	MaxSequences = MaxSimultaneousSeqs;
	
	PoolTweens.Reserve(MaxTweens);
	TweensPtr.Reserve(MaxTweens);
	TweensFinished.Reserve(MaxTweens);
	TweensFinishedIds.Reserve(MaxTweens);

	for (int32 idx = 0; idx < MaxTweens; ++idx)
	{
		FQTweenInstance& Instance = PoolTweens.AddDefaulted_GetRef();
		TweensPtr.Add(MakeShareable(&Instance));
		TweensPtr.Last()->ResetTween();
		
		TweensFinished.Add(-1);
		TweensFinishedIds.Add(-1);
	}

	Sequences.Reserve(MaxSequences);
	SequencesPtr.Reserve(MaxSequences);
	for (int32 idx = 0; idx < MaxSequences; ++idx)
	{
		FQTweenSequence& Seq = Sequences.AddDefaulted_GetRef();
		SequencesPtr.Add(MakeShareable(&Seq));
	}
}

int32 UQTweenEngineSubsystem::GetCountTweensRunning()
{
	int32 count = 0;
	for (int32 index = 0; index < MaxTweenSearch; ++index)
	{
		if (PoolTweens[index].bToggle)
		{
			++count;
		}
	}
	return count;
}

void UQTweenEngineSubsystem::Reset()
{
	for (int32 idx = 0; idx < PoolTweens.Num(); ++idx)
	{
		PoolTweens[idx].ResetTween();
	}

	PoolTweens.Empty();
	TweensPtr.Empty();
	//TweenEmpty = nullptr;
}

void UQTweenEngineSubsystem::Update(float InElapsedTime)
{
	if (FrameRendered == GFrameNumber)
	{
		return;
	}

	SCOPED_NAMED_EVENT(UQTweenEngineSubsystem_Update, FColor::Blue);

	// make sure update is only called once per frame
	Init();

	DtEstimated = DtEstimated < 0.f ? 0.f : InElapsedTime;
	DtActual = InElapsedTime;

	MaxTweenReached = 0;
	FinishedCnt = 0;
	
	for (int32 i = 0; i <= MaxTweenSearch && i < MaxTweens; i++)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (Tween.bToggle)
		{
			MaxTweenReached = i;

			if (!FQTweenInstance::IsValid(&Tween))
				RemoveTween(i);
			else
			{
				if (Tween.UpdateInternal())
				{ // returns true if the Tween is finished with it's loop
					TweensFinished[FinishedCnt] = i;
					TweensFinishedIds[FinishedCnt] = Tween.GetUniqueID();
					FinishedCnt++;
				}
			}
		}
	}

	
	FrameRendered = GFrameNumber;

	for (int32 i = 0; i < FinishedCnt; i++)
	{
		uint32 j = TweensFinished[i];
		FQTweenInstance& Tween = PoolTweens[j];

		if (Tween.GetUniqueID() == TweensFinishedIds[i])
		{
			if (Tween.bHasExtraOnCompletes
				&& Tween.Owner.IsValid()
				&& Tween.OnComplete.IsBound())
			{
				Tween.OnComplete.Broadcast();
			}

			RemoveTween(j);
		}
	}
}

void UQTweenEngineSubsystem::RemoveTween(int Index, uint64 UniqueId)
{
	if (PoolTweens.IsValidIndex(Index)
		&& PoolTweens[Index].GetUniqueID() == UniqueId)
	{
		RemoveTween(Index);
	}
}

void UQTweenEngineSubsystem::RemoveTween(int Index, bool bShouldReset)
{
	if (!PoolTweens.IsValidIndex(Index))
	{
		return;
	}

	if (PoolTweens[Index].bToggle)
	{
		FQTweenInstance& Tween = PoolTweens[Index];
		Tween.Counter = -1;
		//if (Tween.destroyOnComplete 
		//	&& Tween.owner != nullptr 
		//	&& Tween.owner->IsValidLowLevel()
		//	&& Tween.owner != TweenEmpty)
		//{
		//	Tween.owner = nullptr;
		//}

		if (bShouldReset)
		{
			Tween.ResetTween();
		}

		StartSearch = Index;
		if (Index + 1 >= MaxTweenSearch)
		{
			StartSearch = 0;
		}
	}
}

void UQTweenEngineSubsystem::CancelAll()
{
	CancelAll(false);
}

void UQTweenEngineSubsystem::CancelAll(bool bCallComplete)
{
	Init();
	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		if (PoolTweens[i].Owner != nullptr
			&& PoolTweens[i].Owner->IsValidLowLevel())
		{
			if (bCallComplete && PoolTweens[i].OnComplete.IsBound())
			{
				PoolTweens[i].OnComplete.Broadcast();
			}

			RemoveTween(i);
		}
	}
}

void UQTweenEngineSubsystem::Cancel(const UObject* Obj)
{
	Cancel(Obj, false);
}

void UQTweenEngineSubsystem::Cancel(const UObject* Obj, bool bCallOnComplete, EQTweenAction MatchType /*= TweenAction::NONE*/)
{
	Init();
	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (MatchType == EQTweenAction::NONE || MatchType == Tween.Type)
		{
			if (Tween.bToggle && Tween.Owner == Obj)
			{
				if (bCallOnComplete && Tween.OnComplete.IsBound())
				{
					Tween.OnComplete.Broadcast();
				}

				RemoveTween(i);
			}
		}
	}
}

void UQTweenEngineSubsystem::Cancel(const UObject* Obj, uint64 UniqueId, bool bCallOnComplete /*= false*/)
{
	if (0 == UniqueId)
	{
		return;
	}

	Init();
	
	uint32 BackId = 0, BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);

	if (!PoolTweens.IsValidIndex(BackId))
	{
		return;
	}
	
	FQTweenInstance& Tween = PoolTweens[BackId];

	if (Tween.Owner == nullptr
		|| (Tween.Owner == Obj && Tween.Counter == BackCounter))
	{
		if (bCallOnComplete && Tween.OnComplete.IsBound())
		{
			Tween.OnComplete.Broadcast();
		}

		RemoveTween(static_cast<int32>(BackId));
	}
}

void UQTweenEngineSubsystem::Cancel(uint64 UniqueId)
{
	Cancel(UniqueId, false);
}

void UQTweenEngineSubsystem::Cancel(uint64 UniqueId, bool bCallOnComplete)
{
	Init();
	
	uint32 BackId = 0;
	uint32 BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);

	if (static_cast<int32>(BackId) > PoolTweens.Num() - 1)
	{ // sequence
		int32 SequenceId = static_cast<int32>(BackId) - PoolTweens.Num();
		FQTweenSequence& Seq = Sequences[SequenceId];
		for (int i = 0; i < MaxSequences; i++)
		{
			if (Seq.Current->Tween != nullptr)
			{
				uint64 TweenId = Seq.Current->Tween->GetUniqueID();
				int32 TweenIndex = static_cast<int32>(TweenId & 0xFFFF);
				RemoveTween(TweenIndex);
			}

			if (Seq.Current->Previous == nullptr)
			{
				break;
			}
			Seq.Current = Seq.Current->Previous;
		}
	}
	else
	{ // Tween
		if (PoolTweens[BackId].Counter == BackCounter) 
		{
			if (bCallOnComplete
				&& PoolTweens[BackId].OnComplete.IsBound())
			{
				PoolTweens[BackId].OnComplete.Broadcast();
			}

			RemoveTween(static_cast<int>(BackId));
		}
	}
}

FQTweenHandle UQTweenEngineSubsystem::Tween(uint64 UniqueId)
{
	Init();

	uint32 BackId = 0;
	uint32 BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);
    if (TweensPtr.IsValidIndex(BackId)
        && TweensPtr[BackId]->GetUniqueID() == UniqueId
        && TweensPtr[BackId]->Counter == BackCounter)
    {
    	return FQTweenHandle(TweensPtr[BackId]);
    }

	for (int32 i = 0; i <= MaxTweenSearch; i++) 
	{
		if (TweensPtr[i]->GetUniqueID() == UniqueId
			&& TweensPtr[i]->Counter == BackCounter)
		{
			return FQTweenHandle(TweensPtr[i]);
		}
	}

	return FQTweenHandle::Invalid;
}

TArray<FQTweenHandle> UQTweenEngineSubsystem::Tweens(UObject* Obj)
{
	TArray<FQTweenHandle> result;

	if (nullptr != Obj && Obj->IsValidLowLevel())
	{
		for (int32 i = 0; i < MaxTweenSearch; ++i)
		{
			if (TweensPtr[i]->bToggle
				&& TweensPtr[i]->Owner == Obj)
			{
				result.Add(FQTweenHandle(TweensPtr[i]));
			}
		}
	}

	return result;
}

void UQTweenEngineSubsystem::Pause(uint64 UniqueId)
{
	uint32 BackId = 0, BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);
	if (PoolTweens.IsValidIndex(BackId)
		&& PoolTweens[BackId].Counter == BackCounter)
	{
		PoolTweens[BackId].Pause();
	}
}

void UQTweenEngineSubsystem::Pause(UObject* Obj)
{
	if (nullptr == Obj || !Obj->IsValidLowLevel())
	{
		return;
	}

	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (Tween.bToggle && Tween.Owner == Obj)
		{
			Tween.Pause();
		}
	}
}

void UQTweenEngineSubsystem::PauseAll()
{
	Init();

	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		PoolTweens[i].Pause();
	}
}

bool UQTweenEngineSubsystem::IsPaused(UObject* Obj)
{
	if (nullptr == Obj || !Obj->IsValidLowLevel())
	{
		for (int32 i = 0; i < MaxTweenSearch; ++i)
		{
			FQTweenInstance& Tween = PoolTweens[i];
			if (Tween.Direction == 0.f)
			{
				return true;
			}
		}

		return false;
	}

	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (Tween.Owner == Obj
			&& Tween.Direction == 0.f)
		{
			return true;
		}
	}

	return false;
}

bool UQTweenEngineSubsystem::IsPuased(uint64 UniqueId)
{
	uint32 BackId = 0, BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);
	if (static_cast<int32>(BackId) >= MaxTweens || !PoolTweens.IsValidIndex(BackId))
	{
		return false;
	}

	FQTweenInstance& Tween = PoolTweens[BackId];
	if (Tween.Direction == 0.f)
	{
		return true;
	}

	return false;
}

void UQTweenEngineSubsystem::ResumeAll()
{
	Init();
	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		PoolTweens[i].Resume();
	}
}

void UQTweenEngineSubsystem::Resume(uint64 UniqueId)
{
	uint32 BackId = 0, BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);
	if (PoolTweens.IsValidIndex(BackId) 
		&& PoolTweens[BackId].Counter == BackCounter)
	{
		PoolTweens[BackId].Resume();
	}
}

void UQTweenEngineSubsystem::Resume(UObject* obj)
{
	if (nullptr == obj || !obj->IsValidLowLevel())
		return;

	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (Tween.Owner == obj)
		{
			Tween.Resume();
		}
	}
}

bool UQTweenEngineSubsystem::IsTweening(UObject* Obj)
{
	if (nullptr == Obj || !Obj->IsValidLowLevel())
	{
		for (int32 i = 0; i < MaxTweenSearch; ++i)
		{
			FQTweenInstance& Tween = PoolTweens[i];
			if (Tween.bToggle)
			{
				return true;
			}
		}

		return false;
	}

	for (int32 i = 0; i <= MaxTweenSearch; ++i)
	{
		FQTweenInstance& Tween = PoolTweens[i];
		if (Tween.Owner == Obj
			&& Tween.bToggle)
		{
			return true;
		}
	}

	return false;
}

bool UQTweenEngineSubsystem::IsTweening(uint64 UniqueId)
{
	uint32 BackId = 0, BackCounter = 0;
	FQTweenBase::BreakUniqueID(UniqueId, BackId, BackCounter);
	if (static_cast<int32>(BackId) >= MaxTweens || !PoolTweens.IsValidIndex(BackId))
	{
		return false;
	}

	FQTweenInstance& Tween = PoolTweens[BackId];
	if (Tween.bToggle)
	{
		return true;
	}

	return false;
}

TSharedPtr<FQTweenInstance> UQTweenEngineSubsystem::Options()
{
	Init();

	bool bFound = false;
	int32 i = StartSearch;
	for (int32 j = 0; j <= MaxTweens; i++)
	{
		if (j >= MaxTweens)
		{
			FString str = FString::Printf(
				TEXT(
					"QTween - You have run out of available spaces for tweening. To avoid this error increase the number of spaces to available for tweening when you initialize the QTween class ex: LeanTween.init( %d );"),
				MaxTweens * 2);
			LogError(str);
			return nullptr;
		}

		if (i >= MaxTweens)
			i = 0;

		if (!PoolTweens[i].bToggle)
		{
			if (i + 1 > MaxTweenSearch && i + 1 < MaxTweens)
				MaxTweenSearch = i + 1;
			StartSearch = i + 1;
			bFound = true;
			break;
		}

		j++;
	}

	if (bFound == false)
	{
		LogError(TEXT("no available Tween found!"));
		return nullptr;
	}

	IncreaseGlobalCounter();

	PoolTweens[i].SetID(static_cast<uint32>(i), GlobalCounter);

	return TweensPtr[i];
}

TSharedPtr<FQTweenInstance> UQTweenEngineSubsystem::GetTween(const FQTweenHandle& Tween)
{
	Init();

	uint32 BackId = 0;
	uint32 BackCounter = 0;
	FQTweenBase::BreakUniqueID(Tween.UniqueID, BackId, BackCounter);
	if (TweensPtr.IsValidIndex(BackId)
		&& TweensPtr[BackId]->GetUniqueID() == Tween.UniqueID
		&& TweensPtr[BackId]->Counter == BackCounter)
	{
		return TweensPtr[BackId];
	}

	for (int32 i = 0; i <= MaxTweenSearch; i++) 
	{
		if (TweensPtr[i]->GetUniqueID() == Tween.UniqueID
			&& TweensPtr[i]->Counter == BackCounter)
		{
			return TweensPtr[i];
		}
	}

	return nullptr;
}

void UQTweenEngineSubsystem::IncreaseGlobalCounter()
{
	GlobalCounter++;
	if (GlobalCounter > 0x8000)
	{
		GlobalCounter = 0;
	}
}

FQTweenHandle UQTweenEngineSubsystem::PushNewTween(UObject* Obj, const FVector& To, float Time, TSharedPtr<FQTweenInstance> Tween)
{
	Init(MaxTweens);
	if (nullptr == Obj || !IsValid(Obj) || !Obj->IsValidLowLevel())
	{
		return FQTweenHandle::Invalid;
	}

	if(Tween.IsValid())
	{
		Tween->Owner = Obj;
		Tween->bToggle = true;
		Tween->To = To;
		Tween->Time = Time;

		if (Time <= 0.f)
		{
			Tween->UpdateInternal();
		}

		return FQTweenHandle(Tween);
	}

	return FQTweenHandle::Invalid;
}

FQTweenHandle UQTweenEngineSubsystem::PushNewTween(UObject* Obj, const FVector& To, float Time, const FQTweenHandle& Tween)
{
	TSharedPtr<FQTweenInstance> TweenInstance = UQTweenEngineSubsystem::GetTween(Tween);
	return PushNewTween(Obj, To, Time, TweenInstance);
}

TSharedPtr<FQTweenSequence> UQTweenEngineSubsystem::Sequence(bool initSeq /*= true*/)
{
	Init(MaxTweens);

	for (int i = 0; i < Sequences.Num(); i++)
	{
		if (Sequences[i].Tween == nullptr || !Sequences[i].Tween->bToggle)
		{
			if (Sequences[i].bToggle == false)
			{
				FQTweenSequence& Seq = Sequences[i];
				if (initSeq)
				{
					Seq.Init(static_cast<uint32>(i + PoolTweens.Num()), GlobalCounter);

					IncreaseGlobalCounter();
				}
				else
				{
					Seq.Reset();
				}

				return SequencesPtr[i];
			}
		}
	}

	return nullptr;
}

FQTweenHandle UQTweenEngineSubsystem::Alpha(UObject* obj, float to, float time)
{
	TSharedPtr<FQTweenInstance> t = Options()->SetAlpha();
	return PushNewTween(obj, FVector(to, 0, 0), time, t);
}

FQTweenHandle UQTweenEngineSubsystem::Colour(UObject* obj, const FLinearColor& to, float time)
{
	TSharedPtr<FQTweenInstance> Tween = Options()->SetColor()
		->SetPoint(FVector(to.R, to.G, to.B));
	return PushNewTween(obj, FVector(1.0f, to.A, 0.f), time, Tween);
}

FQTweenHandle UQTweenEngineSubsystem::Move(UObject* obj, FVector to, float time)
{
	TSharedPtr<FQTweenInstance> Tween = Options()->SetMove();
	return PushNewTween(obj, to, time, Tween);
}

FQTweenHandle UQTweenEngineSubsystem::MoveBy(UObject* obj, FVector delta, float time)
{
	if (obj->IsA(AActor::StaticClass()))
	{
		AActor* actor = Cast<AActor>(obj);
		if (nullptr != actor)
			return Move(obj, actor->GetActorLocation() + delta, time);
	}
	else if (obj->IsA(UWidget::StaticClass()))
	{
		UWidget* widget = Cast<UWidget>(obj);
		if (nullptr != widget)
		{
			FVector from(widget->GetRenderTransform().Translation, 0.f);
			return Move(obj, from + delta, time);
		}
	}

	return Move(obj, delta, time);
}

FQTweenHandle UQTweenEngineSubsystem::Rotate(UObject* obj, FVector to, float time)
{
	return PushNewTween(obj, to, time, Options()->SetRotate());
}


FQTweenHandle UQTweenEngineSubsystem::Scale(UObject* obj, FVector to, float time)
{
	return PushNewTween(obj, to, time, Options()->SetScale());
}

FQTweenHandle UQTweenEngineSubsystem::TweenFloat(float from, float to, float time)
{
	return TweenFloatFromTo(TweenEmpty, from, to, time);
}

FQTweenHandle UQTweenEngineSubsystem::TweenFloatFromTo(UObject* obj, float from, float to, float time)
{
	return TweenVector(obj, FVector(from, 0, 0), FVector(to, 0, 0), time);
}

FQTweenHandle UQTweenEngineSubsystem::TweenVector2D(UObject* obj, FVector2D from, FVector2D to, float time)
{
	return TweenVector(obj, FVector(from, 0), FVector(to, 0), time);
}

FQTweenHandle UQTweenEngineSubsystem::TweenVector(UObject* obj, FVector from, FVector to, float time)
{
	TSharedPtr<FQTweenInstance> Tween = Options()->SetCallback()
		->SetFrom(from);
	return PushNewTween(obj, to, time, Tween);

}

FQTweenHandle UQTweenEngineSubsystem::TweenColor(UObject* obj, FLinearColor from, FLinearColor to, float time)
{
	TSharedPtr<FQTweenInstance> Tween = Options()->SetCallbackColor()
		->SetPoint(FVector(to.R, to.G, to.B))
		->SetFromColor(from)
		->SetHasInitialized(false);
	return PushNewTween(obj, FVector(1, to.A, 0.f), time, Tween);
}

//float UQTweenEngineSubsystem::DoTweenAsset(UObject* owner, const UACMTweenAsset* asset, bool forward, float delay /*= 0.f*/)
//{
//	if (nullptr == asset)
//		return 0.f;
//
//	float max = 0.f;
//	for (int32 idx = 0; idx < asset->Tweens.Num(); ++idx)
//	{
//		const FACMTweenInfo& info = asset->Tweens[idx];
//		FQTweenHandle Tween = nullptr;
//
//		const FVector& from = forward ? info.StartValue : info.EndValue;
//		const FVector& to = forward ? info.EndValue : info.StartValue;
//		max = FMath::Max(max, info.Duration + info.DelayTime + delay);
//		switch (info.TweenTargetProp)
//		{
//		case EACMTweenAction::ALPHA:
//			Tween = Alpha(owner, to.X, info.Duration)->SetFrom(from);
//			break;
//		case EACMTweenAction::MOVE:
//			Tween = Move(owner, to, info.Duration)->SetFrom(from);
//			break;
//		case EACMTweenAction::COLOR:
//		{
//			const FLinearColor& colFrom = forward ? info.StartColor : info.EndColor;
//			const FLinearColor& colTo = forward ? info.EndColor : info.StartColor;
//			Tween = Colour(owner, colTo, info.Duration)->SetFromColor(colFrom);
//			break;
//		}
//		case EACMTweenAction::CALLBACK_COLOR:
//		{
//			const FLinearColor& colFrom = forward ? info.StartColor : info.EndColor;
//			const FLinearColor& colTo = forward ? info.EndColor : info.StartColor;
//			Tween = TweenColor(owner, from, to, info.Duration)->SetFromColor(colFrom);
//			break;
//		}
//		case EACMTweenAction::ROTATE:
//			Tween = Rotate(owner, to, info.Duration)->SetFrom(from);
//			break;
//		case EACMTweenAction::SCALE:
//			Tween = Scale(owner, to, info.Duration)->SetFrom(from);
//			break;
//		case EACMTweenAction::CALL_BACK:
//			Tween = TweenVector(owner, from, to, info.Duration)->SetFrom(from);
//			break;
//		default:
//			break;
//		}
//
//		if (nullptr != Tween)
//		{
//			Tween->SetDelay(info.DelayTime + delay)
//				->SetEase(info.EaseMode)
//				->SetOnCompleteOnRepeat(info.CallOnCompleteWhenRepeat)
//				->SetOnCompleteOnStart(info.CallOnCompleteWhenStart)
//				->SetLoopType(info.LoopType)
//				->SetLoopCount(info.LoopTimes);
//		}
//	}
//	return max;
//}


TArray<FVector>& UQTweenEngineSubsystem::Add(TArray<FVector>& a, const FVector& b)
{
	for (int32 i = 0; i < a.Num(); ++i)
	{
		a[i] += b;
	}

	return a;
}

float UQTweenEngineSubsystem::ClosestRot(float From, float To)
{
	float MinusWhole = 0 - (360 - To);
	float PlusWhole = 360 + To;
	float ToDiffAbs = FMath::Abs(To - From);
	float MinusDiff = FMath::Abs(MinusWhole - From);
	float PlusDiff = FMath::Abs(PlusWhole - From);
	if (ToDiffAbs < MinusDiff && ToDiffAbs < PlusDiff)
	{
		return To;
	}

	return MinusDiff < PlusDiff ? MinusWhole : PlusWhole;
}

void UQTweenEngineSubsystem::LogError(FString error) const
{
	if (ThrowErrors)
	{
		UE_LOG(LogQTween, Error, TEXT("%s"), *error);
	}
}

#if WITH_EDITOR
void UQTweenEngineSubsystem::RegisterActiveTimer()
{
	if (!bIsActiveTimerRegistered)
	{
		UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
		if (!EditorEngine)
		{
			return;
		}

		auto TimerManager = EditorEngine->GetTimerManager();
		TimerManager->SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UQTweenEngineSubsystem::ActiveTimerTick), 1e-4, true);
		bIsActiveTimerRegistered = true;
	}
}

void UQTweenEngineSubsystem::UnregisterActiveTimer()
{
	if (bIsActiveTimerRegistered)
	{
		UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
		if (!EditorEngine)
		{
			return;
		}

		auto TimerManager = EditorEngine->GetTimerManager();
		TimerManager->ClearTimer(TimerHandle);
	}

	bIsActiveTimerRegistered = false;
}

void UQTweenEngineSubsystem::ActiveTimerTick()
{
	UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
	if (!EditorEngine)
	{
		return;
	}

	UWorld* World = EditorEngine->GetEditorWorldContext().World();
	if (!World)
	{
		return;
	}

	Tick(World->GetDeltaSeconds());
}
#endif