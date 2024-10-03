// Fill out your copyright notice in the Description page of Project Settings.


#include "QTweenSequence.h"
#include "QTweenEngineSubsystem.h"
#include "QTween.h"

UQTweenSequence::UQTweenSequence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Previous(nullptr)
	, Current(nullptr)
	, Tween(nullptr)
	, TotalDelay(0)
	, TimeScale(1.f)
	, Counter(0)
	, bToggle(false)
	, DebugIter(0)
	, Id(0)
{

}

uint64 UQTweenSequence::GetId()
{
	uint64 id = (uint64)Id;
	id |= (Counter << 16);
	return id;
}

void UQTweenSequence::Reset()
{
	Previous = nullptr;
	Tween = nullptr;
	TotalDelay = 0.f;
}

void UQTweenSequence::Init(uint32 InId, uint32 GlobalCounter)
{
	Reset();
	Id = InId;
	Counter = GlobalCounter;
	Current = this;
}

UQTweenSequence* UQTweenSequence::Append(float delay)
{
	if(Current)
	{
		Current->TotalDelay += delay;
	}

	return Current;
}

UQTweenSequence* UQTweenSequence::Append(UQTween* InTween)
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

UQTweenSequence* UQTweenSequence::Insert(UQTween* InTween)
{
	if (nullptr != Current)
	{
		Current->Tween = InTween;
		InTween->SetDelay(AddPreviousDelays());
		AddOn();
	}
	return Current;
}

UQTweenSequence* UQTweenSequence::SetScale(float Scale)
{
	SetScaleRecursive(Current, Scale, 500);
	return AddOn();
}

UQTweenSequence* UQTweenSequence::Reverse()
{
	return AddOn();
}

UQTweenSequence* UQTweenSequence::AddOn()
{
	Current->bToggle = true;
	UQTweenSequence* Last = Current;
	Current->Previous = Current;
	Current = UQTweenEngineSubsystem::Get()->Sequence(true);
	Last->bToggle = false;
	Current->TotalDelay = Last->TotalDelay;
	Current->DebugIter = Last->DebugIter + 1;
	return Current;
}

float UQTweenSequence::AddPreviousDelays()
{
	UQTweenSequence* Prev = Current->Previous;
	if (nullptr != Prev && Prev->Tween != nullptr)
	{
		return Current->TotalDelay + Prev->Tween->Time;
	}

	return Current->TotalDelay;
}

void UQTweenSequence::SetScaleRecursive(UQTweenSequence* Seq, float InTimeScale, int Count)
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
