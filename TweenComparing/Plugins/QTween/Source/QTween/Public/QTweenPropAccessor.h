// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/Widget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

class IQTweenPropAccessor
{
public:
	virtual FVector GetPosition(UObject* obj) = 0;
	virtual void SetPosition(UObject* obj, const FVector& pos) = 0;

	virtual FVector GetScale(UObject* obj) = 0;
	virtual void SetScale(UObject* obj, const FVector& pos) = 0;

	virtual FVector GetRotate(UObject* obj) = 0;
	virtual void SetRotate(UObject* obj, const FVector& pos) = 0;

	virtual float GetAlpha(UObject* obj) = 0;
	virtual void SetAlpha(UObject* obj, float alpha) = 0;

	virtual FLinearColor GetColor(UObject* obj) = 0;
	virtual void SetColor(UObject* obj, const FLinearColor& color) = 0;
};

class FQTweenActorPropAccessor : public IQTweenPropAccessor
{
public:
	FVector GetPosition(UObject* obj) override;
	void SetPosition(UObject* obj, const FVector& pos)override;
	FVector GetScale(UObject* obj) override;
	void SetScale(UObject* obj, const FVector& scale) override;
	FVector GetRotate(UObject* obj) override;
	void SetRotate(UObject* obj, const FVector& rotate) override;
	float GetAlpha(UObject* obj) override;
	void SetAlpha(UObject* obj, float alpha) override;
	FLinearColor GetColor(UObject* obj) override;
	void SetColor(UObject* obj, const FLinearColor& color) override;
};

class FQTweenWidgetPropAccessor : public IQTweenPropAccessor
{
public:
	FVector GetPosition(UObject* obj) override;
	void SetPosition(UObject* obj, const FVector& pos) override;
	FVector GetScale(UObject* obj) override;
	void SetScale(UObject* obj, const FVector& scale);
	FVector GetRotate(UObject* obj);
	void SetRotate(UObject* obj, const FVector& rotate);
	float GetAlpha(UObject* obj);
	void SetAlpha(UObject* obj, float alpha);
	FLinearColor GetColor(UObject* obj);
	void SetColor(UObject* obj, const FLinearColor& color);
};


class FQTweenPropAccessor
{
public:
	static FVector GetPosition(UObject* obj);
	static void SetPosition(UObject* obj, const FVector& pos);
	static FVector GetScale(UObject* obj);
	static void SetScale(UObject* obj, const FVector& pos);
	static FVector GetRotate(UObject* obj);
	static void SetRotate(UObject* obj, const FVector& pos);
	static float GetAlpha(UObject* obj);
	static void SetAlpha(UObject* obj, float alpha);
	static FLinearColor GetColor(UObject* obj);
	static void SetColor(UObject* obj, const FLinearColor& color);
private:
	static bool IsValidActor(UObject* obj);
	static bool IsValidWidget(UObject* obj);
	static IQTweenPropAccessor* GetValidGetterSetter(UObject* obj);

	static FQTweenActorPropAccessor ActorGetterSetter;
	static FQTweenWidgetPropAccessor WidgetGettterSetter;
};
