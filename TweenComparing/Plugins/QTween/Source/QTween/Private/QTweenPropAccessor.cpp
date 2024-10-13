// Fill out your copyright notice in the Description page of Project Settings.


#include "QTweenPropAccessor.h"


//-----------------------------------------------------------------------
// FTweenPropGetterSetter
//-----------------------------------------------------------------------

FVector FQTweenActorPropAccessor::GetPosition(UObject* obj)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		return actor->GetActorLocation();
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetPosition(UObject* obj, const FVector& pos)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		actor->SetActorLocation(pos);
}

FVector FQTweenActorPropAccessor::GetScale(UObject* obj)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		return actor->GetActorScale3D();
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetScale(UObject* obj, const FVector& scale)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		actor->SetActorScale3D(scale);
}

FVector FQTweenActorPropAccessor::GetRotate(UObject* obj)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		return actor->GetActorRotation().Euler();
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetRotate(UObject* obj, const FVector& rotate)
{
	auto actor = Cast<AActor>(obj);
	if (nullptr != actor)
		actor->SetActorRotation(FRotator(rotate.Y, rotate.Z, rotate.X));
}

float FQTweenActorPropAccessor::GetAlpha(UObject* obj)
{
	return 1;
}

void FQTweenActorPropAccessor::SetAlpha(UObject* obj, float alpha)
{

}

FLinearColor FQTweenActorPropAccessor::GetColor(UObject* obj)
{
	return FLinearColor::White;
}

void FQTweenActorPropAccessor::SetColor(UObject* obj, const FLinearColor& color)
{

}


//-----------------------------------------------------------------------
// FTweenWidgetPropGetterSetter
//-----------------------------------------------------------------------

FVector FQTweenWidgetPropAccessor::GetPosition(UObject* obj)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
		return FVector(widget->GetRenderTransform().Translation, 0);
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetPosition(UObject* obj, const FVector& pos)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
	{
		widget->SetRenderTranslation(FVector2D(pos.X, pos.Y));
	}
}

FVector FQTweenWidgetPropAccessor::GetScale(UObject* obj)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
		return FVector(widget->GetRenderTransform().Scale, 0.f);
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetScale(UObject* obj, const FVector& scale)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
		widget->SetRenderScale(FVector2D(scale.X, scale.Y));
}

FVector FQTweenWidgetPropAccessor::GetRotate(UObject* obj)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
		return FVector(widget->GetRenderTransformAngle(), 0, 0);
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetRotate(UObject* obj, const FVector& rotate)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
		widget->SetRenderTransformAngle(rotate.X);
}

float FQTweenWidgetPropAccessor::GetAlpha(UObject* obj)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
	{
		return widget->GetRenderOpacity();
	}
	return 1;
}

void FQTweenWidgetPropAccessor::SetAlpha(UObject* obj, float alpha)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr != widget)
	{
		widget->SetRenderOpacity(alpha);
	}
}

FLinearColor FQTweenWidgetPropAccessor::GetColor(UObject* obj)
{
	auto widget = Cast<UWidget>(obj);
	if (widget)
	{
		if (widget->IsA(UImage::StaticClass()))
		{
			UImage* img = Cast<UImage>(widget);
			if (img != nullptr)
				return img->GetColorAndOpacity();
		}
		else if (widget->IsA(UButton::StaticClass()))
		{
			UButton* btn = Cast<UButton>(widget);
			if (btn != nullptr)
				return btn->GetColorAndOpacity();
		}
		else if (widget->IsA(UTextBlock::StaticClass()))
		{
			UTextBlock* text = Cast<UTextBlock>(widget);
			if (text != nullptr)
				return text->GetColorAndOpacity().GetSpecifiedColor();
		}
	}

	return FLinearColor::White;
}

void FQTweenWidgetPropAccessor::SetColor(UObject* obj, const FLinearColor& color)
{
	auto widget = Cast<UWidget>(obj);
	if (nullptr == widget)
		return;

	if (widget->IsA(UImage::StaticClass()))
	{
		UImage* img = Cast<UImage>(widget);
		if (nullptr != img)
			img->SetColorAndOpacity(color);
	}
	else if (widget->IsA(UButton::StaticClass()))
	{
		UButton* btn = Cast<UButton>(widget);
		if (nullptr != btn)
			btn->SetColorAndOpacity(color);
	}
	else if (widget->IsA(UTextBlock::StaticClass()))
	{
		UTextBlock* txt = Cast<UTextBlock>(widget);
		if (nullptr != txt)
			txt->SetColorAndOpacity(color);
	}
	else
	{
	}
}


//-----------------------------------------------------------------------
// FTweenPropGetterSetter
//-----------------------------------------------------------------------

FQTweenActorPropAccessor FQTweenPropAccessor::ActorGetterSetter;
FQTweenWidgetPropAccessor FQTweenPropAccessor::WidgetGettterSetter;

FVector FQTweenPropAccessor::GetPosition(UObject* obj)
{
	IQTweenPropAccessor* getter = GetValidGetterSetter(obj);
	if (nullptr != getter)
		return getter->GetPosition(obj);
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetPosition(UObject* obj, const FVector& pos)
{
	IQTweenPropAccessor* setter = GetValidGetterSetter(obj);
	if (nullptr != setter)
		return setter->SetPosition(obj, pos);
}

FVector FQTweenPropAccessor::GetScale(UObject* obj)
{
	IQTweenPropAccessor* getter = GetValidGetterSetter(obj);
	if (nullptr != getter)
		return getter->GetScale(obj);
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetScale(UObject* obj, const FVector& pos)
{
	IQTweenPropAccessor* setter = GetValidGetterSetter(obj);
	if (nullptr != setter)
		return setter->SetScale(obj, pos);
}

FVector FQTweenPropAccessor::GetRotate(UObject* obj)
{
	IQTweenPropAccessor* getter = GetValidGetterSetter(obj);
	if (nullptr != getter)
		return getter->GetRotate(obj);
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetRotate(UObject* obj, const FVector& pos)
{
	IQTweenPropAccessor* setter = GetValidGetterSetter(obj);
	if (nullptr != setter)
		return setter->SetRotate(obj, pos);
}

float FQTweenPropAccessor::GetAlpha(UObject* obj)
{
	IQTweenPropAccessor* getter = GetValidGetterSetter(obj);
	if (nullptr != getter)
		return getter->GetAlpha(obj);
	return 1.f;
}
void FQTweenPropAccessor::SetAlpha(UObject* obj, float alpha)
{
	IQTweenPropAccessor* setter = GetValidGetterSetter(obj);
	if (nullptr != setter)
		return setter->SetAlpha(obj, alpha);
}

FLinearColor FQTweenPropAccessor::GetColor(UObject* obj)
{
	IQTweenPropAccessor* getter = GetValidGetterSetter(obj);
	if (nullptr != getter)
		return getter->GetColor(obj);
	return FLinearColor::White;
}
void FQTweenPropAccessor::SetColor(UObject* obj, const FLinearColor& color)
{
	IQTweenPropAccessor* setter = GetValidGetterSetter(obj);
	if (nullptr != setter)
		return setter->SetColor(obj, color);
}
bool FQTweenPropAccessor::IsValidActor(UObject* obj)
{
	if (nullptr == obj || !obj->IsValidLowLevel())
		return false;

	return obj->IsA(AActor::StaticClass());
}

bool FQTweenPropAccessor::IsValidWidget(UObject* obj)
{
	if (nullptr == obj || !obj->IsValidLowLevel())
		return false;

	return obj->IsA(UWidget::StaticClass());
}

IQTweenPropAccessor* FQTweenPropAccessor::GetValidGetterSetter(UObject* obj)
{
	if (nullptr != obj && obj->IsValidLowLevel())
	{
		if (obj->IsA(AActor::StaticClass()))
			return &ActorGetterSetter;
		else if (obj->IsA(UWidget::StaticClass()))

			return &WidgetGettterSetter;
	}
	return nullptr;
}