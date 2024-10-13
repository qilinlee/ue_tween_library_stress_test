// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QTweenSequence.generated.h"

class UQTween;

/**
 * UQTweenSequence
 */
UCLASS()
class QTWEEN_API UQTweenSequence : public UObject
{	
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	UQTweenSequence* Previous;

	UPROPERTY()
	UQTweenSequence* Current;
	
	UPROPERTY()
	UQTween* Tween;
	
	UPROPERTY()
	float TotalDelay;
	
	UPROPERTY()
	float TimeScale;
	
	UPROPERTY()
	uint32 Counter;
	
	UPROPERTY()
	bool bToggle;
private:
	int32 DebugIter;
	uint32 Id;
public:
	uint64 GetId() const;
	void Reset();
	void Init(uint32 id, uint32 GlobalCounter);
	UQTweenSequence* Append(float Delay) const;
	UQTweenSequence* Append(UQTween* Tween);
	UQTweenSequence* Insert(UQTween* Tween);
	UQTweenSequence* SetScale(float Scale);
	UQTweenSequence* Reverse();
private:
	UQTweenSequence* AddOn();
	float AddPreviousDelays() const;
	void SetScaleRecursive(UQTweenSequence* Seq, float TimeScale, int Count);
	
	
};
