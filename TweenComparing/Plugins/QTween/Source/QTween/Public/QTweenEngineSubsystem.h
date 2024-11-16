// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Tickable.h"
#include "QTween.h"
#include "QTweenSequence.h"
#include "Logging/LogMacros.h"
#include "QTweenEngineSubsystem.generated.h"

class FQTweenSequence;

DECLARE_LOG_CATEGORY_EXTERN(LogQTween, Log, All);


/**
 * UQTweenEngineSubsystem
 */
UCLASS(BlueprintType)
class QTWEEN_API UQTweenEngineSubsystem : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static UQTweenEngineSubsystem* Get();
	
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	// FTickableGameObject implementation Begin
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsAllowedToTick() const override final;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	// FTickableGameObject implementation End


	bool IsInitialized() const { return bInitialized; }


public:
	void Init();
	void Init(int32 MaxSimultaneousTweens);
	void Init(int32 MaxSimultaneousTweens, int32 MaxSimultaneousSeqs);
	int32 GetCountTweensRunning();
	void Reset();
	void Update(float elapsedTime);
	void RemoveTween(int Index, uint64 UniqueId);
	void RemoveTween(int Index, bool bShouldReset = true);
	void CancelAll();
	void CancelAll(bool callComplete);
	void Cancel(const UObject* obj);
	void Cancel(const UObject* obj, bool callOnComplete, EQTweenAction matchType = EQTweenAction::NONE);
	void Cancel(const UObject* obj, uint64 uniqueId, bool callOnComplete = false);
	void Cancel(uint64 uniqueId);
	void Cancel(uint64 uniqueId, bool callOnComplete);

	FQTweenHandle Tween(uint64 uniqueId);
	TArray<FQTweenHandle> Tweens(UObject* obj);

	void Pause(uint64 uniqueId);
	void Pause(UObject* obj);
	void PauseAll();
	bool IsPaused(UObject* obj);
	bool IsPaused(uint64 uniqueId);

	void ResumeAll();
	void Resume(uint64 uniqueId);
	void Resume(UObject* obj);

	bool IsTweening(UObject* obj);
	bool IsTweening(uint64 uniqueId);

	TSharedPtr<FQTweenInstance> Options();

	TSharedPtr<FQTweenInstance> GetTween(const FQTweenHandle& Tween);
	FQTweenHandle PushNewTween(UObject* Obj, const FVector& To, float Time, TSharedPtr<FQTweenInstance> Tween);
	FQTweenHandle PushNewTween(UObject* Obj, const FVector& To, float Time, const FQTweenHandle& Tween);
	TSharedPtr<FQTweenSequence> Sequence(bool InInitSeq = true);

#if WITH_EDITOR
	void RegisterActiveTimer();
	void UnregisterActiveTimer();
	void ActiveTimerTick();
#endif

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle Alpha(UObject* Obj, float InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle Colour(UObject* Obj, const FLinearColor& InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle Move(UObject* Obj, FVector InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle MoveBy(UObject* Obj, FVector InDelta, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle Rotate(UObject* Obj, FVector InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween")
	FQTweenHandle Scale(UObject* Obj, FVector InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween", meta = (DisplayName = "TweenFloat", ScriptName = "TweenFloat", Keywords = "float"))
	FQTweenHandle TweenFloat(float InFrom, float InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween", meta = (DisplayName = "TweenFloatFromTo", ScriptName = "TweenFloatFromTo", Keywords = "float"))
	FQTweenHandle TweenFloatFromTo(UObject* Obj, float InFrom, float InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween", meta = (DisplayName = "TweenVector2D", ScriptName = "TweenVector2D", Keywords = "vector2d"))
	FQTweenHandle TweenVector2D(UObject* Obj, FVector2D InFrom, FVector2D InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween", meta = (DisplayName = "TweenVector", ScriptName = "TweenVector", Keywords = "vector"))
	FQTweenHandle TweenVector(UObject* Obj, FVector InFrom, FVector InTo, float InTime);

	UFUNCTION(BlueprintCallable, Category="QTween", meta = (DisplayName = "TweenColor", ScriptName = "TweenColor", Keywords = "color"))
	FQTweenHandle TweenColor(UObject* Obj, FLinearColor InFrom, FLinearColor InTo, float InTime);



	//float DoTweenAsset(UObject* owner, const UACMTweenAsset* asset, bool forward = true, float delay = 0.f);

	template<typename UserClass>
	FQTweenHandle DelayCall(float delayTime, UserClass* obj, void(UserClass::* callback)(), FName funcName)
	{
		FQTweenHandle tween = Options()
			->SetCallback()
			->SetOnComplete(obj, callback, funcName);
		return PushNewTween(obj, FVector::ZeroVector, delayTime, tween);
	}

	static TArray<FVector>& Add(TArray<FVector>& VecA, const FVector& VecB);
	static float ClosestRot(float from, float to);
public:
    UPROPERTY()
    UCurveFloat* CurvePunch;

    UPROPERTY()
    UCurveFloat* CurveShake;

    UPROPERTY()
    UObject* TweenEmpty;

	float DtActual;
	float DtEstimated;
	float DtManual;
private:
	void LogError(FString InError) const;
	void IncreaseGlobalCounter();
	void DestroyTweenInstance(FQTweenInstance* InstancePtr);
	void DestroyTweenSequence(FQTweenSequence* SequencePtr);
private:
	TArray<FQTweenInstance> PoolTweens;
	TArray<TSharedPtr<FQTweenInstance>> TweensPtr;
	TArray<FQTweenSequence> Sequences;
	TArray<TSharedPtr<FQTweenSequence>> SequencesPtr;
	
	uint8 ThrowErrors;

	int32 MaxTweenSearch;
	int32 MaxTweens;
	int32 MaxSequences;

	int32 FrameRendered = -1;
	uint32 GlobalCounter = 0;
	int32 FinishedCnt;
	int32 MaxTweenReached;
	int32 StartSearch;

	TArray<uint32> TweensFinished;
	TArray<uint64> TweensFinishedIds;

	bool bInitialized = false;
	bool bPoolInited = false;

	/** Whether the active timer is currently registered */
	bool bIsActiveTimerRegistered = false;
	FTimerHandle TimerHandle;


};

