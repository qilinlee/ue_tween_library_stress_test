#pragma once

#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "QTweenEasing.generated.h"

UENUM()
enum class EQTweenEasingType : uint8
{
    EasingNone = 0,
    EasingIn = 1,
    EasingOut = 2,
    EasingInOut = 3,
};

UENUM()
enum class EQTweenEasingFunc : uint8
{
    EasingUnknown = 0,
    EasingLinear = 0,
    EasingQuad = 1,
    EasingCubic = 2,
    EasingQuart = 3,
    EasingQuint = 4,
    EasingSin = 5,
    EasingExo = 6,
    EasingCirc = 7,
    EasingBounce = 8,
    EasingElastic = 9,
    EasingBack = 10,
    EasingSpring = 11,
    Max
};

class QTWEEN_API IQTweenEasing
{
public:
    virtual ~IQTweenEasing() = default;
    virtual float EaseIn(float Factor, float Start, float End) = 0;
    virtual float EaseOut(float Factor, float Start, float End) = 0;
    virtual float EaseInOut(float Factor, float Start, float End) = 0;
};

namespace QTween
{
    namespace Easing
    {

        class FLinearEasing : public IQTweenEasing
        {
        public:
            static FLinearEasing Linear;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return Evaluate(Factor, Start, End);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return Evaluate(Factor, Start, End);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                return Evaluate(Factor, Start, End);
            }

        private:
            static float Evaluate(float Factor, float Start, float End)
            {
                return (End - Start) * Factor + Start;
            }
        };

        class FQuadraticEasing : public IQTweenEasing
        {
        public:
            static FQuadraticEasing Quad;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return (End - Start) * Factor * Factor + Start;
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return (-(End - Start)) * Factor * (Factor - 2) + Start;
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1)
                {
                    return (((End - Start) / 2) * Factor * Factor + Start);
                }

                Factor -= 1;
                return ((-(End - Start) / 2) * (Factor * (Factor - 2) - 1) + Start);
            }
        };

        class FCubicInEasing : public IQTweenEasing
        {
        public:
            static FCubicInEasing Cubic;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return ((End - Start) * Factor * Factor * Factor + Start);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return ((End - Start) * (Factor * Factor * Factor + 1) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1) {
                    return (((End - Start) / 2) * Factor * Factor * Factor + Start);
                }
                Factor -= 2;
                return (((End - Start) / 2) * (Factor * Factor * Factor + 2) + Start);
            }
        };

        class FQuarticEasing : public IQTweenEasing
        {
        public:
            static FQuarticEasing Quart;

            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return ((End - Start) * Factor * Factor * Factor * Factor + Start);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                Factor -= 1;
                return (-(End - Start) * (Factor * Factor * Factor * Factor - 1) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1) {
                    return (((End - Start) / 2) * (Factor * Factor * Factor * Factor) + Start);
                }
                Factor -= 2;
                return ((-(End - Start) / 2) * (Factor * Factor * Factor * Factor - 2) + Start);
            }
        };

        class FQuinticEasing : public IQTweenEasing
        {
        public:
            static FQuinticEasing Quintic;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return ((End - Start) * Factor * Factor * Factor * Factor * Factor + Start);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                Factor--;
                return ((End - Start) * (Factor * Factor * Factor * Factor * Factor + 1) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1)
                {
                    return (((End - Start) / 2) * (Factor * Factor * Factor * Factor * Factor) + Start);
                }

                Factor -= 2;
                return (((End - Start) / 2) * (Factor * Factor * Factor * Factor * Factor + 2) + Start);
            }
        };

        class FSinEasing : public IQTweenEasing
        {
        public:
            static FSinEasing Sin;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return (-(End - Start) * FMath::Cos(Factor * (PI) / 2) + (End - Start) + Start);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return ((End - Start) * FMath::Sin(Factor * (PI) / 2) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                return ((-(End - Start) / 2) * (FMath::Cos(Factor * (PI)) - 1) + Start);
            }
        };

        class FExponentialEasing : public IQTweenEasing
        {
        public:
            static FExponentialEasing Exponential;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return ((End - Start) * FMath::Pow(2, 10 * (Factor - 1)) + Start);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return ((End - Start) * (-FMath::Pow(2, -10 * Factor) + 1) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1) {
                    return (((End - Start) / 2) * FMath::Pow(2, 10 * (Factor - 1)) + Start);
                }
                --Factor;
                return (((End - Start) / 2) * (-FMath::Pow(2, -10 * Factor) + 2) + Start);
            }
        };

        class FCircularInEasing : public IQTweenEasing
        {
        public:
            static FCircularInEasing Circular;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return (-(End - Start) * (FMath::Sqrt(1 - Factor * Factor) - 1) + Start);
            }
            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                return ((End - Start) * (FMath::Sqrt(1 - Factor * Factor)) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                Factor *= 2;
                if (Factor < 1) {
                    return ((-(End - Start) / 2) * (FMath::Sqrt(1 - Factor * Factor) - 1) + Start);
                }

                Factor -= 2;
                return (((End - Start) / 2) * (FMath::Sqrt(1 - Factor * Factor) + 1) + Start);
            }
        };

        class FBounceEasing : public IQTweenEasing
        {
        public:
            static FBounceEasing Bounce;
            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                return (End - Start) - EaseOut((1 - Factor), 0.f, End) + Start;;
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                float c = End - Start;
                if (Factor < (1 / 2.75f))
                {
                    return (c * (7.5625f * Factor * Factor) + Start);
                }
                else if (Factor < (2.0f / 2.75f))
                {
                    float postFix = Factor -= (1.5f / 2.75f);
                    return (c * (7.5625f * (postFix)*Factor + .75f) + Start);
                }
                else if (Factor < (2.5f / 2.75f))
                {
                    float postFix = Factor -= (2.25f / 2.75f);
                    return (c * (7.5625f * (postFix)*Factor + .9375f) + Start);
                }
                else
                {
                    float postFix = Factor -= (2.625f / 2.75f);
                    return (c * (7.5625f * (postFix)*Factor + .984375f) + Start);
                }
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                if (Factor < 0.5f)
                    return (EaseIn(Factor * 2, 0.f, End) * .5f + Start);
                else
                    return (EaseOut((Factor * 2 - 1), 0.f, End) * .5f + (End - Start) * .5f + Start);
            }
        };

        class FElasticEasing : public IQTweenEasing
        {
        public:
            static FElasticEasing Elastic;

            FElasticEasing() : overshoot(1.f), period(0.3f) {}

            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                if (Factor <= 0.0001f)
                    return Start;

                if (Factor >= 0.9999f)
                    return End;

                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = End - Start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = End - Start;
                    s = p / 4.f;
                }
                else
                {
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));
                }

                float over = overshoot;
                if (overshoot > 1.f && Factor > 0.6f)
                    over = 1.f + (1.f - Factor) / 0.4f * (overshoot - 1.f);

                Factor -= 1;

                float postFix = a * FMath::Pow(2, 10 * Factor);
                float sine = FMath::Sin((Factor - s) * (2 * (PI)) / p);
                return (Start - (postFix * sine) * over);
            }

            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                if (Factor <= 0.0001f)
                    return Start;

                if (Factor >= 0.9999f)
                    return End;

                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = End - Start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = End - Start;
                    s = p / 4.f;
                }
                else
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));

                float over = overshoot;
                if (overshoot > 1.f && Factor < 0.4f)
                    over = 1.f + (Factor / 0.4f * (overshoot - 1.f));

                float postFix = a * FMath::Pow(2, -10 * Factor);
                float sine = FMath::Sin((Factor - s) * (2 * (PI)) / p);

                return (postFix * sine) * over + End;
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                if (Factor <= 0.0001f)
                    return Start;

                if (Factor >= 0.9999f)
                    return End;

                Factor *= 2;
                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = End - Start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = End - Start;
                    s = p / 4.f;
                }
                else
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));

                float over = overshoot;
                if (overshoot > 1.f)
                {
                    if (Factor < 0.2f)
                        over = 1.f + (Factor / 0.2f * (overshoot - 1.f));
                    else if (Factor > 0.8f)
                        over = 1.f + ((1 - Factor) / 0.2f * (overshoot - 1.f));
                }


                Factor -= 1.f;
                if (Factor < 1)
                {
                    float postFix = a * FMath::Pow(2, 10 * Factor);
                    float sine = FMath::Sin((Factor - s) * (2 * (PI)) / p);
                    return (Start - 0.5f * (postFix * sine) * over);
                }

                float postFix = a * FMath::Pow(2, -10 * Factor);
                float sine = FMath::Sin((Factor - s) * (2 * (PI)) / p);
                return (postFix * sine * 0.5f * over + End);
            }
        private:
            float overshoot;
            float period;
        };

        class FBackEasing : public IQTweenEasing
        {
        public:
            static FBackEasing Back;

            FBackEasing() :Overshoot(1.f) {}

            virtual float EaseIn(float Factor, float Start, float End) override final
            {
                float s = 1.70158f * Overshoot;
                float postFix = Factor;
                return ((End - Start) * (postFix)*Factor * ((s + 1) * Factor - s) + Start);
            }
            virtual float EaseOut(float Factor, float Start, float End) override final
            {
                float s = 1.70158f * Overshoot;
                Factor -= 1;
                return ((End - Start) * (Factor * Factor * ((s + 1) * Factor + s) + 1) + Start);
            }

            virtual float EaseInOut(float Factor, float Start, float End) override final
            {
                float s = 1.70158f * Overshoot;
                float t = Factor;
                float b = Start;
                float c = End - Start;
                float d = 1;
                s *= (1.525f * Overshoot);
                if ((t /= d / 2) < 1)
                    return (c / 2 * (t * t * (((s)+1) * t - s)) + b);
                float postFix = t -= 2;
                return (c / 2 * ((postFix)*t * (((s)+1) * t + s) + 2) + b);
            }
        private:
            float Overshoot;
        };

        class FSpringEasing : public IQTweenEasing
        {
        public:
            static FSpringEasing Spring;
            virtual float EaseIn(float Factor, float Start, float End)override final
            {
                return Evaluate(Factor, Start, End);
            }

            virtual float EaseOut(float Factor, float Start, float End)override final
            {
                return Evaluate(Factor, Start, End);
            }

            virtual float EaseInOut(float Factor, float Start, float End)override final
            {
                return Evaluate(Factor, Start, End);
            }
        private:
            static float Evaluate(float Factor, float Start, float End)
            {
                float t = (FMath::Sin(Factor * PI * (0.2f + 2.5f * Factor * Factor * Factor))
                    * FMath::Pow(1.f - Factor, 2.2f) + Factor) * (1.f + (1.2f * (1.f - Factor)));
                return Start + (End - Start) * t;
            }

        };
    }
}