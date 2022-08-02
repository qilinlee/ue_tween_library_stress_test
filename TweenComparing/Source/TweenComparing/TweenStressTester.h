#pragma once
#include "BUITween.h"
#include "FCTween.h"
#include "iTween.h"
#include "iTweenEvent.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"

#include "TweenStressTester.generated.h"

UENUM()
enum class ETweenPlugin : uint8
{
	FCTween,
	BUITween,
	ITween,
};

UCLASS()
class ATweenStressTester : public AActor
{
	GENERATED_BODY()

public:
	// tweens to spawn on startup
	UPROPERTY(EditAnywhere)
	float NumTweensStartup = 40000;

	// tweens to spawn each frame
	UPROPERTY(EditAnywhere)
	float NumTweensPerFrame = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TestWidgetTemplate;

	UPROPERTY(EditAnywhere)
	ETweenPlugin PluginToTest =ETweenPlugin::FCTween;
	
	UPROPERTY()
	UUserWidget* TestWidgetInstance;
	
	UPROPERTY()
	float Counter;

	UPROPERTY()
	bool bIsRunningStressTest;
	
	ATweenStressTester()
	{
		PrimaryActorTick.bCanEverTick = true;
	}

	virtual void BeginPlay() override
	{
		Super::BeginPlay();

		Counter = 0;

		TestWidgetInstance = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), TestWidgetTemplate);
		TestWidgetInstance->AddToViewport();
		bIsRunningStressTest = false;
		

		FCTween::ClearActiveTweens();
		FCTween::Deinitialize();
		FCTween::Initialize();
		if(PluginToTest == ETweenPlugin::FCTween)
		{
			FCTween::EnsureCapacity(50, 50, NumTweensStartup + NumTweensPerFrame * 60 * 4, 10);
		}
	}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
	{
		Super::EndPlay(EndPlayReason);
		
		FCTween::ClearActiveTweens();
		FCTween::Deinitialize();
		FCTween::Initialize();
	}

	virtual void Tick(float DeltaSeconds) override
	{
		Super::Tick(DeltaSeconds);

		if(bIsRunningStressTest)
		{
			extern ENGINE_API float GAverageFPS;
			UE_LOG(LogTemp,Warning,TEXT("FPS: %f"), GAverageFPS)

			for(int i = 0; i < NumTweensPerFrame; ++i)
			{
				CreateTween();
			}
		}
		else
		{
			Counter += DeltaSeconds;
			if(Counter > 2.0f)
			{
				//wait till game has started up
				RunStressTest();
				bIsRunningStressTest = true;
				Counter = 0;
			}
		}

	}

	void RunStressTest()
	{
		double TimeStart = FPlatformTime::Seconds();
		
		for(int i = 0; i < NumTweensStartup; ++i)
		{
			CreateTween();
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Initialize Milliseconds: %f"), (FPlatformTime::Seconds() - TimeStart) * 1000.0);
		
	}

	void CreateTween()
	{
		FVector2d TweenFromLocation = FVector2d::ZeroVector;
		FVector2d TweenToLocation = FVector2d(200.0f, 200.0f);

		switch(PluginToTest)
		{
			case ETweenPlugin::FCTween:
				FCTween::Play(TweenFromLocation, TweenToLocation, [&](FVector2d t){
					TestWidgetInstance->SetRenderTranslation(t);
				}, 2.0f, EFCEase::OutQuad);
				break;
			case ETweenPlugin::BUITween:
				UBUITween::Create(TestWidgetInstance, 2.0f)
					.FromTranslation(TweenFromLocation)
					.ToTranslation(TweenToLocation)
					.Easing(EBUIEasingType::OutQuad)
					.Begin();
				break;
			case ETweenPlugin::ITween:
				UiTween::UMGRTMoveFromToSimple("", TestWidgetInstance, TweenFromLocation, TweenToLocation, 2.0f, ItweenEaseType::easeOutQuadratic);
				break;
		}
	}
};
