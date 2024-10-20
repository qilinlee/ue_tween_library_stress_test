// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QTween.h"


/**
 * FQTweenSequence
 */
class QTWEEN_API FQTweenSequence : public FQTweenBase
{
	friend class UQTweenEngineSubsystem;
protected:
	TSharedPtr<FQTweenSequence> Previous;
	TSharedPtr<FQTweenSequence> Current;
	TSharedPtr<FQTweenInstance> Tween;
	
	float TotalDelay;
	float TimeScale;
	
	bool bToggle;
private:
	int32 DebugIter;
public:
	FQTweenSequence();
	
	void Reset();
	void Init(uint32 id, uint32 GlobalCounter);
	TSharedPtr<FQTweenSequence> Append(float Delay) const;
	TSharedPtr<FQTweenSequence> Append(TSharedPtr<FQTweenInstance> Tween);
	TSharedPtr<FQTweenSequence> Insert(TSharedPtr<FQTweenInstance> Tween);
	TSharedPtr<FQTweenSequence> SetScale(float Scale);
	TSharedPtr<FQTweenSequence> Reverse();
private:
	TSharedPtr<FQTweenSequence> AddOn();
	float AddPreviousDelays() const;
	void SetScaleRecursive(TSharedPtr<FQTweenSequence> Seq, float TimeScale, int Count);
};
