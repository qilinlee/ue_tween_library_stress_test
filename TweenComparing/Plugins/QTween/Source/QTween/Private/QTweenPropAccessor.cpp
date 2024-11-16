// Fill out your copyright notice in the Description page of Project Settings.


#include "QTweenPropAccessor.h"


//-----------------------------------------------------------------------
// FTweenPropGetterSetter
//-----------------------------------------------------------------------

FVector FQTweenActorPropAccessor::GetPosition(UObject* Obj)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
		return Actor->GetActorLocation();
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetPosition(UObject* Obj, const FVector& Pos)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
	{
		Actor->SetActorLocation(Pos);
	}
}

FVector FQTweenActorPropAccessor::GetScale(UObject* Obj)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
	{
		return Actor->GetActorScale3D();
	}
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetScale(UObject* Obj, const FVector& Scale)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
	{
		Actor->SetActorScale3D(Scale);
	}
}

FVector FQTweenActorPropAccessor::GetRotate(UObject* Obj)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
	{
		return Actor->GetActorRotation().Euler();
	}
	return FVector::ZeroVector;
}

void FQTweenActorPropAccessor::SetRotate(UObject* Obj, const FVector& Rotator)
{
	auto Actor = Cast<AActor>(Obj);
	if (nullptr != Actor)
	{
		Actor->SetActorRotation(FRotator(Rotator.Y, Rotator.Z, Rotator.X));
	}
}

float FQTweenActorPropAccessor::GetAlpha(UObject* Obj)
{
	return 1;
}

void FQTweenActorPropAccessor::SetAlpha(UObject* Obj, float Alpha)
{

}

FLinearColor FQTweenActorPropAccessor::GetColor(UObject* Obj)
{
	return FLinearColor::White;
}

void FQTweenActorPropAccessor::SetColor(UObject* Obj, const FLinearColor& Color)
{

}


//-----------------------------------------------------------------------
// FTweenWidgetPropGetterSetter
//-----------------------------------------------------------------------

FVector FQTweenWidgetPropAccessor::GetPosition(UObject* Obj)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		return FVector(Widget->GetRenderTransform().Translation, 0);
	}
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetPosition(UObject* Obj, const FVector& Pos)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		Widget->SetRenderTranslation(FVector2D(Pos.X, Pos.Y));
	}
}

FVector FQTweenWidgetPropAccessor::GetScale(UObject* Obj)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		return FVector(Widget->GetRenderTransform().Scale, 0.f);
	}
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetScale(UObject* Obj, const FVector& Scale)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		Widget->SetRenderScale(FVector2D(Scale.X, Scale.Y));
	}
}

FVector FQTweenWidgetPropAccessor::GetRotate(UObject* Obj)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		return FVector(Widget->GetRenderTransformAngle(), 0, 0);
	}
	return FVector::ZeroVector;
}

void FQTweenWidgetPropAccessor::SetRotate(UObject* Obj, const FVector& Rotator)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		Widget->SetRenderTransformAngle(Rotator.X);
	}
}

float FQTweenWidgetPropAccessor::GetAlpha(UObject* Obj)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		return Widget->GetRenderOpacity();
	}
	return 1;
}

void FQTweenWidgetPropAccessor::SetAlpha(UObject* Obj, float Alpha)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr != Widget)
	{
		Widget->SetRenderOpacity(Alpha);
	}
}

FLinearColor FQTweenWidgetPropAccessor::GetColor(UObject* Obj)
{
	if (auto* Widget = Cast<UWidget>(Obj))
	{
		if (Widget->IsA(UImage::StaticClass()))
		{
			if (UImage* Image = Cast<UImage>(Widget); Image != nullptr)
			{
				return Image->GetColorAndOpacity();
			}
		}
		else if (Widget->IsA(UButton::StaticClass()))
		{
			if (UButton* Button = Cast<UButton>(Widget); Button != nullptr)
			{
				return Button->GetColorAndOpacity();
			}
		}
		else if (Widget->IsA(UTextBlock::StaticClass()))
		{
			if (UTextBlock* Text = Cast<UTextBlock>(Widget); Text != nullptr)
			{
				return Text->GetColorAndOpacity().GetSpecifiedColor();
			}
		}
	}

	return FLinearColor::White;
}

void FQTweenWidgetPropAccessor::SetColor(UObject* Obj, const FLinearColor& Color)
{
	auto Widget = Cast<UWidget>(Obj);
	if (nullptr == Widget)
	{
		return;
	}

	if (Widget->IsA(UImage::StaticClass()))
	{
		if (UImage* Image = Cast<UImage>(Widget); nullptr != Image)
		{
			Image->SetColorAndOpacity(Color);
		}
	}
	else if (Widget->IsA(UButton::StaticClass()))
	{
		if (UButton* Button = Cast<UButton>(Widget); nullptr != Button)
		{
			Button->SetColorAndOpacity(Color);
		}
	}
	else if (Widget->IsA(UTextBlock::StaticClass()))
	{
		if (UTextBlock* Text = Cast<UTextBlock>(Widget); nullptr != Text)
		{
			Text->SetColorAndOpacity(Color);
		}
	}
	else
	{
	}
}


//-----------------------------------------------------------------------
// FTweenPropGetterSetter
//-----------------------------------------------------------------------

FQTweenActorPropAccessor FQTweenPropAccessor::ActorGetterSetter;
FQTweenWidgetPropAccessor FQTweenPropAccessor::WidgetGetterSetter;

FVector FQTweenPropAccessor::GetPosition(UObject* Obj)
{
	if (IQTweenPropAccessor* Getter = GetValidGetterSetter(Obj); nullptr != Getter)
	{
		return Getter->GetPosition(Obj);
	}
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetPosition(UObject* Obj, const FVector& Pos)
{
	SCOPED_NAMED_EVENT(QTween_PropAccessor_SetPosition, FColor::Emerald);
	if (IQTweenPropAccessor* Setter = GetValidGetterSetter(Obj); nullptr != Setter)
	{
		return Setter->SetPosition(Obj, Pos);
	}
}

FVector FQTweenPropAccessor::GetScale(UObject* Obj)
{
	if (IQTweenPropAccessor* Getter = GetValidGetterSetter(Obj); nullptr != Getter)
	{
		return Getter->GetScale(Obj);
	}
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetScale(UObject* Obj, const FVector& Pos)
{
	if (IQTweenPropAccessor* Setter = GetValidGetterSetter(Obj); nullptr != Setter)
	{
		return Setter->SetScale(Obj, Pos);
	}
}

FVector FQTweenPropAccessor::GetRotate(UObject* Obj)
{
	if (IQTweenPropAccessor* Getter = GetValidGetterSetter(Obj); nullptr != Getter)
	{
		return Getter->GetRotate(Obj);
	}
	return FVector::ZeroVector;
}
void FQTweenPropAccessor::SetRotate(UObject* Obj, const FVector& Pos)
{
	if (IQTweenPropAccessor* Setter = GetValidGetterSetter(Obj); nullptr != Setter)
	{
		return Setter->SetRotate(Obj, Pos);
	}
}

float FQTweenPropAccessor::GetAlpha(UObject* Obj)
{
	if (IQTweenPropAccessor* Getter = GetValidGetterSetter(Obj); nullptr != Getter)
	{
		return Getter->GetAlpha(Obj);
	}
	return 1.f;
}
void FQTweenPropAccessor::SetAlpha(UObject* Obj, float Alpha)
{
	if (IQTweenPropAccessor* Setter = GetValidGetterSetter(Obj); nullptr != Setter)
	{
		return Setter->SetAlpha(Obj, Alpha);
	}
}

FLinearColor FQTweenPropAccessor::GetColor(UObject* Obj)
{
	if (IQTweenPropAccessor* Getter = GetValidGetterSetter(Obj); nullptr != Getter)
	{
		return Getter->GetColor(Obj);
	}
	return FLinearColor::White;
}
void FQTweenPropAccessor::SetColor(UObject* Obj, const FLinearColor& Color)
{
	if (IQTweenPropAccessor* Setter = GetValidGetterSetter(Obj); nullptr != Setter)
	{
		return Setter->SetColor(Obj, Color);
	}
}
bool FQTweenPropAccessor::IsValidActor(UObject* Obj)
{
	if (nullptr == Obj || !Obj->IsValidLowLevel())
	{
		return false;
	}

	return Obj->IsA(AActor::StaticClass());
}

bool FQTweenPropAccessor::IsValidWidget(UObject* Obj)
{
	if (nullptr == Obj || !Obj->IsValidLowLevel())
	{
		return false;
	}

	return Obj->IsA(UWidget::StaticClass());
}

IQTweenPropAccessor* FQTweenPropAccessor::GetValidGetterSetter(UObject* Obj)
{
	if (nullptr != Obj && Obj->IsValidLowLevel())
	{
		if (Obj->IsA(AActor::StaticClass()))
		{
			return &ActorGetterSetter;
		}

		if (Obj->IsA(UWidget::StaticClass()))
		{
			return &WidgetGetterSetter;
		}
	}
	return nullptr;
}