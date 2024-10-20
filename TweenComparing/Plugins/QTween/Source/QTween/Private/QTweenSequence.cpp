// Fill out your copyright notice in the Description page of Project Settings.


#include "QTweenSequence.h"
#include "QTweenEngineSubsystem.h"
#include "QTween.h"

FQTweenSequence::FQTweenSequence()
	: FQTweenBase()
	, Previous(nullptr)
	, Current(nullptr)
	, Tween(nullptr)
	, TotalDelay(0)
	, TimeScale(1.f)
	, bToggle(false)
	, DebugIter(0)
{

}

void FQTweenSequence::Reset()
{
	Previous = nullptr;
	Tween = nullptr;
	TotalDelay = 0.f;
}

void FQTweenSequence::Init(uint32 InId, uint32 GlobalCounter)
{
	Reset();
	Id = InId;
	Counter = GlobalCounter;
	Current = StaticCastSharedRef<FQTweenSequence>(AsShared());
}

TSharedPtr<FQTweenSequence> FQTweenSequence::Append(float delay) const
{
	if(Current)
	{
		Current->TotalDelay += delay;
	}

	return Current;
}

TSharedPtr<FQTweenSequence> FQTweenSequence::Append(TSharedPtr<FQTweenInstance> InTween)
{
	if (nullptr != Current)
	{
		Current->Tween = InTween;
		Current->TotalDelay = AddPreviousDelays();
		InTween->SetDelay(Current->TotalDelay);
		AddOn();
	}
	return Current;
}

TSharedPtr<FQTweenSequence> FQTweenSequence::Insert(TSharedPtr<FQTweenInstance> InTween)
{
	if (nullptr != Current)
	{
		Current->Tween = InTween;
		InTween->SetDelay(AddPreviousDelays());
		AddOn();
	}
	return Current;
}

TSharedPtr<FQTweenSequence> FQTweenSequence::SetScale(float Scale)
{
	SetScaleRecursive(Current, Scale, 500);
	return AddOn();
}

TSharedPtr<FQTweenSequence> FQTweenSequence::Reverse()
{
	return AddOn();
}

TSharedPtr<FQTweenSequence> FQTweenSequence::AddOn()
{
	Current->bToggle = true;
	TSharedPtr<FQTweenSequence> Last = Current;
	Current->Previous = Current;
	Current = UQTweenEngineSubsystem::Get()->Sequence(true);
	Last->bToggle = false;
	Current->TotalDelay = Last->TotalDelay;
	Current->DebugIter = Last->DebugIter + 1;
	return Current;
}

float FQTweenSequence::AddPreviousDelays() const
{
	TSharedPtr<FQTweenSequence> Prev = Current->Previous;
	if (nullptr != Prev && Prev->Tween != nullptr)
	{
		return Current->TotalDelay + Prev->Tween->Time;
	}

	return Current->TotalDelay;
}

void FQTweenSequence::SetScaleRecursive(TSharedPtr<FQTweenSequence> Seq, float InTimeScale, int Count)
{
	if (Count > 0)
	{
		this->TimeScale = InTimeScale;
		Seq->TotalDelay *= InTimeScale;
		if (Seq->Tween)
		{
			if (Seq->Tween->Time != 0.f)
			{
				Seq->Tween->SetTime(Seq->Tween->Time * InTimeScale);
			}

			Seq->Tween->SetDelay(Seq->Tween->Delay * InTimeScale);
		}

		if (Seq->Previous != nullptr)
		{
			SetScaleRecursive(Seq->Previous, InTimeScale, Count - 1);
		}
	}
}
