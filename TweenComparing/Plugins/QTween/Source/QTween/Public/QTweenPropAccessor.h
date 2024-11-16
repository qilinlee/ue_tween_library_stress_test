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
	virtual ~IQTweenPropAccessor() = default;
	virtual FVector GetPosition(UObject* Obj) = 0;
	virtual void SetPosition(UObject* Obj, const FVector& Pos) = 0;

	virtual FVector GetScale(UObject* Obj) = 0;
	virtual void SetScale(UObject* Obj, const FVector& Pos) = 0;

	virtual FVector GetRotate(UObject* Obj) = 0;
	virtual void SetRotate(UObject* Obj, const FVector& Pos) = 0;

	virtual float GetAlpha(UObject* Obj) = 0;
	virtual void SetAlpha(UObject* Obj, float Alpha) = 0;

	virtual FLinearColor GetColor(UObject* Obj) = 0;
	virtual void SetColor(UObject* Obj, const FLinearColor& Color) = 0;
};

class FQTweenActorPropAccessor : public IQTweenPropAccessor
{
public:
	virtual FVector GetPosition(UObject* Obj) override;
	virtual void SetPosition(UObject* Obj, const FVector& Pos)override;
	virtual FVector GetScale(UObject* Obj) override;
	virtual void SetScale(UObject* Obj, const FVector& Scale) override;
	virtual FVector GetRotate(UObject* Obj) override;
	virtual void SetRotate(UObject* Obj, const FVector& Rotate) override;
	virtual float GetAlpha(UObject* Obj) override;
	virtual void SetAlpha(UObject* Obj, float Alpha) override;
	virtual FLinearColor GetColor(UObject* Obj) override;
	virtual void SetColor(UObject* Obj, const FLinearColor& Color) override;
};

class FQTweenWidgetPropAccessor : public IQTweenPropAccessor
{
public:
	virtual FVector GetPosition(UObject* Obj) override;
	virtual void SetPosition(UObject* Obj, const FVector& Pos) override;
	virtual FVector GetScale(UObject* Obj) override;
	virtual void SetScale(UObject* Obj, const FVector& Scale) override;
	virtual FVector GetRotate(UObject* Obj) override;
	virtual void SetRotate(UObject* Obj, const FVector& Rotate) override;
	virtual float GetAlpha(UObject* Obj) override;
	virtual void SetAlpha(UObject* Obj, float Alpha) override;
	virtual FLinearColor GetColor(UObject* Obj) override;
	virtual void SetColor(UObject* Obj, const FLinearColor& Color) override;
};


class FQTweenPropAccessor
{
public:
	static FVector GetPosition(UObject* Obj);
	static void SetPosition(UObject* Obj, const FVector& Pos);
	static FVector GetScale(UObject* Obj);
	static void SetScale(UObject* Obj, const FVector& Pos);
	static FVector GetRotate(UObject* Obj);
	static void SetRotate(UObject* Obj, const FVector& Pos);
	static float GetAlpha(UObject* Obj);
	static void SetAlpha(UObject* Obj, float alpha);
	static FLinearColor GetColor(UObject* Obj);
	static void SetColor(UObject* Obj, const FLinearColor& color);
private:
	static bool IsValidActor(UObject* Obj);
	static bool IsValidWidget(UObject* Obj);
	static IQTweenPropAccessor* GetValidGetterSetter(UObject* Obj);

	static FQTweenActorPropAccessor ActorGetterSetter;
	static FQTweenWidgetPropAccessor WidgetGetterSetter;
};
