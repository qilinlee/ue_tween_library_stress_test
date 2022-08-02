// Copyright Jared Therriault and Bob Berkbile 2014

#include "iTweenEvent.h"
#include "GameFramework/WorldSettings.h"
#include "TimerManager.h"
#include "iTween.h"

AiTweenEvent::AiTweenEvent()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AiTweenEvent::BeginPlay()
{
	AActor::BeginPlay();
	aux = (AiTAux*)GetOwner();

	UiTween::Print("iTweenEvent object created");
}

void AiTweenEvent::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	AActor::EndPlay(endPlayReason);

	if (aux)
	{
		UiTween::Print("iTweenEvent object destroyed.");

		aux->currentTweens.Remove(this);
	}

	if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint || eventType == EEventType::ItweenEventType::compMoveToSplinePoint || eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
	{
		if (destroySplineObject)
		{
			if (splineComponent)
			{
				splineComponent->GetOwner()->Destroy();
			}
			if (OldSplineComponent)
			{
				OldSplineComponent->GetOwner()->Destroy();
			}
		}
		else if (DestroyNewSplineObject)
		{
			if (splineComponent)
			{
				splineComponent->GetOwner()->Destroy();
			}
		}
	}
}

void AiTweenEvent::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	deltaSeconds = DeltaSeconds;
	if (ignoreTimeDilation)
	{
		deltaSeconds *= (1 / GetWorldSettings()->TimeDilation);
	}
	ExecuteTween();
}

void AiTweenEvent::UpdateTween()
{
	deltaSeconds = timerInterval;
	ExecuteTween();
}

void AiTweenEvent::SetTimerInterval(float interval /*= 0.f*/)
{
	timerInterval = FMath::Abs(interval);

	if (timerInterval > 0.0001f)
	{
		this->PrimaryActorTick.SetTickFunctionEnable(false);

		UpdateTween();
		FTimerHandle tweenTimer;

		GetWorld()->GetTimerManager().SetTimer(tweenTimer, this, &AiTweenEvent::UpdateTween, timerInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

		this->PrimaryActorTick.SetTickFunctionEnable(true);
	}
}


void AiTweenEvent::RestartTween()
{
	this->alpha = 0.f;
}

void AiTweenEvent::ParseParameters(FString params)
{
	//Create an array and a map for parsing
	TArray<FString> arr;
	TMap<FString, FString> map;
	//Split into new array entry after each semicolon
	params.ParseIntoArray(arr, TEXT(";"), true);

	//Split each array entry into map at the equal (assignment) sign
	for (int i = 0; i < arr.Num(); i++)
	{
		arr[i].TrimStartAndEndInline();

		FString left;
		FString right;
		arr[i].Split("=", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		left = left.ToLower();
		left.ReplaceInline(TEXT(" "), TEXT(""));
		right.TrimStartAndEndInline();
		map.Add(left, right);
	}

	if (map.Contains("name"))
	{
		tweenName = FName(*map["name"]);
		UiTween::Print("tweenName: " + tweenName.ToString());
	}

	if (map.Contains("floatfrom"))
	{
		map.Add("ffrom", map["floatfrom"]);
		map.Remove("floatfrom");
	}

	if (map.Contains("ffrom"))
	{
		map["ffrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ffrom"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ffrom"].TrimStartAndEndInline();

		if (map["ffrom"].Contains("current") || map["ffrom"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo)
				{
					floatFrom = widgetTweening->RenderTransform.Angle;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
					floatFrom = 0.f;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
				floatFrom = 0.f;
			}
		}
		else
		{
			floatFrom = FCString::Atof(*map["ffrom"]);
		}

		if (map["ffrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["ffrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			floatFrom *= FCString::Atof(*right);
		}

		UiTween::Print("floatFrom: " + FString::SanitizeFloat(floatFrom));
	}

	if (map.Contains("floatto"))
	{
		map.Add("fto", map["floatto"]);
		map.Remove("floatto");
	}

	if (map.Contains("fto"))
	{
		map["fto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["fto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["fto"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["fto"].TrimStartAndEndInline();

		if (map["fto"].Contains("current") || map["fto"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo)
				{
					floatTo = widgetTweening->RenderTransform.Angle;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
					floatTo = 0.f;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Rotate From/To operation. The 'current' and 'this' keywords only apply to that operation. Defaulting floatFrom to 0.", "error");
				floatTo = 0.f;
			}
		}
		else
		{
			floatTo = FCString::Atof(*map["fto"]);
		}

		if (map["fto"].Contains("*"))
		{
			FString left;
			FString right;
			map["fto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			floatTo *= FCString::Atof(*right);
		}

		UiTween::Print("floatTo: " + FString::SanitizeFloat(floatTo));
	}

	if (map.Contains("coordinatespace"))
	{
		map.Add("cspace", map["coordinatespace"]);
		map.Remove(map["coordinatespace"]);
	}

	if (map.Contains("cspace"))
	{
		map["cspace"].TrimStartAndEndInline();

		if (map["cspace"].Contains("world"))
		{
			coordinateSpace = ECoordinateSpace::ItweenCoordinateSpace::world;
		}
		else if (map["cspace"].Contains("parent"))
		{
			coordinateSpace = ECoordinateSpace::ItweenCoordinateSpace::parent;
		}
		else if (map["cspace"].Contains("self"))
		{
			coordinateSpace = ECoordinateSpace::ItweenCoordinateSpace::self;
		}

		UiTween::Print("coordinateSpace: " + map["cspace"]);
	}

	if (map.Contains("vectorfrom"))
	{
		map.Add("vfrom", map["vectorfrom"]);
		map.Remove("vectorfrom");
	}

	if (map.Contains("vfrom"))
	{
		map["vfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("FVector"), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["vfrom"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["vfrom"].ReplaceInline(TEXT("Z="), TEXT(","));

		map["vfrom"].TrimStartAndEndInline();

		//Implement something that will allow a parameter to be the object's current transform
		if (map["vfrom"].Contains("zero"))
		{
			vectorFrom = FVector::ZeroVector;
		}
		else if (map["vfrom"].Contains("one"))
		{
			vectorFrom = FVector(1, 1, 1);
		}
		else if (map["vfrom"].Contains("current") || map["vfrom"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				vectorFrom = actorTweening->GetActorLocation();
			}
			else if (componentTweening != nullptr)
			{
				vectorFrom = componentTweening->GetComponentLocation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vectorFrom to (0,0,0).", "error");
				vectorFrom = FVector::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["vfrom"].ParseIntoArray(parse, TEXT(","), true);

			vectorFrom = FVector(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["vfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["vfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vectorFrom *= FCString::Atof(*right);
		}

		UiTween::Print("vectorFrom: " + vectorFrom.ToString());
	}

	if (map.Contains("vectorto"))
	{
		map.Add("vto", map["vectorto"]);
		map.Remove("vectorto");
	}

	if (map.Contains("vto"))
	{
		map["vto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["vto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("FVector"), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["vto"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["vto"].ReplaceInline(TEXT("Z="), TEXT(","));

		map["vto"].TrimStartAndEndInline();

		if (map["vto"].Contains("zero"))
		{
			vectorTo = FVector::ZeroVector;
		}
		else if (map["vto"].Contains("one"))
		{
			vectorTo = FVector(1, 1, 1);
		}
		else if (map["vto"].Contains("current") || map["vto"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				vectorTo = actorTweening->GetActorLocation();
			}
			else if (componentTweening != nullptr)
			{
				vectorTo = componentTweening->GetComponentLocation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vectorTo to (0,0,0).", "error");
				vectorTo = FVector::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["vto"].ParseIntoArray(parse, TEXT(","), true);

			vectorTo = FVector(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["vto"].Contains("*"))
		{
			FString left;
			FString right;
			map["vto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vectorTo *= FCString::Atof(*right);
		}

		UiTween::Print("vectorTo: " + vectorTo.ToString());
	}

	if (map.Contains("vector2dfrom"))
	{
		map.Add("v2from", map["vector2dfrom"]);
		map.Remove("vector2dfrom");
	}

	if (map.Contains("v2from"))
	{
		map["v2from"].ReplaceInline(TEXT("("), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("["), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("FVector2D"), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["v2from"].ReplaceInline(TEXT("Y="), TEXT(","));

		map["v2from"].TrimStartAndEndInline();

		if (map["v2from"].Contains("zero"))
		{
			vector2DFrom = FVector2D::ZeroVector;
		}
		else if (map["v2from"].Contains("one"))
		{
			vector2DFrom = FVector2D(1, 1);
		}
		else if (map["v2from"].Contains("current") || map["v2from"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Translation;
				}
				else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Scale;
				}
				else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo)
				{
					vector2DFrom = widgetTweening->RenderTransform.Shear;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a Slate or UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DFrom to (0,0).", "error");
				vector2DFrom = FVector2D::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["v2from"].ParseIntoArray(parse, TEXT(","), true);

			vector2DFrom = FVector2D(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]));
		}

		if (map["v2from"].Contains("*"))
		{
			FString left;
			FString right;
			map["v2from"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vector2DFrom *= FCString::Atof(*right);
		}

		UiTween::Print("vector2DFrom: " + vector2DFrom.ToString());
	}

	if (map.Contains("vector2dto"))
	{
		map.Add("v2to", map["vector2dto"]);
		map.Remove("vector2dto");
	}

	if (map.Contains("v2to"))
	{
		map["v2to"].ReplaceInline(TEXT("("), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("["), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("FVector2D"), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("X="), TEXT(" "));
		map["v2to"].ReplaceInline(TEXT("Y="), TEXT(","));

		map["v2to"].TrimStartAndEndInline();

		if (map["v2to"].Contains("zero"))
		{
			vector2DTo = FVector2D::ZeroVector;
		}
		else if (map["v2to"].Contains("one"))
		{
			vector2DTo = FVector2D(1, 1);
		}
		else if (map["v2to"].Contains("current") || map["v2to"].Contains("this"))
		{
			if (widgetTweening != nullptr)
			{
				if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Translation;
				}
				else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Scale;
				}
				else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo)
				{
					vector2DTo = widgetTweening->RenderTransform.Shear;
				}
				else
				{
					UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DTo to (0,0).", "error");
					vector2DFrom = FVector2D::ZeroVector;
				}
			}
			else
			{
				UiTween::Print("You aren't performing a UMG RT Move, Scale, or Shear From/To operation. The 'current' and 'this' keywords only apply to those operations. Defaulting vector2DTo to (0,0).", "error");
				vector2DFrom = FVector2D::ZeroVector;
			}
		}
		else
		{
			TArray<FString> parse;

			map["v2to"].ParseIntoArray(parse, TEXT(","), true);

			vector2DTo = FVector2D(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]));
		}

		if (map["v2to"].Contains("*"))
		{
			FString left;
			FString right;
			map["v2to"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			vector2DTo *= FCString::Atof(*right);
		}

		UiTween::Print("vector2DTo: " + vector2DTo.ToString());
	}

	if (map.Contains("rotatorfrom"))
	{
		map.Add("rfrom", map["rotatorfrom"]);
		map.Remove("rotatorfrom");
	}

	if (map.Contains("rfrom"))
	{
		map["rfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("FRotator"), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("P="), TEXT(" "));
		map["rfrom"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["rfrom"].ReplaceInline(TEXT("R="), TEXT(","));

		map["rfrom"].TrimStartAndEndInline();

		if (map["rfrom"].Contains("zero"))
		{
			rotatorFrom = FRotator::ZeroRotator;
		}
		else if (map["rfrom"].Contains("one"))
		{
			rotatorFrom = FRotator(1, 1, 1);
		}
		else if (map["rfrom"].Contains("current") || map["rfrom"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				rotatorFrom = actorTweening->GetActorRotation();
			}
			else if (componentTweening != nullptr)
			{
				rotatorFrom = componentTweening->GetComponentRotation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting rotatorFrom to (0,0,0).", "error");
				rotatorFrom = FRotator::ZeroRotator;
			}
		}
		else
		{
			TArray<FString> parse;

			map["rfrom"].ParseIntoArray(parse, TEXT(","), true);

			rotatorFrom = FRotator(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["rfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["rfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			rotatorFrom *= FCString::Atof(*right);
		}

		UiTween::Print("rotatorFrom: " + rotatorFrom.ToString());
	}

	if (map.Contains("rotatorto"))
	{
		map.Add("rto", map["rotatorto"]);
		map.Remove("rotatorto");
	}

	if (map.Contains("rto"))
	{
		map["rto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["rto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("FRotator"), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("P="), TEXT(" "));
		map["rto"].ReplaceInline(TEXT("Y="), TEXT(","));
		map["rto"].ReplaceInline(TEXT("R="), TEXT(","));

		map["rto"].TrimStartAndEndInline();

		if (map["rto"].Contains("zero"))
		{
			rotatorTo = FRotator::ZeroRotator;
		}
		else if (map["rto"].Contains("one"))
		{
			rotatorTo = FRotator(1, 1, 1);
		}
		else if (map["rto"].Contains("current") || map["rto"].Contains("this"))
		{
			if (actorTweening != nullptr)
			{
				rotatorTo = actorTweening->GetActorRotation();
			}
			else if (componentTweening != nullptr)
			{
				rotatorTo = componentTweening->GetComponentRotation();
			}
			else
			{
				UiTween::Print("You aren't performing an Actor- or SceneComponent-based operation. The 'current' and 'this' keywords only apply to those operations. Defaulting rotatorTo to (0,0,0).", "error");
				rotatorTo = FRotator::ZeroRotator;
			}
		}
		else
		{
			TArray<FString> parse;

			map["rto"].ParseIntoArray(parse, TEXT(","), true);

			rotatorTo = FRotator(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}

		if (map["rto"].Contains("*"))
		{
			FString left;
			FString right;
			map["rto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			rotatorTo *= FCString::Atof(*right);
		}

		UiTween::Print("rotatorTo: " + rotatorTo.ToString());
	}

	if (map.Contains("linearcolorfrom"))
	{
		map.Add("lcfrom", map["linearcolorfrom"]);
		map.Remove("linearcolorfrom");
	}

	if (map.Contains("linearcolourfrom"))
	{
		map.Add("lcfrom", map["linearcolourfrom"]);
		map.Remove("linearcolourfrom");
	}

	if (map.Contains("colorfrom"))
	{
		map.Add("lcfrom", map["colorfrom"]);
		map.Remove("colorfrom");
	}

	if (map.Contains("colourfrom"))
	{
		map.Add("lcfrom", map["colourfrom"]);
		map.Remove("colourfrom");
	}

	if (map.Contains("cfrom"))
	{
		map.Add("lcfrom", map["cfrom"]);
		map.Remove("cfrom");
	}

	if (map.Contains("lcfrom"))
	{
		map["lcfrom"].ReplaceInline(TEXT("("), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("["), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("FLinearColor"), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("R="), TEXT(" "));
		map["lcfrom"].ReplaceInline(TEXT("G="), TEXT(","));
		map["lcfrom"].ReplaceInline(TEXT("B="), TEXT(","));
		map["lcfrom"].ReplaceInline(TEXT("A="), TEXT(","));

		map["lcfrom"].TrimStartAndEndInline();

		TArray<FString> parse;

		map["lcfrom"].ParseIntoArray(parse, TEXT(","), true);

		if (parse.Num() == 1)
		{
			map["lcfrom"].ToLowerInline();

			if (map["lcfrom"].Contains("black"))
			{
				linearColorFrom = FLinearColor(FColor::Black);
			}
			else if (map["lcfrom"].Contains("blue"))
			{
				linearColorFrom = FLinearColor(FColor::Blue);
			}
			else if (map["lcfrom"].Contains("cyan"))
			{
				linearColorFrom = FLinearColor(FColor::Cyan);
			}
			else if (map["lcfrom"].Contains("green"))
			{
				linearColorFrom = FLinearColor(FColor::Green);
			}
			else if (map["lcfrom"].Contains("magenta"))
			{
				linearColorFrom = FLinearColor(FColor::Magenta);
			}
			else if (map["lcfrom"].Contains("red"))
			{
				linearColorFrom = FLinearColor(FColor::Red);
			}
			else if (map["lcfrom"].Contains("white"))
			{
				linearColorFrom = FLinearColor(FColor::White);
			}
			else if (map["lcfrom"].Contains("yellow"))
			{
				linearColorFrom = FLinearColor(FColor::Yellow);
			}
		}
		else if (parse.Num() == 2)
		{
			UiTween::Print("Linear color parameters take 1, 3, or 4 arguments, not 2.", "error");
		}
		else if (parse.Num() == 3)
		{
			linearColorFrom = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}
		else if (parse.Num() == 4)
		{
			linearColorFrom = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]), FCString::Atof(*parse[3]));
		}

		if (map["lcfrom"].Contains("*"))
		{
			FString left;
			FString right;
			map["lcfrom"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			linearColorFrom *= FCString::Atof(*right);
			linearColorFrom.A /= FCString::Atof(*right);
		}

		UiTween::Print("linearColorFrom: " + linearColorFrom.ToString());
	}

	if (map.Contains("linearcolorto"))
	{
		map.Add("lcto", map["linearcolorto"]);
		map.Remove("linearcolorto");
	}

	if (map.Contains("linearcolourto"))
	{
		map.Add("lcto", map["linearcolourto"]);
		map.Remove("linearcolourto");
	}

	if (map.Contains("colorto"))
	{
		map.Add("lcto", map["colorto"]);
		map.Remove("colorto");
	}

	if (map.Contains("colourto"))
	{
		map.Add("lcto", map["colourto"]);
		map.Remove("colourto");
	}

	if (map.Contains("cto"))
	{
		map.Add("lcto", map["cto"]);
		map.Remove("cto");
	}

	if (map.Contains("lcto"))
	{
		map["lcto"].ReplaceInline(TEXT("("), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("["), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("]"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("FLinearColor"), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("R="), TEXT(" "));
		map["lcto"].ReplaceInline(TEXT("G="), TEXT(","));
		map["lcto"].ReplaceInline(TEXT("B="), TEXT(","));
		map["lcto"].ReplaceInline(TEXT("A="), TEXT(","));

		map["lcto"].TrimStartAndEndInline();

		TArray<FString> parse;

		map["lcto"].ParseIntoArray(parse, TEXT(","), true);

		if (parse.Num() == 1)
		{
			map["lcto"].ToLowerInline();

			if (map["lcto"].Contains("black"))
			{
				linearColorTo = FLinearColor(FColor::Black);
			}
			else if (map["lcto"].Contains("blue"))
			{
				linearColorTo = FLinearColor(FColor::Blue);
			}
			else if (map["lcto"].Contains("cyan"))
			{
				linearColorTo = FLinearColor(FColor::Cyan);
			}
			else if (map["lcto"].Contains("green"))
			{
				linearColorTo = FLinearColor(FColor::Green);
			}
			else if (map["lcto"].Contains("magenta"))
			{
				linearColorTo = FLinearColor(FColor::Magenta);
			}
			else if (map["lcto"].Contains("red"))
			{
				linearColorTo = FLinearColor(FColor::Red);
			}
			else if (map["lcto"].Contains("white"))
			{
				linearColorTo = FLinearColor(FColor::White);
			}
			else if (map["lcto"].Contains("yellow"))
			{
				linearColorTo = FLinearColor(FColor::Yellow);
			}
		}
		else if (parse.Num() == 2)
		{
			UiTween::Print("Color parameters take 1, 3, or 4 arguments, not 2.", "error");
		}
		else if (parse.Num() == 3)
		{
			linearColorTo = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]));
		}
		else if (parse.Num() == 4)
		{
			linearColorTo = FLinearColor(FCString::Atof(*parse[0]), FCString::Atof(*parse[1]), FCString::Atof(*parse[2]), FCString::Atof(*parse[3]));
		}

		if (map["lcto"].Contains("*"))
		{
			FString left;
			FString right;
			map["lcto"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			linearColorTo *= FCString::Atof(*right);
			linearColorTo.A /= FCString::Atof(*right);
		}

		UiTween::Print("linearColorTo: " + linearColorTo.ToString());
	}

	if (map.Contains("delaytype"))
	{
		map.Add("dtype", map["delaytype"]);
		map.Remove("delaytype");
	}

	if (map.Contains("dtype"))
	{
		if (map["dtype"] == "first")
		{
			delayType = EDelayType::ItweenDelayType::first;
		}
		else if (map["dtype"] == "firstloop")
		{
			delayType = EDelayType::ItweenDelayType::firstLoop;
		}
		else if (map["dtype"] == "firstloopfull")
		{
			delayType = EDelayType::ItweenDelayType::firstLoopFull;
		}
		else if (map["dtype"] == "loop")
		{
			delayType = EDelayType::ItweenDelayType::loop;
		}
		else if (map["dtype"] == "loopfull")
		{
			delayType = EDelayType::ItweenDelayType::loopFull;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the delayType argument are: first, firstloop, firstloopfull, loop, and loopfull.", "error");
		}

		UiTween::Print("delaytype: " + map["dtype"]);
	}

	if (map.Contains("delay"))
	{
		map["delay"].ReplaceInline(TEXT("("), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("["), TEXT(" "));
		map["delay"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["delay"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["delay"].TrimStartAndEndInline();

		delay = FCString::Atof(*map["delay"]);

		if (map["delay"].Contains("*"))
		{
			FString left;
			FString right;
			map["delay"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			delay *= FCString::Atof(*right);
		}

		UiTween::Print("delay: " + FString::SanitizeFloat(delay));
	}

	if (map.Contains("ticktype"))
	{
		map.Add("ttype", map["ticktype"]);
		map.Remove("ticktype");
	}

	if (map.Contains("ttype"))
	{
		if (map["ttype"] == "seconds")
		{
			tickType = ETickType::ItweenTickType::seconds;
		}
		else if (map["ttype"] == "speed")
		{
			tickType = ETickType::ItweenTickType::speed;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the tickType argument are: seconds and speed.", "error");
		}

		UiTween::Print("tickType: " + map["ttype"]);
	}

	if (map.Contains("ticktypevalue"))
	{
		map.Add("ttv", map["ticktypevalue"]);
		map.Remove("ticktypevalue");
	}

	if (map.Contains("ttv"))
	{
		map["ttv"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ttv"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ttv"].TrimStartAndEndInline();

		tickTypeValue = FCString::Atof(*map["ttv"]);

		if (map["ttv"].Contains("*"))
		{
			FString left;
			FString right;
			map["ttv"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("tickTypeValue: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("speed"))
	{
		tickType = ETickType::ItweenTickType::speed;

		map["speed"].ReplaceInline(TEXT("("), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("["), TEXT(" "));
		map["speed"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["speed"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["speed"].TrimStartAndEndInline();

		tickTypeValue = FCString::Atof(*map["speed"]);

		if (map["speed"].Contains("*"))
		{
			FString left;
			FString right;
			map["speed"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("speed: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("timeinseconds"))
	{
		tickType = ETickType::ItweenTickType::seconds;

		map["timeinseconds"].ReplaceInline(TEXT("("), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("["), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["timeinseconds"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["timeinseconds"].TrimStartAndEndInline();

		tickTypeValue = FCString::Atof(*map["timeinseconds"]);

		if (map["timeinseconds"].Contains("*"))
		{
			FString left;
			FString right;
			map["timeinseconds"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("timeInSeconds: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("time"))
	{
		tickType = ETickType::ItweenTickType::seconds;

		map["time"].ReplaceInline(TEXT("("), TEXT(" "));
		map["time"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("["), TEXT(" "));
		map["time"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["time"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["time"].TrimStartAndEndInline();

		tickTypeValue = FCString::Atof(*map["time"]);

		if (map["time"].Contains("*"))
		{
			FString left;
			FString right;
			map["time"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			tickTypeValue *= FCString::Atof(*right);
		}

		UiTween::Print("timeInSeconds: " + FString::SanitizeFloat(tickTypeValue));
	}

	if (map.Contains("punchamplitude"))
	{
		map.Add("pamp", map["punchamplitude"]);
		map.Remove("punchamplitude");
	}

	if (map.Contains("punch"))
	{
		map.Add("pamp", map["punch"]);
		map.Remove("punch");
	}

	if (map.Contains("pamp"))
	{
		map["pamp"].ReplaceInline(TEXT("("), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("["), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["pamp"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["pamp"].TrimStartAndEndInline();

		punchAmplitude = FCString::Atof(*map["pAmp"]);

		if (map["pamp"].Contains("*"))
		{
			FString left;
			FString right;
			map["pamp"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			punchAmplitude *= FCString::Atof(*right);
		}

		UiTween::Print("punchAmplitude: " + FString::SanitizeFloat(punchAmplitude));
	}

	if (map.Contains("generatedpointdistance"))
	{
		map.Add("gpd", map["generatedpointdistance"]);
		map.Remove("generatedpointdistance");
	}

	if (map.Contains("gpd"))
	{
		map["gpd"].ReplaceInline(TEXT("("), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("["), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["gpd"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["gpd"].TrimStartAndEndInline();

		generatedPointDistance = FCString::Atof(*map["pAmp"]);

		if (map["gpd"].Contains("*"))
		{
			FString left;
			FString right;
			map["gpd"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			generatedPointDistance *= FCString::Atof(*right);
		}

		UiTween::Print("generatedPointDistance: " + FString::SanitizeFloat(generatedPointDistance));
	}

	if (map.Contains("sweeptransform"))
	{
		map.Add("sweep", map["sweeptransform"]);
		map.Remove("sweeptransform");
	}

	if (map.Contains("sweep"))
	{
		if (map["sweep"].Contains("true"))
		{
			sweepTransform = true;
		}
		else
		{
			sweepTransform = false;
		}

		UiTween::Print("sweeptransform: " + map["sweep"]);
	}

	if (map.Contains("vectorconstraints"))
	{
		map.Add("vconst", map["vectorconstraints"]);
		map.Remove("vectorconstraints");
	}

	if (map.Contains("vconst"))
	{
		if (map["vconst"].Contains("no"))
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::none;
		}
		else if (map["vconst"] == "x")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::xOnly;
		}
		else if (map["vconst"] == "y")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::yOnly;
		}
		else if (map["vconst"] == "z")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::zOnly;
		}
		else if (map["vconst"] == "xy")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::xyOnly;
		}
		else if (map["vconst"] == "xz")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::xzOnly;
		}
		else if (map["vconst"] == "yz")
		{
			vectorConstraints = EVectorConstraints::ItweenVectorConstraints::yzOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the vectorConstraints argument are: no, none, x, y, z, xy, xz, and yz.", "error");
		}

		UiTween::Print("vectorConstraints: " + map["vconst"]);
	}

	if (map.Contains("vector2dconstraints"))
	{
		map.Add("v2const", map["vector2dconstraints"]);
		map.Remove("vector2dconstraints");
	}

	if (map.Contains("v2const"))
	{
		if (map["v2const"].Contains("no"))
		{
			vector2DConstraints = EVector2DConstraints::ItweenVector2DConstraints::none;
		}
		else if (map["v2const"] == "x")
		{
			vector2DConstraints = EVector2DConstraints::ItweenVector2DConstraints::xOnly;
		}
		else if (map["v2const"] == "y")
		{
			vector2DConstraints = EVector2DConstraints::ItweenVector2DConstraints::yOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the vector2DConstraints argument are: no, none, x, and y.", "error");
		}

		UiTween::Print("vector2DConstraints: " + map["v2const"]);
	}

	if (map.Contains("rotatorconstraints"))
	{
		map.Add("rconst", map["rotatorconstraints"]);
		map.Remove("rotatorconstraints");
	}

	if (map.Contains("rconst"))
	{
		if (map["rconst"].Contains("no"))
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::none;
		}
		else if (map["rconst"] == "pitch")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::pitchOnly;
		}
		else if (map["rconst"] == "yaw")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::yawOnly;
		}
		else if (map["rconst"] == "roll")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::rollOnly;
		}
		else if (map["rconst"] == "pitchyaw")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::pitchYawOnly;
		}
		else if (map["rconst"] == "pitchroll")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::pitchRollOnly;
		}
		else if (map["rconst"] == "yawroll")
		{
			rotatorConstraints = ERotatorConstraints::ItweenRotatorConstraints::yawRollOnly;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the rotatorConstraints argument are: no, none, pitch, yaw, roll, pitchyaw, pitchroll, and yawroll.", "error");
		}

		UiTween::Print("rotatorConstraints: " + map["rconst"]);
	}

	if (map.Contains("looptype"))
	{
		map.Add("loop", map["looptype"]);
		map.Remove("looptype");
	}

	if (map.Contains("loop"))
	{
		if (map["loop"] == "once")
		{
			loopType = ELoopType::ItweenLoopType::once;
		}
		else if (map["loop"] == "pingpong" || map["loop"] == "backandforth")
		{
			loopType = ELoopType::ItweenLoopType::pingPong;
		}
		else if (map["loop"] == "rewind")
		{
			loopType = ELoopType::ItweenLoopType::rewind;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the loopType argument are: once, pingpong or backandforth, and rewind.", "error");
		}

		UiTween::Print("loopType: " + map["loop"]);
	}

	if (map.Contains("looktype"))
	{
		map.Add("look", map["looktype"]);
		map.Remove("looktype");
	}

	if (map.Contains("orientation"))
	{
		map.Add("look", map["orientation"]);
		map.Remove("orientation");
	}

	if (map.Contains("look"))
	{
		if (map["look"].Contains("no"))
		{
			orientation = ELookType::ItweenLookType::noOrientationChange;
		}
		else if (map["look"].Contains("path"))
		{
			orientation = ELookType::ItweenLookType::orientToPath;
		}
		else if (map["look"].Contains("target"))
		{
			orientation = ELookType::ItweenLookType::orientToTarget;
		}
		else
		{
			UiTween::Print("Acceptable parameters for the orientation argument are: no, path, and target.", "error");
		}

		UiTween::Print("lookType/orientation: " + map["look"]);
	}

	if (map.Contains("orientationspeed"))
	{
		map.Add("ospeed", map["orientationspeed"]);
		map.Remove("orientationspeed");
	}

	if (map.Contains("lookspeed"))
	{
		map.Add("ospeed", map["lookspeed"]);
		map.Remove("lookspeed");
	}

	if (map.Contains("ospeed"))
	{
		map["ospeed"].ReplaceInline(TEXT("("), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("["), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["ospeed"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["ospeed"].TrimStartAndEndInline();

		orientationSpeed = FCString::Atof(*map["ospeed"]);

		if (map["ospeed"].Contains("*"))
		{
			FString left;
			FString right;
			map["ospeed"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			orientationSpeed *= FCString::Atof(*right);
		}

		UiTween::Print("orientationSpeed: " + FString::SanitizeFloat(orientationSpeed));
	}

	if (map.Contains("tickwhenpaused"))
	{
		map.Add("twp", map["tickwhenpaused"]);
		map.Remove("tickwhenpaused");
	}

	if (map.Contains("twp"))
	{
		if (map["twp"].Contains("true"))
		{
			tickWhenPaused = true;
		}
		else
		{
			tickWhenPaused = false;
		}

		UiTween::Print("tickWhenPaused: " + map["twp"]);
	}

	if (map.Contains("ignoretimedilation"))
	{
		map.Add("itd", map["ignoretimedilation"]);
		map.Remove("ignoretimedilation");
	}

	if (map.Contains("itd"))
	{
		if (map["itd"].Contains("true"))
		{
			ignoreTimeDilation = true;
		}
		else
		{
			ignoreTimeDilation = false;
		}

		UiTween::Print("ignoreTimeDilation: " + map["itd"]);
	}

	if (map.Contains("interp"))
	{
		if (map["interp"].Contains("true"))
		{
			interpolateToSpline = true;
		}
		else
		{
			interpolateToSpline = false;
		}

		UiTween::Print("interpolateToSpline: " + map["interp"]);
	}

	if (map.Contains("switchpathorientationdirection"))
	{
		map.Add("spod", map["switchpathorientationdirection"]);
		map.Remove("switchpathorientationdirection");
	}

	if (map.Contains("spod"))
	{
		if (map["spod"].Contains("true"))
		{
			switchPathOrientationDirection = true;
		}
		else
		{
			switchPathOrientationDirection = false;
		}

		UiTween::Print("switchPathOrientationDirection: " + map["spod"]);
	}

	if (map.Contains("destroysplineobject"))
	{
		map.Add("dso", map["destroysplineobject"]);
		map.Remove("destroysplineobject");
	}

	if (map.Contains("destroyspline"))
	{
		map.Add("dso", map["destroyspline"]);
		map.Remove("destroyspline");
	}

	if (map.Contains("dso"))
	{
		if (map["dso"].Contains("true"))
		{
			destroySplineObject = true;
		}
		else
		{
			destroySplineObject = false;
		}

		UiTween::Print("destroySplineObject: " + map["dso"]);
	}

	if (map.Contains("shortestpath"))
	{
		map.Add("short", map["shortestpath"]);
		map.Remove(map["shortestpath"]);
	}

	if (map.Contains("short"))
	{
		if (map["short"].Contains("true"))
		{
			shortestPath = true;
		}
		else
		{
			shortestPath = false;
		}

		UiTween::Print("shortestPath: " + map["short"]);
	}

	if (map.Contains("easetype"))
	{
		map.Add("ease", map["easetype"]);
		map.Remove(map["easetype"]);
	}

	if (map.Contains("ease"))
	{
		map["ease"].TrimStartAndEndInline();

		if (map["ease"].Contains("linear"))
		{
			easeType = EEaseType::ItweenEaseType::linear;
		}
		else if (map["ease"].Contains("punch"))
		{
			easeType = EEaseType::ItweenEaseType::punch;
		}
		else if (map["ease"].Contains("spring"))
		{
			easeType = EEaseType::ItweenEaseType::spring;
		}
		else if (map["ease"].Contains("curve"))
		{
			easeType = EEaseType::ItweenEaseType::customCurve;
		}
		else if (map["ease"].Contains("easeInAndOutBack") || map["ease"].Contains("ioback"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutBack;
		}
		else if (map["ease"].Contains("easeInAndOutBounce") || map["ease"].Contains("iobounce"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutBounce;
		}
		else if (map["ease"].Contains("easeInAndOutCircular") || map["ease"].Contains("iocirc"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutCircular;
		}
		else if (map["ease"].Contains("easeInAndOutCubic") || map["ease"].Contains("iocubic"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutCubic;
		}
		else if (map["ease"].Contains("easeInAndOutElastic") || map["ease"].Contains("ioelas"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutElastic;
		}
		else if (map["ease"].Contains("easeInAndOutExponential") || map["ease"].Contains("ioexpo"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutExponential;
		}
		else if (map["ease"].Contains("easeInAndOutQuadratic") || map["ease"].Contains("ioquad"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutQuadratic;
		}
		else if (map["ease"].Contains("easeInAndOutQuartic") || map["ease"].Contains("ioquart"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutQuartic;
		}
		else if (map["ease"].Contains("easeInAndOutQuintic") || map["ease"].Contains("ioquint"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutQuintic;
		}
		else if (map["ease"].Contains("easeInAndOutSine") || map["ease"].Contains("iosine"))
		{
			easeType = EEaseType::ItweenEaseType::easeInAndOutSine;
		}
		else if (map["ease"].Contains("easeInBack") || map["ease"].Contains("iback"))
		{
			easeType = EEaseType::ItweenEaseType::easeInBack;
		}
		else if (map["ease"].Contains("easeInBounce") || map["ease"].Contains("ibounce"))
		{
			easeType = EEaseType::ItweenEaseType::easeInBounce;
		}
		else if (map["ease"].Contains("easeInCircular") || map["ease"].Contains("icirc"))
		{
			easeType = EEaseType::ItweenEaseType::easeInCircular;
		}
		else if (map["ease"].Contains("easeInCubic") || map["ease"].Contains("icubic"))
		{
			easeType = EEaseType::ItweenEaseType::easeInCubic;
		}
		else if (map["ease"].Contains("easeInElastic") || map["ease"].Contains("ielas"))
		{
			easeType = EEaseType::ItweenEaseType::easeInElastic;
		}
		else if (map["ease"].Contains("easeInExponential") || map["ease"].Contains("iexpo"))
		{
			easeType = EEaseType::ItweenEaseType::easeInExponential;
		}
		else if (map["ease"].Contains("easeInQuadratic") || map["ease"].Contains("iquad"))
		{
			easeType = EEaseType::ItweenEaseType::easeInQuadratic;
		}
		else if (map["ease"].Contains("easeInQuartic") || map["ease"].Contains("iquart"))
		{
			easeType = EEaseType::ItweenEaseType::easeInQuartic;
		}
		else if (map["ease"].Contains("easeInQuintic") || map["ease"].Contains("iquint"))
		{
			easeType = EEaseType::ItweenEaseType::easeInQuintic;
		}
		else if (map["ease"].Contains("easeInSine") || map["ease"].Contains("isine"))
		{
			easeType = EEaseType::ItweenEaseType::easeInSine;
		}
		else if (map["ease"].Contains("easeOutBack") || map["ease"].Contains("oback"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutBack;
		}
		else if (map["ease"].Contains("easeOutBounce") || map["ease"].Contains("obounce"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutBounce;
		}
		else if (map["ease"].Contains("easeOutCircular") || map["ease"].Contains("ocirc"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutCircular;
		}
		else if (map["ease"].Contains("easeOutCubic") || map["ease"].Contains("ocubic"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutCubic;
		}
		else if (map["ease"].Contains("easeOutElastic") || map["ease"].Contains("oelas"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutElastic;
		}
		else if (map["ease"].Contains("easeOutExponential") || map["ease"].Contains("oexpo"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutExponential;
		}
		else if (map["ease"].Contains("easeOutQuadratic") || map["ease"].Contains("oquad"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutQuadratic;
		}
		else if (map["ease"].Contains("easeOutQuartic") || map["ease"].Contains("oquart"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutQuartic;
		}
		else if (map["ease"].Contains("easeOutQuintic") || map["ease"].Contains("oquint"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutQuintic;
		}
		else if (map["ease"].Contains("easeOutSine") || map["ease"].Contains("osine"))
		{
			easeType = EEaseType::ItweenEaseType::easeOutSine;
		}

		UiTween::Print("easeType: " + map["ease"]);
	}

	if (map.Contains("timerinterval"))
	{
		map.Add("tiin", map["timerinterval"]);
		map.Remove(map["timerinterval"]);
	}

	if (map.Contains("interval"))
	{
		map.Add("tiin", map["interval"]);
		map.Remove(map["interval"]);
	}

	if (map.Contains("tiin"))
	{
		map["tiin"].ReplaceInline(TEXT("("), TEXT(" "));
		map["tiin"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["tiin"].ReplaceInline(TEXT("["), TEXT(" "));
		map["tiin"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["tiin"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["tiin"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["tiin"].TrimStartAndEndInline();

		timerInterval = FCString::Atof(*map["tiin"]);

		if (map["tiin"].Contains("*"))
		{
			FString left;
			FString right;
			map["tiin"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			timerInterval *= FCString::Atof(*right);
		}

		if (map["tiin"].Contains("fps"))
		{
			timerInterval = 1 / timerInterval;
		}

		UiTween::Print("timerInterval: " + FString::SanitizeFloat(timerInterval));
	}

	if (map.Contains("tietoobjectvalidity"))
	{
		map.Add("tov", map["tietoobjectvalidity"]);
		map.Remove(map["tietoobjectvalidity"]);
	}

	if (map.Contains("tova"))
	{
		if (map["tov"].Contains("true"))
		{
			tieToObjectValidity = true;
		}
		else
		{
			tieToObjectValidity = false;
		}

		UiTween::Print("tieToObjectValidity: " + map["tov"]);
	}

	if (map.Contains("enforcevalueto"))
	{
		map.Add("evt", map["enforcevalueto"]);
		map.Remove(map["enforcevalueto"]);
	}

	if (map.Contains("evt"))
	{
		if (map["evt"].Contains("true"))
		{
			enforceValueTo = true;
		}
		else
		{
			enforceValueTo = false;
		}

		UiTween::Print("enforceValueTo: " + map["evt"]);
	}

	if (map.Contains("maximumloopsections"))
	{
		map.Add("mls", map["maximumloopsections"]);
		map.Remove(map["maximumloopsections"]);
	}

	if (map.Contains("maxloops"))
	{
		map.Add("mls", map["maxloops"]);
		map.Remove(map["maxloops"]);
	}

	if (map.Contains("mls"))
	{
		map["mls"].ReplaceInline(TEXT("("), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("["), TEXT(" "));
		map["mls"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["mls"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["mls"].TrimStartAndEndInline();

		maximumLoopSections = FCString::Atoi(*map["mls"]);

		if (map["mls"].Contains("*"))
		{
			FString left;
			FString right;
			map["mls"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			maximumLoopSections *= FCString::Atoi(*right);
		}

		UiTween::Print("maximumLoopSections: " + FString::FromInt(maximumLoopSections));
	}

	if (map.Contains("cullnonrenderedtweens"))
	{
		map.Add("cull", map["cullnonrenderedtweens"]);
		map.Remove(map["cullnonrenderedtweens"]);
	}

	if (map.Contains("cull"))
	{
		if (map["cull"].Contains("true"))
		{
			cullNonRenderedTweens = true;
		}
		else
		{
			cullNonRenderedTweens = false;
		}

		UiTween::Print("cullNonRenderedTweens: " + map["cull"]);
	}

	if (map.Contains("secondstowaitbeforecull"))
	{
		map.Add("stwbc", map["secondstowaitbeforecull"]);
		map.Remove(map["secondstowaitbeforecull"]);
	}

	if (map.Contains("cullwait"))
	{
		map.Add("stwbc", map["cullwait"]);
		map.Remove(map["cullwait"]);
	}

	if (map.Contains("stwbc"))
	{
		map["stwbc"].ReplaceInline(TEXT("("), TEXT(" "));
		map["stwbc"].ReplaceInline(TEXT("{"), TEXT(" "));
		map["stwbc"].ReplaceInline(TEXT("["), TEXT(" "));
		map["stwbc"].ReplaceInline(TEXT(")"), TEXT(" "));
		map["stwbc"].ReplaceInline(TEXT("}"), TEXT(" "));
		map["stwbc"].ReplaceInline(TEXT("]"), TEXT(" "));

		map["stwbc"].TrimStartAndEndInline();

		secondsToWaitBeforeCull = FCString::Atof(*map["stwbc"]);

		if (map["stwbc"].Contains("*"))
		{
			FString left;
			FString right;
			map["stwbc"].Split("*", &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			secondsToWaitBeforeCull *= FCString::Atof(*right);
		}

		UiTween::Print("secondsToWaitBeforeCull: " + FString::SanitizeFloat(secondsToWaitBeforeCull));
	}

	if (map.Contains("startfunction"))
	{
		map.Add("osfn", map["startfunction"]);
		map.Remove(map["startfunction"]);
	}

	if (map.Contains("osfn"))
	{
		OnTweenStartFunctionName = map["osfn"];
	}

	if (map.Contains("updatefunction"))
	{
		map.Add("oufn", map["updatefunction"]);
		map.Remove(map["updatefunction"]);
	}

	if (map.Contains("tickfunction"))
	{
		map.Add("oufn", map["tickfunction"]);
		map.Remove(map["tickfunction"]);
	}

	if (map.Contains("oufn"))
	{
		OnTweenUpdateFunctionName = map["oufn"];
	}

	if (map.Contains("loopfunction"))
	{
		map.Add("olfn", map["loopfunction"]);
		map.Remove(map["loopfunction"]);
	}

	if (map.Contains("olfn"))
	{
		OnTweenLoopFunctionName = map["olfn"];
	}

	//of course it's a...
	if (map.Contains("completefunction"))
	{
		map.Add("ocfn", map["completefunction"]);
		map.Remove(map["completefunction"]);
	}

	if (map.Contains("ocfn"))
	{
		OnTweenCompleteFunctionName = map["ocfn"];
	}
}

void AiTweenEvent::IgnorePauseTimeDilationSettings()
{
	AActor* actor;
	FString boo;

	if (eventType == EEventType::ItweenEventType::actorMoveFromTo || eventType == EEventType::ItweenEventType::actorRotateFromTo || eventType == EEventType::ItweenEventType::actorScaleFromTo || eventType == EEventType::ItweenEventType::actorMoveToSplinePoint || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint)
	{
		if (actorTweening)
		{
			actor = actorTweening;
		}
		else
		{
			actor = this;
		}
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo || eventType == EEventType::ItweenEventType::compRotateFromTo || eventType == EEventType::ItweenEventType::compScaleFromTo || eventType == EEventType::ItweenEventType::compMoveToSplinePoint || eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
	{
		if (componentTweening)
		{
			actor = componentTweening->GetOwner();
		}
		else
		{
			actor = this;
		}
	}
	else
	{
		actor = this;
	}

	if (tickWhenPaused)
	{
		//Needs to be able to get whether actor is tickable when paused
		actorTickableWhenPaused = actor->PrimaryActorTick.bTickEvenWhenPaused;
		this->SetTickableWhenPaused(tickWhenPaused);
		actor->SetTickableWhenPaused(tickWhenPaused);
	}

	if (actor->PrimaryActorTick.bTickEvenWhenPaused)
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = true";
	}
	else
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = false";
	}
	UiTween::Print(boo);

	if (ignoreTimeDilation)
	{
		actorTimeDilation = actor->CustomTimeDilation;
		//this->CustomTimeDilation = actor->CustomTimeDilation = (1 / GetWorldSettings()->TimeDilation);
	}
}

void AiTweenEvent::RestorePauseTimeDilationSettings()
{
	AActor* actor;
	FString boo;

	if (eventType == EEventType::ItweenEventType::actorMoveFromTo || eventType == EEventType::ItweenEventType::actorRotateFromTo || eventType == EEventType::ItweenEventType::actorScaleFromTo || eventType == EEventType::ItweenEventType::actorMoveToSplinePoint || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint)
	{
		if (actorTweening)
		{
			actor = actorTweening;
		}
		else
		{
			actor = this;
		}
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo || eventType == EEventType::ItweenEventType::compRotateFromTo || eventType == EEventType::ItweenEventType::compScaleFromTo || eventType == EEventType::ItweenEventType::compMoveToSplinePoint || eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
	{
		if (componentTweening)
		{
			actor = componentTweening->GetOwner();
		}
		else
		{
			actor = this;
		}
	}
	else
	{
		actor = this;
	}

	if (tickWhenPaused)
	{
		//Resets actor back to not tickable when paused for now until we can get whether an actor is tickable when paused
		actor->SetTickableWhenPaused(actorTickableWhenPaused);
	}

	if (actor->PrimaryActorTick.bTickEvenWhenPaused)
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = true";
	}
	else
	{
		boo = "PrmaryActorTick.bTickEvenWhenPaused = false";
	}
	UiTween::Print(boo);

	if (ignoreTimeDilation)
	{
		actor->CustomTimeDilation = actorTimeDilation;
	}
}

void AiTweenEvent::SetTickTypeValue()
{
	if (tickType != ETickType::ItweenTickType::seconds)
	{
		if (eventType == EEventType::ItweenEventType::actorMoveFromTo || eventType == EEventType::ItweenEventType::actorScaleFromTo || eventType == EEventType::ItweenEventType::compMoveFromTo || eventType == EEventType::ItweenEventType::compScaleFromTo || eventType == EEventType::ItweenEventType::vectorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(vectorFrom, vectorTo) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == EEventType::ItweenEventType::actorRotateFromTo || eventType == EEventType::ItweenEventType::compRotateFromTo || eventType == EEventType::ItweenEventType::rotatorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(rotatorFrom.Vector(), rotatorTo.Vector()) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint || eventType == EEventType::ItweenEventType::compMoveToSplinePoint || eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
		{
			tickTypeValue = (FMath::Abs(0 - splineComponent->Duration) / tickTypeValue);
		}
		else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo || eventType == EEventType::ItweenEventType::umgRTScaleFromTo || eventType == EEventType::ItweenEventType::umgRTShearFromTo || eventType == EEventType::ItweenEventType::vector2DFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0)) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
		else if (eventType == EEventType::ItweenEventType::floatFromTo || eventType == EEventType::ItweenEventType::umgRTRotateFromTo)
		{
			tickTypeValue = (FMath::Abs(floatFrom - floatTo) / tickTypeValue);
		}
		else if (eventType == EEventType::ItweenEventType::linearColorFromTo)
		{
			tickTypeValue = ((UiTween::GetDistanceBetweenTwoVectors(FVector(linearColorFrom.R, linearColorFrom.G, linearColorFrom.B), FVector(linearColorTo.R, linearColorTo.G, linearColorTo.B)) / tickTypeValue) / GetWorldSettings()->WorldToMeters);
		}
	}
}

float AiTweenEvent::GetAlphaFromEquation(float value)
{
	//Big thanks to Bob Berkebile at PixelPlacement for the original iTween equation interpretations. Unlike the rest of 
	//iTween for UE4, Easetype equations are taken almost 100% verbatim from iTween source code (with Bob's permission, 
	//of course).

	//TERMS OF USE - EASING EQUATIONS Open source under the BSD License. Copyright(c)2001 Robert PennerAll rights reserved. 

	//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
	//following conditions are met : Redistributions of source code must retain the above copyright notice, this list 
	//of conditions and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
	//notice, this list of conditions and the following disclaimer in the documentation and / or other materials 
	//provided with the distribution. Neither the name of the author nor the names of contributors may be used to endorse 
	//or promote products derived from this software without specific prior written permission. THIS SOFTWARE IS PROVIDED 
	//BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
	//TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
	//THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
	//CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
	//OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
	//LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
	//ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	switch (easeType)
	{

	case EEaseType::ItweenEaseType::linear:
		return value;

	case EEaseType::ItweenEaseType::spring:
	{
		value = FMath::Clamp<float>(value, start, end);
		value = (FMath::Sin(value * pi * (0.2f + 2.5f * value * value * value)) * FMath::Pow(1.f - value, 2.2f) + value) * (1.f + (1.2f * (1.f - value)));
		return start + (end - start) * value;
	}


	case EEaseType::ItweenEaseType::easeInQuadratic:
	{
		end -= start;
		return end * value * value + start;
	}


	case EEaseType::ItweenEaseType::easeOutQuadratic:
	{
		end -= start;
		return -end * value * (value - 2) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutQuadratic:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return end / 2 * value * value + start;
		value--;
		return -end / 2 * (value * (value - 2) - 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInCubic:
	{
		end -= start;
		return end * value * value * value + start;
	}


	case EEaseType::ItweenEaseType::easeOutCubic:
	{
		value--;
		end -= start;
		return end * (value * value * value + 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutCubic:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return end / 2 * value * value * value + start;
		value -= 2;
		return end / 2 * (value * value * value + 2) + start;
	}


	case EEaseType::ItweenEaseType::easeInQuartic:
	{
		end -= start;
		return end * value * value * value * value + start;
	}


	case EEaseType::ItweenEaseType::easeOutQuartic:
	{
		value--;
		end -= start;
		return -end * (value * value * value * value - 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutQuartic:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return end / 2 * value * value * value * value + start;
		value -= 2;
		return -end / 2 * (value * value * value * value - 2) + start;
	}


	case EEaseType::ItweenEaseType::easeInQuintic:
	{
		end -= start;
		return end * value * value * value * value * value + start;
	}


	case EEaseType::ItweenEaseType::easeOutQuintic:
	{
		value--;
		end -= start;
		return end * (value * value * value * value * value + 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutQuintic:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return end / 2 * value * value * value * value * value + start;
		value -= 2;
		return end / 2 * (value * value * value * value * value + 2) + start;
	}


	case EEaseType::ItweenEaseType::easeInSine:
	{
		end -= start;
		return -end * FMath::Cos(value / 1 * (pi / 2)) + end + start;
	}


	case EEaseType::ItweenEaseType::easeOutSine:
	{
		end -= start;
		return end * FMath::Sin(value / 1 * (pi / 2)) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutSine:
	{
		end -= start;
		return -end / 2 * (FMath::Cos(pi * value / 1) - 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInExponential:
	{
		end -= start;
		return end * FMath::Pow(2, 10 * (value / 1 - 1)) + start;
	}


	case EEaseType::ItweenEaseType::easeOutExponential:
	{
		end -= start;
		return end * (-FMath::Pow(2, -10 * value / 1) + 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutExponential:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return end / 2 * FMath::Pow(2, 10 * (value - 1)) + start;
		value--;
		return end / 2 * (-FMath::Pow(2, -10 * value) + 2) + start;
	}


	case EEaseType::ItweenEaseType::easeInCircular:
	{
		end -= start;
		return -end * (FMath::Sqrt(1 - value * value) - 1) + start;
	}


	case EEaseType::ItweenEaseType::easeOutCircular:
	{
		value--;
		end -= start;
		return end * FMath::Sqrt(1 - value * value) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutCircular:
	{
		value /= 0.5f;
		end -= start;
		if (value < 1) return -end / 2 * (FMath::Sqrt(1 - value * value) - 1) + start;
		value -= 2;
		return end / 2 * (FMath::Sqrt(1 - value * value) + 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInBounce:
		return EaseInBounce(value);

	case EEaseType::ItweenEaseType::easeOutBounce:
		return EaseOutBounce(value);


	case EEaseType::ItweenEaseType::easeInAndOutBounce:
		return EaseInOutBounce(value);

	case EEaseType::ItweenEaseType::easeInBack:
	{
		end -= start;
		value /= 1;
		float s = 1.70158f;
		return end * (value)* value * ((s + 1) * value - s) + start;
	}


	case EEaseType::ItweenEaseType::easeOutBack:
	{
		float s = 1.70158f;
		end -= start;
		value = (value / 1) - 1;
		return end * ((value)* value * ((s + 1) * value + s) + 1) + start;
	}


	case EEaseType::ItweenEaseType::easeInAndOutBack:
	{
		float s = 1.70158f;
		end -= start;
		value /= 0.5f;
		if ((value) < 1) {
			s *= (1.525f);
			return end / 2 * (value * value * (((s)+1) * value - s)) + start;
		}
		value -= 2;
		s *= (1.525f);
		return end / 2 * ((value)* value * (((s)+1) * value + s) + 2) + start;
	}


	case EEaseType::ItweenEaseType::punch:
	{
		float s = 9;
		if (value == 0) {
			return 0;
		}
		if (value == 1) {
			return 0;
		}
		float period = 1 * 0.3f;
		s = period / (2 * pi) * FMath::Asin(0.f);
		return (punchAmplitude * FMath::Pow(2, -10 * value) * FMath::Sin((value * 1 - s) * (2 * pi) / period));
	}


	case EEaseType::ItweenEaseType::easeInElastic:
	{
		end -= start;

		float d = 1.f;
		float p = d * 0.3f;
		float s = 0;
		float a = 0;

		if (value == 0) return start;

		if ((value /= d) == 1) return start + end;

		if (a == 0.f || a < FMath::Abs(end)) {
			a = end;
			s = p / 4;
		}
		else {
			s = p / (2 * pi) * FMath::Asin(end / a);
		}
		value -= 1;
		return -(a * FMath::Pow(2, 10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
	}


	case EEaseType::ItweenEaseType::easeOutElastic:
		//Thank you to rafael.marteleto for fixing this as a port over from Pedro's UnityTween
	{
		end -= start;

		float d = 1.f;
		float p = d * 0.3f;
		float s = 0;
		float a = 0;

		if (value == 0) return start;

		if ((value /= d) == 1) return start + end;

		if (a == 0.f || a < FMath::Abs(end)) {
			a = end;
			s = p / 4;
		}
		else {
			s = p / (2 * pi) * FMath::Asin(end / a);
		}

		return (a * FMath::Pow(2, -10 * value) * FMath::Sin((value * d - s) * (2 * pi) / p) + end + start);
	}


	case EEaseType::ItweenEaseType::easeInAndOutElastic:
	{
		end -= start;

		float d = 1.f;
		float p = d * 0.3f;
		float s = 0;
		float a = 0;

		if (value == 0) return start;

		if ((value /= d / 2) == 2) return start + end;

		if (a == 0.f || a < FMath::Abs(end)) {
			a = end;
			s = p / 4;
		}
		else {
			s = p / (2 * pi) * FMath::Asin(end / a);
		}

#if PLATFORM_WINDOWS || PLATFORM_LINUX 
		if (value < 1) return -0.5f * (a * FMath::Pow(2, 10 * (value -= 1)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
		return a * FMath::Pow(2, -10 * (value -= 1)) * FMath::Sin((value * d - s) * (2 * pi) / p) * 0.5f + end + start;
#else //todo: This is still broken on mobile, but at least you'll be able to build. Instead, try using ease in elastic then ease out elastic.
		value = value - 1;
		if (value < 1) return -0.5f * (a * FMath::Pow(2, 10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p)) + start;
		return a * FMath::Pow(2, -10 * (value)) * FMath::Sin((value * d - s) * (2 * pi) / p) * 0.5f + end + start;
#endif
	}

	case EEaseType::ItweenEaseType::customCurve:
	{
		if (customEaseTypeCurve)
		{
			return customEaseTypeCurve->GetFloatValue(value);
		}
		else
		{
			return value;
		}
	}
	default:
		return value;
	}
}

float AiTweenEvent::EaseInBounce(float value)
{
	end -= start;
	float d = 1.f;
	return end - EaseOutBounce(d - value) + start;
}

float AiTweenEvent::EaseOutBounce(float value)
{
	value /= 1.f;
	end -= start;
	if (value < (1 / 2.75f)) {
		return end * (7.5625f * value * value) + start;
	}
	else if (value < (2 / 2.75f)) {
		value -= (1.5f / 2.75f);
		return end * (7.5625f * (value)* value + 0.75f) + start;
	}
	else if (value < (2.5 / 2.75)) {
		value -= (2.25f / 2.75f);
		return end * (7.5625f * (value)* value + 0.9375f) + start;
	}
	else {
		value -= (2.625f / 2.75f);
		return end * (7.5625f * (value)* value + 0.984375f) + start;
	}
}

float AiTweenEvent::EaseInOutBounce(float value)
{
	end -= start;
	float d = 1.f;
	if (value < d / 2)
	{
		return EaseInBounce(value *= 2) * 0.5f + start;
	}
	else
	{
		return EaseOutBounce(value * 2 - d) * 0.5f + end*0.5f + start;
	}
}

void AiTweenEvent::EndPhase()
{
	shouldTween = false;
	shouldTick = false;

	//OnTweenComplete Interface Message
	RunInterface(onTweenCompleteTarget, ETweenInterfaceType::ItweenInterfaceType::complete);

	RestorePauseTimeDilationSettings();

	if (splineComponent && destroySplineObject)
	{
		splineComponent->GetOwner()->Destroy();
	}

	this->Destroy();
}

void AiTweenEvent::DelayTween()
{
	if (delayCount < delay)
	{
		delayCount += deltaSeconds;
	}
	else
	{
		if (!shouldTween)
		{
			FirstSet();

			//OnTweenStart Interface Message
			RunInterface(onTweenStartTarget, ETweenInterfaceType::ItweenInterfaceType::start);

			shouldTween = true;

			ExecuteTween();
		}
	}
}

void AiTweenEvent::LoopSectionStuff()
{
	numberOfLoopSections++;

	if (maximumLoopSections != 0)
	{
		if (maximumLoopSections < 0)
		{
			maximumLoopSections *= -1;
		}
		if (numberOfLoopSections >= maximumLoopSections)
		{
			EndPhase();
		}
		else
		{
			//OnTweenLoop Interface Message
			RunInterface(onTweenLoopTarget, ETweenInterfaceType::ItweenInterfaceType::loop);
		}
	}
}

void AiTweenEvent::LoopWithDelay()
{
	shouldTween = false;
	shouldTick = false;
	SetTickTypeValue();
	alpha = 0.f;
	delayCount = 0;
	shouldDelay = true;
	if (!isTweenPaused)
	{
		shouldTick = true;
	}
}

void AiTweenEvent::LoopWithoutDelay()
{
	shouldTween = false;
	shouldTick = false;
	SetTickTypeValue();
	alpha = 0.f;
	if (!isTweenPaused)
	{
		shouldTick = true;
		shouldTween = true;
	}
}

void AiTweenEvent::SpacializeValues()
{
	if (eventType == EEventType::ItweenEventType::actorMoveFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::self)
		{
			vectorTo = (actorTweening->GetActorRotation().RotateVector(vectorTo) + actorTweening->GetActorLocation());
		}
		else if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			if (actorTweening->GetRootComponent()->GetAttachParent())
			{
				vectorTo = (actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorRotation().RotateVector(vectorTo) + actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorLocation());
			}
		}
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::self)
		{
			rotatorTo = FRotator(FQuat(actorTweening->GetActorRotation()) * FQuat(rotatorTo));
		}
		else if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			if (actorTweening->GetRootComponent()->GetAttachParent())
			{
				rotatorTo = FRotator(FQuat(actorTweening->GetRootComponent()->GetAttachParent()->GetOwner()->GetActorRotation()) * FQuat(rotatorTo));

			}
		}
	}
	else if (eventType == EEventType::ItweenEventType::actorScaleFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			vectorTo = actorTweening->GetRootComponent()->GetAttachParent()->GetComponentScale() * vectorTo;
		}
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::self)
		{
			vectorTo = (componentTweening->GetComponentRotation().RotateVector(vectorTo) + componentTweening->GetComponentLocation());
		}
		else if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			if (componentTweening->GetOwner())
			{
				vectorTo = (componentTweening->GetOwner()->GetActorRotation().RotateVector(vectorTo) + componentTweening->GetOwner()->GetActorLocation());
			}
		}
	}
	else if (eventType == EEventType::ItweenEventType::compRotateFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::self)
		{
			rotatorTo = FRotator(FQuat(componentTweening->GetComponentRotation()) * FQuat(rotatorTo));
		}
		else if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			if (componentTweening->GetOwner())
			{
				rotatorTo = FRotator(FQuat(componentTweening->GetOwner()->GetActorRotation()) * FQuat(rotatorTo));
			}
		}
	}
	else if (eventType == EEventType::ItweenEventType::compScaleFromTo)
	{
		if (coordinateSpace == ECoordinateSpace::ItweenCoordinateSpace::parent)
		{
			vectorTo = componentTweening->GetOwner()->GetActorScale3D() * vectorTo;
		}
	}
}

void AiTweenEvent::SwitchValues()
{
	if (eventType == EEventType::ItweenEventType::actorMoveFromTo || eventType == EEventType::ItweenEventType::actorScaleFromTo || eventType == EEventType::ItweenEventType::compMoveFromTo || eventType == EEventType::ItweenEventType::compScaleFromTo || eventType == EEventType::ItweenEventType::vectorFromTo)
	{
		vectorTemp = vectorFrom;
		vectorFrom = vectorTo;
		vectorTo = vectorTemp;
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateFromTo || eventType == EEventType::ItweenEventType::compRotateFromTo || eventType == EEventType::ItweenEventType::rotatorFromTo)
	{
		rotatorTemp = rotatorFrom;
		rotatorFrom = rotatorTo;
		rotatorTo = rotatorTemp;
	}
	else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo || eventType == EEventType::ItweenEventType::umgRTScaleFromTo || eventType == EEventType::ItweenEventType::umgRTShearFromTo || eventType == EEventType::ItweenEventType::vector2DFromTo || eventType == EEventType::ItweenEventType::slateMoveFromTo || eventType == EEventType::ItweenEventType::slateScaleFromTo)
	{
		vector2DTemp = vector2DFrom;
		vector2DFrom = vector2DTo;
		vector2DTo = vector2DTemp;
	}
	else if (eventType == EEventType::ItweenEventType::floatFromTo || eventType == EEventType::ItweenEventType::umgRTRotateFromTo)
	{
		floatTemp = floatFrom;
		floatFrom = floatTo;
		floatTo = floatTemp;
	}
	else if (eventType == EEventType::ItweenEventType::linearColorFromTo)
	{
		linearColorTemp = linearColorFrom;
		linearColorFrom = linearColorTo;
		linearColorTo = linearColorTemp;
	}
	else
	{
		//dummy code, disregard
	}
}

FString AiTweenEvent::GetEventTypeEnumAsFString(EEventType::ItweenEventType enumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEventType"), true);

	if (!EnumPtr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetNameStringByIndex((int32)enumValue);
}

bool AiTweenEvent::IsEventDataType()
{
	if (eventType == EEventType::ItweenEventType::floatFromTo || eventType == EEventType::ItweenEventType::linearColorFromTo || eventType == EEventType::ItweenEventType::vectorFromTo || eventType == EEventType::ItweenEventType::vector2DFromTo || eventType == EEventType::ItweenEventType::rotatorFromTo)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AiTweenEvent::RunInterface(UObject* target, ETweenInterfaceType::ItweenInterfaceType type)
{
	if (type == ETweenInterfaceType::ItweenInterfaceType::start && OnTweenStartDelegate.IsBound())
	{
		OnTweenStartDelegate.Execute();
	}
	else if (type == ETweenInterfaceType::ItweenInterfaceType::update && OnTweenUpdateDelegate.IsBound())
	{
		OnTweenUpdateDelegate.Execute();
	}
	else if (type == ETweenInterfaceType::ItweenInterfaceType::loop && OnTweenLoopDelegate.IsBound())
	{
		OnTweenLoopDelegate.Execute();
	}
	else if (type == ETweenInterfaceType::ItweenInterfaceType::complete && OnTweenCompleteDelegate.IsBound())
	{
		OnTweenCompleteDelegate.Execute();
	}

	IiTInterface* interf = Cast<IiTInterface>(target);

	if (interf)
	{
		if (type == ETweenInterfaceType::ItweenInterfaceType::start)
		{
			interf->OnTweenStartNative(this, actorTweening, componentTweening, widgetTweening, tweenName);
		}
		else if (type == ETweenInterfaceType::ItweenInterfaceType::update)
		{
			interf->OnTweenUpdateNative(this, actorTweening, componentTweening, widgetTweening, tweenName, dtv, alpha);
		}
		else if (type == ETweenInterfaceType::ItweenInterfaceType::loop)
		{
			interf->OnTweenLoopNative(this, actorTweening, componentTweening, widgetTweening, tweenName, numberOfLoopSections, loopType, playingBackward);
		}
		else if (type == ETweenInterfaceType::ItweenInterfaceType::complete)
		{
			interf->OnTweenCompleteNative(this, actorTweening, componentTweening, widgetTweening, tweenName, sweepResult, successfulTransform);
		}
		else
		{
			UiTween::Print("Target is not defined as iTweenEvent message-accepting type.", "error");
		}
	}
	else
	{

		if (target && target->GetClass()->ImplementsInterface(UiTInterface::StaticClass()))
		{
			if (type == ETweenInterfaceType::ItweenInterfaceType::start)
			{
				IiTInterface::Execute_OnTweenStart(target, this, actorTweening, componentTweening, widgetTweening, tweenName);
			}
			else if (type == ETweenInterfaceType::ItweenInterfaceType::update)
			{
				IiTInterface::Execute_OnTweenUpdate(target, this, actorTweening, componentTweening, widgetTweening, tweenName, dtv, alpha);
			}
			else if (type == ETweenInterfaceType::ItweenInterfaceType::loop)
			{
				IiTInterface::Execute_OnTweenLoop(target, this, actorTweening, componentTweening, widgetTweening, tweenName, numberOfLoopSections, loopType, playingBackward);
			}
			else if (type == ETweenInterfaceType::ItweenInterfaceType::complete)
			{
				IiTInterface::Execute_OnTweenComplete(target, this, actorTweening, componentTweening, widgetTweening, tweenName, sweepResult, successfulTransform);
			}
			else
			{
				UiTween::Print("Target is not defined as iTweenEvent message-accepting type.", "error");
			}
		}
		else
		{
			/*if (target)
			{
			UiTween::Print("Cannot send Interface message to" + target->GetFName().ToString(), "error");
			}
			else
			{
			UiTween::Print("Cannot send Interface message.", "error");
			}*/
			;
		}
	}
}

void AiTweenEvent::CheckLoopType()
{
	switch (loopType)
	{
	case ELoopType::ItweenLoopType::once:
		EndPhase();
		break;
	case ELoopType::ItweenLoopType::rewind:
		LoopSectionStuff();
		if (delayType == EDelayType::ItweenDelayType::first)
		{
			LoopWithoutDelay();
		}
		else
		{
			LoopWithDelay();
		}
		break;
	case ELoopType::ItweenLoopType::pingPong:
	{
		playingBackward = !playingBackward;
		LoopSectionStuff();
		SwitchValues();
		/*if (isLocal)
		{
		SpacializeValues();
		}*/
		if (delayType == EDelayType::ItweenDelayType::first)
		{
			LoopWithoutDelay();
		}
		else if (delayType == EDelayType::ItweenDelayType::firstLoop || delayType == EDelayType::ItweenDelayType::loop)
		{
			LoopWithDelay();
		}
		else
		{
			if (playingBackward)
			{
				LoopWithoutDelay();
			}
			else
			{
				LoopWithDelay();
			}
		}
		break;
	}
	default:
		EndPhase();
		break;
	}
}

void AiTweenEvent::CheckIfFirstDelay()
{
	if (delayType == EDelayType::ItweenDelayType::first || delayType == EDelayType::ItweenDelayType::firstLoop || delayType == EDelayType::ItweenDelayType::firstLoopFull)
	{
		shouldDelay = true;
	}
}

void AiTweenEvent::BindDelegates()
{
	if (onTweenStartTarget && OnTweenStartFunctionName != "")
	{
		OnTweenStartDelegate.BindUFunction(onTweenStartTarget, *OnTweenStartFunctionName);
	}
	if (onTweenUpdateTarget && OnTweenUpdateFunctionName != "")
	{
		OnTweenUpdateDelegate.BindUFunction(onTweenUpdateTarget, *OnTweenUpdateFunctionName);
	}
	if (onTweenLoopTarget && OnTweenLoopFunctionName != "")
	{
		OnTweenLoopDelegate.BindUFunction(onTweenLoopTarget, *OnTweenLoopFunctionName);
	}
	if (onTweenCompleteTarget && OnTweenCompleteFunctionName != "")
	{
		OnTweenCompleteDelegate.BindUFunction(onTweenCompleteTarget, *OnTweenCompleteFunctionName);
	}
}

void AiTweenEvent::InitEvent()
{
	//if (aux && aux->performDebugOperations)
	//{
	//	NameEventActor();
	//}

	//Validate nodes
	CheckTargetValidity();

	//Check if delay is needed
	CheckIfFirstDelay();

	BindDelegates();

	IgnorePauseTimeDilationSettings();

	SpacializeValues();

	if (interpolateToSpline && splineComponent != nullptr)
	{
		if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint || eventType == EEventType::ItweenEventType::compMoveToSplinePoint || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint || eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
		{
			ReconstructSpline();
		}
	}

	//Convert time to speed (if desired and applicable)
	SetTickTypeValue();

	SetTimerInterval(timerInterval);

	shouldTick = true;
}

void AiTweenEvent::ReconstructSpline()
{
	//Make a new spline so as to not alter the original
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AiTSpline* NewSplineActor = (AiTSpline*)GetWorld()->SpawnActor<AiTSpline>(AiTSpline::StaticClass(), this->GetTransform(), params);
	USplineComponent* NewSpline = NewSplineActor->spline;

	TArray<FVector> splinePoints;
	TArray<FVector> splineTangents;

	if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint)
	{
		splinePoints.Add(actorTweening->GetActorLocation());
	}
	else if (eventType == EEventType::ItweenEventType::compMoveToSplinePoint)
	{
		splinePoints.Add(componentTweening->GetComponentLocation());
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateToSplinePoint)
	{
		splinePoints.Add(actorTweening->GetActorLocation() + (actorTweening->GetActorForwardVector() * generatedPointDistance));
	}
	else if (eventType == EEventType::ItweenEventType::compRotateToSplinePoint)
	{
		splinePoints.Add(componentTweening->GetComponentLocation() + (componentTweening->GetForwardVector() * generatedPointDistance));
	}
	for (int i = 0; i < splineComponent->GetNumberOfSplinePoints(); i++)
	{
		splinePoints.Add(splineComponent->GetWorldLocationAtSplinePoint(i));
		splineTangents.Add(splineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World));
	}

	NewSpline->ClearSplinePoints();

	//Add spline points to new spline
	for (int i = 0; i < splinePoints.Num(); i++)
	{
		NewSpline->AddSplineWorldPoint(splinePoints[i]);
	}

	//Set spline tangents
	for (int i = 0; i < splineTangents.Num(); i++)
	{
		int n = i + 1;
		if (n < NewSpline->GetNumberOfSplinePoints())
		{
			NewSpline->SetTangentAtSplinePoint(n, splineTangents[i], ESplineCoordinateSpace::World);
		}
	}

	//We want to store the old spline component, just in case the user wanted to destroy that one.
	OldSplineComponent = splineComponent;

	//The spline component used is now this new spline
	splineComponent = NewSpline;

	//Now we want to destroy the iTSpline when we're done so we don't have unnecessary extra actors.
	DestroyNewSplineObject = true;
}

//void AiTweenEvent::NameEventActor()
//{
//	this->SetActorLabel(*(tweenName.ToString() + " - " + FString::FromInt(tweenIndex) + " - " + GetEventTypeEnumAsFString(eventType)));
//}

void AiTweenEvent::CheckTargetValidity()
{
	if (!orientationTarget)
	{
		orientationTarget = aux;

		if (orientation == ELookType::ItweenLookType::orientToTarget)
		{
			orientation = ELookType::ItweenLookType::orientToPath;
			UiTween::Print("No orientation target was specified. Orientation type has been defaulted to orientToPath.", "error");
		}
	}

	if (!onTweenStartTarget)
	{
		onTweenStartTarget = aux;
	}

	if (!onTweenUpdateTarget)
	{
		onTweenUpdateTarget = aux;
	}

	if (!onTweenLoopTarget)
	{
		onTweenLoopTarget = aux;
	}

	if (!onTweenCompleteTarget)
	{
		onTweenCompleteTarget = aux;
	}

	if (!customEaseTypeCurve && easeType == EEaseType::ItweenEaseType::customCurve)
	{
		UiTween::Print("No Curve defined! You must specify a CurveFloat object that begins and a time of 0 and ends at a time of 1. Defaulting to Linear (No Easing).", "error");
	}
}

void AiTweenEvent::FirstSet()
{
	bool shouldEnd = false;

	if (!firstSet)
	{
		if (eventType == EEventType::ItweenEventType::actorMoveFromTo && actorTweening)
		{
			actorTweening->SetActorLocation(UiTween::ConstrainVector(vectorFrom, actorTweening->GetActorLocation(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::actorRotateFromTo && actorTweening)
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(rotatorFrom, actorTweening->GetActorRotation(), rotatorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::actorScaleFromTo && actorTweening)
		{
			actorTweening->SetActorScale3D(UiTween::ConstrainVector(vectorFrom, actorTweening->GetActorScale3D(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compMoveFromTo && componentTweening)
		{
			componentTweening->SetWorldLocation(UiTween::ConstrainVector(vectorFrom, componentTweening->GetComponentLocation(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compRotateFromTo && componentTweening)
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rotatorFrom, componentTweening->GetComponentRotation(), rotatorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compScaleFromTo && componentTweening)
		{
			componentTweening->SetWorldScale3D(UiTween::ConstrainVector(vectorFrom, componentTweening->GetComponentScale(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Translation, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTransformAngle(floatFrom);
		}
		else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo && widgetTweening)
		{
			widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Scale, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo && widgetTweening)
		{
			widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(vector2DFrom, widgetTweening->RenderTransform.Shear, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::floatFromTo)
		{
			dtv.floatCurrent = floatFrom;
		}
		else if (eventType == EEventType::ItweenEventType::vectorFromTo)
		{
			dtv.vectorCurrent = vectorFrom;
		}
		else if (eventType == EEventType::ItweenEventType::vector2DFromTo)
		{
			dtv.vector2DCurrent = vector2DFrom;
		}
		else if (eventType == EEventType::ItweenEventType::rotatorFromTo)
		{
			dtv.rotatorCurrent = rotatorFrom;
		}
		else if (eventType == EEventType::ItweenEventType::linearColorFromTo)
		{
			dtv.linearColorCurrent = linearColorFrom;
		}
		else if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint && actorTweening)
		{
			//UiTween::Print("dummy code, disregard");
		}
		else if (eventType == EEventType::ItweenEventType::compMoveToSplinePoint && componentTweening)
		{
			//UiTween::Print("dummy code, disregard");
		}
		else if (eventType == EEventType::ItweenEventType::actorRotateToSplinePoint && actorTweening)
		{
			//UiTween::Print("dummy code, disregard");
		}
		else if (eventType == EEventType::ItweenEventType::compRotateToSplinePoint && componentTweening)
		{
			//UiTween::Print("dummy code, disregard");
		}
		else
		{
			if (tieToObjectValidity)
			{
				shouldEnd = true;
			}
		}

		if (!shouldEnd)
		{
			OffsetTween();
		}
		else
		{
			EndPhase();
		}

		firstSet = true;
	}
}

void AiTweenEvent::LastSet()
{
	bool shouldEnd = false;

	if (enforceValueTo)
	{
		if (eventType == EEventType::ItweenEventType::actorMoveFromTo && actorTweening)
		{
			actorTweening->SetActorLocation(UiTween::ConstrainVector(vectorTo, actorTweening->GetActorLocation(), vectorConstraints), sweepTransform);
		}
		else if (eventType == EEventType::ItweenEventType::actorRotateFromTo && actorTweening)
		{
			actorTweening->SetActorRotation(UiTween::ConstrainRotator(rotatorTo, actorTweening->GetActorRotation(), rotatorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::actorScaleFromTo && actorTweening)
		{
			actorTweening->SetActorScale3D(UiTween::ConstrainVector(vectorTo, actorTweening->GetActorScale3D(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compMoveFromTo && componentTweening)
		{
			componentTweening->SetWorldLocation(UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentLocation(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compRotateFromTo && componentTweening)
		{
			componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rotatorTo, componentTweening->GetComponentRotation(), rotatorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::compScaleFromTo && componentTweening)
		{
			componentTweening->SetWorldScale3D(UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentScale(), vectorConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Translation, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo && widgetTweening)
		{
			widgetTweening->SetRenderTransformAngle(floatTo);
		}
		else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo && widgetTweening)
		{
			widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Scale, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo && widgetTweening)
		{
			widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(vector2DTo, widgetTweening->RenderTransform.Shear, vector2DConstraints));
		}
		else if (eventType == EEventType::ItweenEventType::floatFromTo)
		{
			dtv.floatCurrent = floatTo;
		}
		else if (eventType == EEventType::ItweenEventType::vectorFromTo)
		{
			dtv.vectorCurrent = vectorTo;
		}
		else if (eventType == EEventType::ItweenEventType::vector2DFromTo)
		{
			dtv.vector2DCurrent = vector2DTo;
		}
		else if (eventType == EEventType::ItweenEventType::rotatorFromTo)
		{
			dtv.rotatorCurrent = rotatorTo;
		}
		else if (eventType == EEventType::ItweenEventType::linearColorFromTo)
		{
			dtv.linearColorCurrent = linearColorTo;
		}
		else if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint && actorTweening)
		{
			EndActorMoveToSplinePoint();
		}
		else if (eventType == EEventType::ItweenEventType::compMoveToSplinePoint && componentTweening)
		{
			EndComponentMoveToSplinePoint();
		}
		else if (eventType == EEventType::ItweenEventType::actorRotateToSplinePoint && actorTweening)
		{
			EndActorRotateToSplinePoint();
		}
		else if (eventType == EEventType::ItweenEventType::compRotateToSplinePoint && componentTweening)
		{
			EndComponentRotateToSplinePoint();
		}
		else
		{
			if (tieToObjectValidity)
			{
				shouldEnd = true;
			}
		}

		if (!shouldEnd)
		{
				OffsetTween();
		}
		else
		{
			EndPhase();
		}
	}
}

void AiTweenEvent::ExecuteTween()
{
	if (shouldTick)
	{
		if (shouldTween)
		{
			alpha = FMath::Clamp<float>((alpha + (deltaSeconds / tickTypeValue)), 0.f, 1.f);

			InterpTween();

			//OnTweenTick Interface Message
			RunInterface(onTweenUpdateTarget, ETweenInterfaceType::ItweenInterfaceType::update);

			if (alpha >= 1.0f)
			{
				if (easeType != EEaseType::ItweenEaseType::punch)
				{
					LastSet();
				}

				CheckLoopType();
			}
		}
		else
		{
			if (shouldDelay)
			{
				DelayTween();
			}
			else
			{
				//OnTweenStart Interface Message
				RunInterface(onTweenStartTarget, ETweenInterfaceType::ItweenInterfaceType::start);

				FirstSet();

				shouldTween = true;
			}
		}
	}
}

void AiTweenEvent::InterpTween() //todo: add possibility of world offsets and offset constraints
{
	bool cull = false;
	if (cullNonRenderedTweens)
	{
		if (actorTweening)
		{
			if (FMath::IsNearlyEqual(actorTweening->GetLastRenderTime(), lastSavedRenderTime))
			{
				if (timeSinceLastRendered >= secondsToWaitBeforeCull)
				{
					cull = true;
				}
				timeSinceLastRendered += deltaSeconds;
			}
			else
			{
				timeSinceLastRendered = 0.f;
			}
			lastSavedRenderTime = actorTweening->GetLastRenderTime();
		}
		if (componentTweening)
		{
			if (FMath::IsNearlyEqual(componentTweening->GetOwner()->GetLastRenderTime(), lastSavedRenderTime))
			{
				if (timeSinceLastRendered >= secondsToWaitBeforeCull)
				{
					cull = true;
				}
				timeSinceLastRendered += deltaSeconds;
			}
			else
			{
				timeSinceLastRendered = 0.f;
			}
			lastSavedRenderTime = componentTweening->GetOwner()->GetLastRenderTime();
		}
	}

	bool shouldEnd = false;

	if (eventType == EEventType::ItweenEventType::actorMoveFromTo && actorTweening)
	{
		if (!cull)
		{
			TickActorMoveFromTo();
		}
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateFromTo && actorTweening)
	{
		if (!cull)
		{
			TickActorRotateFromTo();
		}
	}
	else if (eventType == EEventType::ItweenEventType::actorScaleFromTo && actorTweening)
	{
		if (!cull)
		{
			actorTweening->SetActorScale3D(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorScale3D(), vectorConstraints));
		}
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo && componentTweening)
	{
		if (!cull)
		{
			TickComponentMoveFromTo();
		}
	}
	else if (eventType == EEventType::ItweenEventType::compRotateFromTo && componentTweening)
	{
		if (!cull)
		{
			TickComponentRotateFromTo();
		}
	}
	else if (eventType == EEventType::ItweenEventType::compScaleFromTo && componentTweening)
	{
		if (!cull)
		{
			componentTweening->SetWorldScale3D(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentScale(), vectorConstraints));
		}
	}
	else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo && widgetTweening)
	{
		TickUMGRTMoveFromTo();
	}
	else if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo && widgetTweening)
	{
		widgetTweening->SetRenderTransformAngle(FMath::Lerp<float>(floatFrom, floatTo, GetAlphaFromEquation(alpha)));
	}
	else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo && widgetTweening)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		widgetTweening->SetRenderScale(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Scale, vector2DConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo && widgetTweening)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		widgetTweening->SetRenderShear(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Shear, vector2DConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::floatFromTo)
	{
		dtv.floatCurrent = FMath::Lerp<float>(floatFrom, floatTo, GetAlphaFromEquation(alpha));
	}
	else if (eventType == EEventType::ItweenEventType::vectorFromTo)
	{
		dtv.vectorCurrent = UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), dtv.vectorCurrent, vectorConstraints);
	}
	else if (eventType == EEventType::ItweenEventType::vector2DFromTo)
	{
		FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
		dtv.vector2DCurrent = UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), dtv.vector2DCurrent, vector2DConstraints);
	}
	else if (eventType == EEventType::ItweenEventType::rotatorFromTo)
	{
		TickRotatorFromTo();
	}
	else if (eventType == EEventType::ItweenEventType::linearColorFromTo)
	{
		dtv.linearColorCurrent = FMath::Lerp<FLinearColor>(linearColorFrom, linearColorTo, GetAlphaFromEquation(alpha));
	}
	else if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint && actorTweening)
	{
		if (!cull)
		{
			TickActorMoveToSplinePoint();
		}
	}
	else if (eventType == EEventType::ItweenEventType::compMoveToSplinePoint && componentTweening)
	{
		if (!cull)
		{
			TickComponentMoveToSplinePoint();
		}
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateToSplinePoint && actorTweening)
	{
		if (!cull)
		{
			TickActorRotateToSplinePoint();
		}
	}
	else if (eventType == EEventType::ItweenEventType::compRotateToSplinePoint && componentTweening)
	{
		if (!cull)
		{
			TickComponentRotateToSplinePoint();
		}
	}
	else
	{
		if (tieToObjectValidity)
		{
			shouldEnd = true;
		}
	}

	if (!shouldEnd)
	{
		if (!cull)
		{
			OffsetTween();
		}
	}
	else
	{
		EndPhase();
	}
}


void AiTweenEvent::OffsetTween()
{
	if (actorTweening || eventType == EEventType::ItweenEventType::actorMoveToSplinePoint && actorTweening)
	{
			actorTweening->AddActorLocalOffset(locationOffset, sweepTransform, &sweepResult);

			if (sweepTransform)
			{
				if (sweepResult.bBlockingHit)
				{
					EndPhase();
				}
			}
	}
	else if (eventType == EEventType::ItweenEventType::actorRotateFromTo && actorTweening || eventType == EEventType::ItweenEventType::actorRotateToSplinePoint && actorTweening)
	{
			actorTweening->AddActorLocalRotation(rotationOffset); //todo: investigate sweeping rotations
	}
	else if (eventType == EEventType::ItweenEventType::actorScaleFromTo && actorTweening)
	{
			actorTweening->SetActorScale3D(actorTweening->GetActorScale3D() + scale3DOffset);
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo && componentTweening || eventType == EEventType::ItweenEventType::compMoveToSplinePoint && componentTweening)
	{
		componentTweening->AddLocalOffset(locationOffset, sweepTransform, &sweepResult);

		if (sweepTransform)
		{
			if (sweepResult.bBlockingHit)
			{
				successfulTransform = false;
				EndPhase();
			}
		}
	}
	else if (eventType == EEventType::ItweenEventType::compRotateFromTo && componentTweening || eventType == EEventType::ItweenEventType::compRotateToSplinePoint && componentTweening)
	{
		componentTweening->AddLocalRotation(rotationOffset); //todo: investigate rotation sweeps
	}
	else if (eventType == EEventType::ItweenEventType::compScaleFromTo && componentTweening)
	{
			componentTweening->SetWorldScale3D(componentTweening->GetComponentScale() + scale3DOffset);
	}
	else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo && widgetTweening)
	{
		widgetTweening->SetRenderTranslation(widgetTweening->RenderTransform.Translation + translationOffset);
	}
	else if (eventType == EEventType::ItweenEventType::umgRTRotateFromTo && widgetTweening)
	{
		widgetTweening->SetRenderTransformAngle(widgetTweening->RenderTransform.Angle + angleOffset);
	}
	else if (eventType == EEventType::ItweenEventType::umgRTScaleFromTo && widgetTweening)
	{
		widgetTweening->SetRenderScale(widgetTweening->RenderTransform.Scale + scaleOffset);
	}
	else if (eventType == EEventType::ItweenEventType::umgRTShearFromTo && widgetTweening)
	{
		widgetTweening->SetRenderShear(widgetTweening->RenderTransform.Shear + shearOffset);
	}
}

void AiTweenEvent::OrientationSwitch()
{
	switch (orientation)
	{
	case ELookType::ItweenLookType::noOrientationChange:
		break;
	case ELookType::ItweenLookType::orientToTarget:
		if (orientationTarget)
		{
			OrientToTargetFunction();
			break;
		}
		else
		{
			OrientToPathFunction();
			break;
		}
	case ELookType::ItweenLookType::orientToPath:
		OrientToPathFunction();
		break;
	default:
		break;
	}
}

void AiTweenEvent::OrientToTargetFunction()
{
	if (eventType == EEventType::ItweenEventType::actorMoveFromTo || eventType == EEventType::ItweenEventType::actorMoveToSplinePoint)
	{
		if (orientationTarget->IsA(AActor::StaticClass()))
		{
			if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
			{
				actorTweening->SetActorRotation(FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - actorTweening->GetActorLocation()).Rotator());
			}
			else
			{
				UiTween::ActorRotateUpdate(actorTweening, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - actorTweening->GetActorLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
			}
		}
		else if (orientationTarget->IsA(USceneComponent::StaticClass()))
		{
			if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
			{
				actorTweening->SetActorRotation(FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - actorTweening->GetActorLocation()).Rotator());
			}
			else
			{
				UiTween::ActorRotateUpdate(actorTweening, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - actorTweening->GetActorLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
			}
		}
		else if (orientationTarget->IsA(UWidget::StaticClass()))
		{
			//todo: Feature is still to come. Waiting on getting the ability to read widget position in screen space
			//UiTween::Print("dummy code, disregard");
		}

	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo || eventType == EEventType::ItweenEventType::compMoveToSplinePoint)
	{
		if (orientationTarget->IsA(AActor::StaticClass()))
		{
			if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
			{
				componentTweening->SetWorldRotation(FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - componentTweening->GetComponentLocation()).Rotator());
			}
			else
			{
				UiTween::ComponentRotateUpdate(componentTweening, FRotationMatrix::MakeFromX(((AActor*)orientationTarget)->GetActorLocation() - componentTweening->GetComponentLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
			}
		}
		else if (orientationTarget->IsA(USceneComponent::StaticClass()))
		{
			if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
			{
				componentTweening->SetWorldRotation(FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - componentTweening->GetComponentLocation()).Rotator());
			}
			else
			{
				UiTween::ComponentRotateUpdate(componentTweening, FRotationMatrix::MakeFromX(((USceneComponent*)orientationTarget)->GetComponentLocation() - componentTweening->GetComponentLocation()).Rotator(), deltaSeconds, orientationSpeed, false, rotatorConstraints);
			}
		}
		else if (orientationTarget->IsA(UWidget::StaticClass()))
		{
			//todo: Feature is still to come. Waiting on getting the ability to read widget position in screen space
			//UiTween::Print("dummy code, disregard");
		}
	}
	else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo)
	{
		//todo: Waiting on getting the ability to read widget position in screen space
		/*FVector2D loc;
		UiTween::GetWorldLocal()->GetFirstPlayerController()->ProjectWorldLocationToScreen(((AActor*)orientationTarget)->GetActorLocation(), loc);
		UiTween::UMGRTRotateUpdate(widgetTweening, loc, orientationSpeed, deltaSeconds);*/
		//UiTween::Print("dummy code, disregard");
	}
}

void AiTweenEvent::OrientToPathFunction()
{
	if (eventType == EEventType::ItweenEventType::actorMoveFromTo)
	{
		FRotator newOrientation;

		if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
		{
			newOrientation = FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, actorTweening->GetActorLocation(), vectorConstraints) - vectorFrom)).Rotator();
		}
		else
		{
			newOrientation = FMath::RInterpTo(actorTweening->GetActorRotation(), FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, actorTweening->GetActorLocation(), vectorConstraints)) - vectorFrom).Rotator(), deltaSeconds, orientationSpeed);
		}

		actorTweening->SetActorRotation(UiTween::ConstrainRotator(newOrientation, actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::compMoveFromTo)
	{
		FRotator newOrientation;

		if (FMath::IsNearlyEqual(orientationSpeed, 0.f))
		{
			newOrientation = FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentLocation(), vectorConstraints) - vectorFrom)).Rotator();
		}
		else
		{
			newOrientation = FMath::RInterpTo(componentTweening->GetComponentRotation(), FRotationMatrix::MakeFromX((UiTween::ConstrainVector(vectorTo, componentTweening->GetComponentLocation(), vectorConstraints)) - vectorFrom).Rotator(), deltaSeconds, orientationSpeed);
		}

		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(newOrientation, componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::actorMoveToSplinePoint)
	{
		//if (playingBackward && switchPathOrientationDirection)
		//{
		//	FRotator rot = splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration - 0.05f).Quaternion().Rotator();

		//	actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::RInterpTo(actorTweening->GetActorRotation(), (rot), deltaSeconds, orientationSpeed), actorTweening->GetActorRotation(), rotatorConstraints));
		//}
		//else
		//{
		//	actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::RInterpTo(actorTweening->GetActorRotation(), splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), deltaSeconds, orientationSpeed), actorTweening->GetActorRotation(), rotatorConstraints));
		//}
		float amount = PathLookDistance;
		float localAlpha = GetAlphaFromEquation(alpha);

		if (playingBackward)
		{
			localAlpha = 1.0f - localAlpha;

			if (switchPathOrientationDirection)
			{
				amount *= -1;
			}
		}

		FRotator rot = FRotationMatrix::MakeFromX(splineComponent->GetWorldLocationAtTime((localAlpha + amount) * splineComponent->Duration) - actorTweening->GetActorLocation()).Rotator();

		if (!FMath::IsNearlyEqual(orientationSpeed, 0.f))
		{
			rot = FMath::RInterpTo(actorTweening->GetActorRotation(), (rot), deltaSeconds, orientationSpeed);
		}

		actorTweening->SetActorRotation(UiTween::ConstrainRotator(rot, actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::compMoveToSplinePoint)
	{
		//if (playingBackward && switchPathOrientationDirection)
		//{
		//	componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::RInterpTo(componentTweening->GetComponentRotation(), (splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration).Quaternion().Inverse().Rotator()), deltaSeconds, orientationSpeed), componentTweening->GetComponentRotation(), rotatorConstraints));
		//}
		//else
		//{
		//	componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::RInterpTo(componentTweening->GetComponentRotation(), splineComponent->GetWorldRotationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), deltaSeconds, orientationSpeed), componentTweening->GetComponentRotation(), rotatorConstraints));
		//}

		float amount = PathLookDistance;
		float localAlpha = GetAlphaFromEquation(alpha);

		if (playingBackward)
		{
			localAlpha = 1.0f - localAlpha;

			if (switchPathOrientationDirection)
			{
				amount *= -1;
			}
		}

		FRotator rot = FRotationMatrix::MakeFromX(splineComponent->GetWorldLocationAtTime((localAlpha + amount) * splineComponent->Duration) - componentTweening->GetComponentLocation()).Rotator();

		if (!FMath::IsNearlyEqual(orientationSpeed, 0.f))
		{
			rot = FMath::RInterpTo(componentTweening->GetComponentRotation(), (rot), deltaSeconds, orientationSpeed);
		}

		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rot, componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else if (eventType == EEventType::ItweenEventType::umgRTMoveFromTo)
	{
		//todo: Waiting on getting the ability to read widget position in screen space
		/*FVector2D loc;
		UiTween::GetWorldLocal()->GetFirstPlayerController()->ProjectWorldLocationToScreen(((AActor*)orientationTarget)->GetActorLocation(), loc);
		UiTween::UMGRTRotateUpdate(widgetTweening, loc, orientationSpeed, deltaSeconds);*/
		//UiTween::Print("dummy code, disregard");
	}
}

void AiTweenEvent::TickActorMoveFromTo()
{
	successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorLocation(), vectorConstraints), sweepTransform, &sweepResult);

	if (sweepTransform)
	{
		if (sweepResult.bBlockingHit)
		{
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickActorRotateFromTo()
{
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		successfulTransform = actorTweening->SetActorRotation(UiTween::ConstrainRotator(rot.Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		successfulTransform = actorTweening->SetActorRotation(UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickComponentMoveFromTo()
{
	componentTweening->SetWorldLocation(UiTween::ConstrainVector(FMath::Lerp<FVector>(vectorFrom, vectorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentLocation(), vectorConstraints), sweepTransform, &sweepResult);

	if (sweepTransform)
	{
		if (sweepResult.bBlockingHit)
		{
			successfulTransform = false;
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickComponentRotateFromTo()
{
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(rot.Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickUMGRTMoveFromTo()
{
	FVector v = FMath::Lerp<FVector>(FVector(vector2DFrom.X, vector2DFrom.Y, 0), FVector(vector2DTo.X, vector2DTo.Y, 0), GetAlphaFromEquation(alpha));
	widgetTweening->SetRenderTranslation(UiTween::ConstrainVector2D(FVector2D(v.X, v.Y), widgetTweening->RenderTransform.Translation, vector2DConstraints));

	OrientationSwitch();
}

void AiTweenEvent::TickRotatorFromTo()
{
	dtv.rotatorCurrent = rotatorFrom;
	if (shortestPath)
	{
		FQuat rot = FQuat::Slerp(rotatorFrom.Quaternion(), rotatorTo.Quaternion(), GetAlphaFromEquation(alpha));

		rot.Normalize();

		dtv.rotatorCurrent = UiTween::ConstrainRotator(rot.Rotator(), dtv.rotatorCurrent, rotatorConstraints);
	}
	else
	{
		dtv.rotatorCurrent = UiTween::ConstrainRotator(FMath::LerpStable<FRotator>(rotatorFrom, rotatorTo, GetAlphaFromEquation(alpha)), dtv.rotatorCurrent, rotatorConstraints);
	}
}

void AiTweenEvent::TickActorMoveToSplinePoint()
{
	if (playingBackward)
	{
		successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration)), actorTweening->GetActorLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
	else
	{
		successfulTransform = actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), actorTweening->GetActorLocation(), vectorConstraints));
	}

	if (sweepTransform)
	{
		if (sweepResult.bBlockingHit)
		{
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickComponentMoveToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration)), componentTweening->GetComponentLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
	else
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration), componentTweening->GetComponentLocation(), vectorConstraints));
	}

	if (sweepTransform)
	{
		if (sweepResult.bBlockingHit)
		{
			successfulTransform = false;
			EndPhase();
		}
	}

	OrientationSwitch();
}

void AiTweenEvent::TickActorRotateToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration))) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration)) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::TickComponentRotateToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((GetAlphaFromEquation(alpha) * splineComponent->Duration) - splineComponent->Duration))) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(GetAlphaFromEquation(alpha) * splineComponent->Duration)) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::EndActorMoveToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration)), actorTweening->GetActorLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
	else
	{
		actorTweening->SetActorLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration), actorTweening->GetActorLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
}

void AiTweenEvent::EndComponentMoveToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration)), componentTweening->GetComponentLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
	else
	{
		componentTweening->SetWorldLocation(UiTween::ConstrainVector(splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration), componentTweening->GetComponentLocation(), vectorConstraints), sweepTransform, &sweepResult);
	}
}

void AiTweenEvent::EndActorRotateToSplinePoint()
{
	if (playingBackward)
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration))) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
	else
	{
		actorTweening->SetActorRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration)) - actorTweening->GetActorLocation()).Rotator(), actorTweening->GetActorRotation(), rotatorConstraints));
	}
}

void AiTweenEvent::EndComponentRotateToSplinePoint()
{
	if (playingBackward)
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(FMath::Abs((1.f * splineComponent->Duration) - splineComponent->Duration))) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
	else
	{
		componentTweening->SetWorldRotation(UiTween::ConstrainRotator(FRotationMatrix::MakeFromX((splineComponent->GetWorldLocationAtTime(1.f * splineComponent->Duration)) - componentTweening->GetComponentLocation()).Rotator(), componentTweening->GetComponentRotation(), rotatorConstraints));
	}
}
