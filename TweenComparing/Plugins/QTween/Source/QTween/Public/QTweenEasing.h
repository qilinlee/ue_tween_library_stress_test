#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
    virtual float EaseIn(float fractor, float start, float end) = 0;
    virtual float EaseOut(float fractor, float start, float end) = 0;
    virtual float EaseInOut(float fractor, float start, float end) = 0;
};

namespace QTween
{
    namespace Easing
    {

        class FLinearEasing : public IQTweenEasing
        {
        public:
            static FLinearEasing Linear;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return Evaluate(fractor, start, end);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                return Evaluate(fractor, start, end);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                return Evaluate(fractor, start, end);
            }

        private:
            float Evaluate(float fractor, float start, float end)
            {
                return (end - start) * fractor + start;
            }
        };

        class FQuadraticEasing : public IQTweenEasing
        {
        public:
            static FQuadraticEasing Quard;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return (end - start) * fractor * fractor + start;
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                return (-(end - start)) * fractor * (fractor - 2) + start;
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1)
                {
                    return (((end - start) / 2) * fractor * fractor + start);
                }

                fractor -= 1;
                return ((-(end - start) / 2) * (fractor * (fractor - 2) - 1) + start);
            }
        };

        class FCubicInEasing : public IQTweenEasing
        {
        public:
            static FCubicInEasing Cubic;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return ((end - start) * fractor * fractor * fractor + start);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                return ((end - start) * (fractor * fractor * fractor + 1) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1) {
                    return (((end - start) / 2) * fractor * fractor * fractor + start);
                }
                fractor -= 2;
                return (((end - start) / 2) * (fractor * fractor * fractor + 2) + start);
            }
        };

        class FQuarticEasing : public IQTweenEasing
        {
        public:
            static FQuarticEasing Quart;

            virtual float EaseIn(float fractor, float start, float end) final
            {
                return ((end - start) * fractor * fractor * fractor * fractor + start);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                fractor -= 1;
                return (-(end - start) * (fractor * fractor * fractor * fractor - 1) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1) {
                    return (((end - start) / 2) * (fractor * fractor * fractor * fractor) + start);
                }
                fractor -= 2;
                return ((-(end - start) / 2) * (fractor * fractor * fractor * fractor - 2) + start);
            }
        };

        class FQuinticEasing : public IQTweenEasing
        {
        public:
            static FQuinticEasing Quintic;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return ((end - start) * fractor * fractor * fractor * fractor * fractor + start);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                fractor--;
                return ((end - start) * (fractor * fractor * fractor * fractor * fractor + 1) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1)
                {
                    return (((end - start) / 2) * (fractor * fractor * fractor * fractor * fractor) + start);
                }

                fractor -= 2;
                return (((end - start) / 2) * (fractor * fractor * fractor * fractor * fractor + 2) + start);
            }
        };

        class FSinEasing : public IQTweenEasing
        {
        public:
            static FSinEasing Sin;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return (-(end - start) * FMath::Cos(fractor * (PI) / 2) + (end - start) + start);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                return ((end - start) * FMath::Sin(fractor * (PI) / 2) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                return ((-(end - start) / 2) * (FMath::Cos(fractor * (PI)) - 1) + start);
            }
        };

        class FExponentialEasing : public IQTweenEasing
        {
        public:
            static FExponentialEasing Exponential;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return ((end - start) * FMath::Pow(2, 10 * (fractor - 1)) + start);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                return ((end - start) * (-FMath::Pow(2, -10 * fractor) + 1) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1) {
                    return (((end - start) / 2) * FMath::Pow(2, 10 * (fractor - 1)) + start);
                }
                --fractor;
                return (((end - start) / 2) * (-FMath::Pow(2, -10 * fractor) + 2) + start);
            }
        };

        class FCircularInEasing : public IQTweenEasing
        {
        public:
            static FCircularInEasing Circluar;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return (-(end - start) * (FMath::Sqrt(1 - fractor * fractor) - 1) + start);
            }
            virtual float EaseOut(float fractor, float start, float end) final
            {
                return ((end - start) * (FMath::Sqrt(1 - fractor * fractor)) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                fractor *= 2;
                if (fractor < 1) {
                    return ((-(end - start) / 2) * (FMath::Sqrt(1 - fractor * fractor) - 1) + start);
                }

                fractor -= 2;
                return (((end - start) / 2) * (FMath::Sqrt(1 - fractor * fractor) + 1) + start);
            }
        };

        class FBounceEasing : public IQTweenEasing
        {
        public:
            static FBounceEasing Bounce;
            virtual float EaseIn(float fractor, float start, float end) final
            {
                return (end - start) - EaseOut((1 - fractor), 0.f, end) + start;;
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                float c = end - start;
                if (fractor < (1 / 2.75f))
                {
                    return (c * (7.5625f * fractor * fractor) + start);
                }
                else if (fractor < (2.0f / 2.75f))
                {
                    float postFix = fractor -= (1.5f / 2.75f);
                    return (c * (7.5625f * (postFix)*fractor + .75f) + start);
                }
                else if (fractor < (2.5f / 2.75f))
                {
                    float postFix = fractor -= (2.25f / 2.75f);
                    return (c * (7.5625f * (postFix)*fractor + .9375f) + start);
                }
                else
                {
                    float postFix = fractor -= (2.625f / 2.75f);
                    return (c * (7.5625f * (postFix)*fractor + .984375f) + start);
                }
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                if (fractor < 0.5f)
                    return (EaseIn(fractor * 2, 0.f, end) * .5f + start);
                else
                    return (EaseOut((fractor * 2 - 1), 0.f, end) * .5f + (end - start) * .5f + start);
            }
        };

        class FElasticEasing : public IQTweenEasing
        {
        public:
            static FElasticEasing Elastic;

            FElasticEasing() : overshoot(1.f), period(0.3f) {}

            virtual float EaseIn(float fractor, float start, float end) final
            {
                if (fractor <= 0.0001f)
                    return start;

                if (fractor >= 0.9999f)
                    return end;

                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = end - start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = end - start;
                    s = p / 4.f;
                }
                else
                {
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));
                }

                float over = overshoot;
                if (overshoot > 1.f && fractor > 0.6f)
                    over = 1.f + (1.f - fractor) / 0.4f * (overshoot - 1.f);

                fractor -= 1;

                float postFix = a * FMath::Pow(2, 10 * fractor);
                float sine = FMath::Sin((fractor - s) * (2 * (PI)) / p);
                return (start - (postFix * sine) * over);
            }

            virtual float EaseOut(float fractor, float start, float end) final
            {
                if (fractor <= 0.0001f)
                    return start;

                if (fractor >= 0.9999f)
                    return end;

                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = end - start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = end - start;
                    s = p / 4.f;
                }
                else
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));

                float over = overshoot;
                if (overshoot > 1.f && fractor < 0.4f)
                    over = 1.f + (fractor / 0.4f * (overshoot - 1.f));

                float postFix = a * FMath::Pow(2, -10 * fractor);
                float sine = FMath::Sin((fractor - s) * (2 * (PI)) / p);

                return (postFix * sine) * over + end;
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                if (fractor <= 0.0001f)
                    return start;

                if (fractor >= 0.9999f)
                    return end;

                fractor *= 2;
                float p = period;
                float a = 0.f;
                float s = 0.f;
                float d = end - start;

                if (a == 0.f || a < FMath::Abs(d))
                {
                    a = end - start;
                    s = p / 4.f;
                }
                else
                    s = p / (2.f * (PI)*FMath::FastAsin(d / a));

                float over = overshoot;
                if (overshoot > 1.f)
                {
                    if (fractor < 0.2f)
                        over = 1.f + (fractor / 0.2f * (overshoot - 1.f));
                    else if (fractor > 0.8f)
                        over = 1.f + ((1 - fractor) / 0.2f * (overshoot - 1.f));
                }


                fractor -= 1.f;
                if (fractor < 1)
                {
                    float postFix = a * FMath::Pow(2, 10 * fractor);
                    float sine = FMath::Sin((fractor - s) * (2 * (PI)) / p);
                    return (start - 0.5f * (postFix * sine) * over);
                }

                float postFix = a * FMath::Pow(2, -10 * fractor);
                float sine = FMath::Sin((fractor - s) * (2 * (PI)) / p);
                return (postFix * sine * 0.5f * over + end);
            }
        private:
            float overshoot;
            float period;
        };

        class FBackEasing : public IQTweenEasing
        {
        public:
            static FBackEasing Back;

            FBackEasing() :overshoot(1.f) {}

            virtual float EaseIn(float fractor, float start, float end) final
            {
                float s = 1.70158f * overshoot;
                float postFix = fractor;
                return ((end - start) * (postFix)*fractor * ((s + 1) * fractor - s) + start);
            }
            virtual float EaseOut(float fractor, float start, float end) final
            {
                float s = 1.70158f * overshoot;
                fractor -= 1;
                return ((end - start) * (fractor * fractor * ((s + 1) * fractor + s) + 1) + start);
            }

            virtual float EaseInOut(float fractor, float start, float end) final
            {
                float s = 1.70158f * overshoot;
                float t = fractor;
                float b = start;
                float c = end - start;
                float d = 1;
                s *= (1.525f * overshoot);
                if ((t /= d / 2) < 1)
                    return (c / 2 * (t * t * (((s)+1) * t - s)) + b);
                float postFix = t -= 2;
                return (c / 2 * ((postFix)*t * (((s)+1) * t + s) + 2) + b);
            }
        private:
            float overshoot;
        };

        class FSpringEasing : public IQTweenEasing
        {
        public:
            static FSpringEasing Spring;
            virtual float EaseIn(float fractor, float start, float end)override
            {
                return Evaluate(fractor, start, end);
            }

            virtual float EaseOut(float fractor, float start, float end)override
            {
                return Evaluate(fractor, start, end);
            }

            virtual float EaseInOut(float fractor, float start, float end)override
            {
                return Evaluate(fractor, start, end);
            }
        private:
            float Evaluate(float fractor, float start, float end)
            {
                float t = (FMath::Sin(fractor * PI * (0.2f + 2.5f * fractor * fractor * fractor)) * FMath::Pow(1.f - fractor, 2.2f) + fractor) * (1.f + (1.2f * (1.f - fractor)));
                return start + (end - start) * t;
            }

        };
    }
}