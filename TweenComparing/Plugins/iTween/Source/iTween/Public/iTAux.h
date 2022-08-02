//Copyright Jared Therriault and Bob Berkbile 2014

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "iTAux.generated.h"

/**
*
*/
UENUM(BlueprintType)
namespace EEventType
{
	enum ItweenEventType
	{
		actorMoveFromTo,
		actorMoveToSplinePoint,
		actorRotateFromTo,
		actorRotateToSplinePoint,
		actorScaleFromTo,
		compMoveFromTo,
		compMoveToSplinePoint,
		compRotateFromTo,
		compRotateToSplinePoint,
		compScaleFromTo,
		umgRTMoveFromTo,
		umgRTRotateFromTo,
		umgRTScaleFromTo,
		umgRTShearFromTo,
		floatFromTo,
		linearColorFromTo,
		vectorFromTo,
		vector2DFromTo,
		rotatorFromTo,
		slateMoveFromTo,
		slateScaleFromTo
	};
}
UENUM(BlueprintType)
namespace EEaseType
{
	enum ItweenEaseType
	{
		linear UMETA(DisplayName = "Linear (No Easing)"),
		easeInQuadratic,
		easeOutQuadratic,
		easeInAndOutQuadratic,
		easeInCubic,
		easeOutCubic,
		easeInAndOutCubic,
		easeInQuartic,
		easeOutQuartic,
		easeInAndOutQuartic,
		easeInQuintic,
		easeOutQuintic,
		easeInAndOutQuintic,
		easeInSine,
		easeOutSine,
		easeInAndOutSine,
		easeInExponential,
		easeOutExponential,
		easeInAndOutExponential,
		easeInCircular,
		easeOutCircular,
		easeInAndOutCircular,
		easeInBounce UMETA(DisplayName = "Ease In Bounce (Jump Up)"),
		easeOutBounce,
		easeInAndOutBounce UMETA(DisplayName = "Ease In and Out Bounce (Jump Up and Gravity Switch To Bounce)"),
		easeInBack UMETA(DisplayName = "Ease In Back (Anticipation/ Wind Up)"),
		easeOutBack UMETA(DisplayName = "Ease Out Back (Ease Back After Going Too Far)"),
		easeInAndOutBack UMETA(DisplayName = "Ease In and Out Back (Wind Up and Overshoot Then Back)"),
		easeInElastic,
		easeOutElastic,
		easeInAndOutElastic,
		spring UMETA(DisplayName = "Spring (Soft Rubber/ Gelatin To Destination)"),
		punch UMETA(DisplayName = "Punch (Snap Rubber In Place In Direction Of Destination)"),
		customCurve
	};
}
UENUM(BlueprintType)
namespace EVectorConstraints
{
	enum ItweenVectorConstraints
	{
		none UMETA(DisplayName = "No Constraints (XYZ)"),
		xOnly,
		yOnly,
		zOnly,
		xyOnly UMETA(DisplayName = "XY Only"),
		yzOnly UMETA(DisplayName = "YZ Only"),
		xzOnly UMETA(DisplayName = "XZ Only")
	};
}
UENUM(BlueprintType)
namespace EVector2DConstraints
{
	enum ItweenVector2DConstraints
	{
		none UMETA(DisplayName = "No Constraints (XY)"),
		xOnly,
		yOnly
	};
}
UENUM(BlueprintType)
namespace ERotatorConstraints
{
	enum ItweenRotatorConstraints
	{
		none UMETA(DisplayName = "No Constraints (Pitch, Yaw, and Roll)"),
		pitchOnly,
		yawOnly,
		rollOnly,
		pitchYawOnly UMETA(DisplayName = "Pitch and Yaw Only"),
		yawRollOnly UMETA(DisplayName = "Yaw and Roll Only"),
		pitchRollOnly UMETA(DisplayName = "Pitch and Roll Only")
	};
}
UENUM(BlueprintType)
namespace EDelayType
{
	enum ItweenDelayType
	{
		first UMETA(DisplayName = "First Delay Only"),
		firstLoop UMETA(DisplayName = "First Delay And After Each Loop Section"),
		loop UMETA(DisplayName = "After Each Loop Section Only"),
		firstLoopFull UMETA(DisplayName = "First Delay And After Each Full Loop"),
		loopFull UMETA(DisplayName = "After Each Full Loop Only")
	};
}
UENUM(BlueprintType)
namespace ETickType
{
	enum ItweenTickType
	{
		seconds,
		speed
	};
}
UENUM(BlueprintType)
namespace ELoopType
{
	enum ItweenLoopType
	{
		once UMETA(DisplayName = "Play Once"),
		rewind,
		pingPong UMETA(DisplayName = "Back-and-Forth (Ping-Pong)")
	};
}
UENUM(BlueprintType)
namespace ECoordinateSpace
{
	enum ItweenCoordinateSpace
	{
		world,
		self,
		parent
	};
}
UENUM(BlueprintType)
namespace ELookType
{
	enum ItweenLookType
	{
		noOrientationChange,
		orientToTarget,
		orientToPath
	};
}

UENUM(BlueprintType)
namespace ETweenInterfaceType
{
	enum ItweenInterfaceType
	{
		start,
		update,
		loop,
		complete
	};
}

USTRUCT(BlueprintType)
struct FDataTypeValues
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Vector Value"), Category = "Vector Properties")
	FVector vectorCurrent = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Rotator Value"), Category = "Rotator Properties")
		FRotator rotatorCurrent = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Vector2D Value"), Category = "Vector2D Properties")
		FVector2D vector2DCurrent = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Float Value"), Category = "Float Properties")
		float floatCurrent = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Linear Color Value"), Category = "Linear Color Properties")
		FLinearColor linearColorCurrent = FLinearColor::Black;
};

class AiTweenEvent;
UCLASS()
class AiTAux : public AActor
{
	GENERATED_BODY()

public:
	//Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool performDebugOperations = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool printDebugMessages = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool printErrorMessages = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = iTween)
		bool tickWhenPaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = iTween)
		bool ignoreTimeDilation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = iTween)
		float defaultTimerInterval = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = iTween)
		TArray<AiTweenEvent*> currentTweens;
};

