#ifndef LightManager_h
#define LightManager_h

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include "../Util/Nullable.cpp"

using namespace std;

class LightManager
{

private:
    LightManager() = default;

    const static uint16_t _ledCountsOneSide = 20;
    const static uint16_t _ledCounts = _ledCountsOneSide * 2;
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *lights;
    NeoPixelAnimator *animator;
    RgbColor _lastColor[_ledCounts];

    vector<RgbColor> _randomColors;

    void generateRandomColors()
    {
        if (_randomColors.size() > 0)
        {
            return;
        }

        byte r = 255;
        byte g = 0;
        byte b = 0;
        byte step = 15;

        // g up
        for (int i = 0; i < 255; i += step)
        {
            g = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }
        

        // r down
        for (int i = 255 - step; i >= 0; i -= step)
        {
            r = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }

        // b up
        for (int i = 0 + step; i < 255; i += step)
        {
            b = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }

        // g down
        for (int i = 255 - step; i >= 0; i -= step)
        {
            g = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }

        // r up
        for (int i = 0 + step; i < 255; i += step)
        {
            r = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }

        // b down
        for (int i = 255 - step; i >= 0; i -= step)
        {
            b = (byte)i;
            _randomColors.push_back(RgbColor(r, g, b));
        }
    }

    
    

public:
    static LightManager *getInstance()
    {
        static LightManager instance;
        return &instance;
    }

    void init()
    {
        Serial.println("Init LightManager");
        generateRandomColors();

        lights = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(_ledCounts, 2);
        lights->Begin();
        lights->Show();

        setAllLightsToColor(RgbColor(255, 10, 0));

        animator = new NeoPixelAnimator(_ledCounts, NEO_MILLISECONDS);
    }

    void stopAllAnimations()
    {
        animator->StopAll();
    }

    void setAllLightsToColor(RgbColor color)
    {
        for (size_t i = 0; i < _ledCounts; i++)
        {
            setPixelColor(i, color);
        }
        lights->Show();
    }

    static float PulseEase(float unitValue)
    {
        float returnValue = (sin(unitValue * PI));
        Serial.println((String) "U: " + unitValue + " V: " + returnValue);
        return returnValue;
    }

    static float PulseLinearEase(float unitValue)
    {
        if (unitValue < 0.5)
        {
            return unitValue * 2;
        }
        else
        {
            return 2 - (unitValue * 2);
        }
    }

    void setPixelColor(int pin, RgbColor &color)
    {
        lights->SetPixelColor(pin, color);
        _lastColor[pin] = color;

        if (pin < _ledCountsOneSide)
        {
            int nextPin = pin + 20;
            lights->SetPixelColor(nextPin, color);
            // Serial.println((String)"Pin: " + pin + " Next: " + nextPin);
            _lastColor[nextPin] = color;
        }
    }

    RgbColor getPixelColor(int pin)
    {
        return _lastColor[pin];
    }


    String rgbValuesToHex(vector<byte> colors)
    {
        std::ostringstream ss;
        ss << "#" << std::hex << std::setfill('0')  << std::uppercase;
        for(auto const& value: colors) {
            ss << std::setw(2) << static_cast<int>(value);
        }
        return String(ss.str().c_str());
    }

    String rgbColorToHex(RgbColor color)
    {
        return rgbValuesToHex({ color.R, color.G, color.B });
    }

    RgbColor getRandomColor()
    {
        int index = random(_randomColors.size());
        return _randomColors[index];
    }

    void repeatPulseAnimation(uint16_t pixelIndex, RgbColor endColor = RgbColor(0), uint16_t animationTime = 200, AnimEaseFunction easeFunction = PulseLinearEase)
    {
        const uint8_t peak = 255;
        RgbColor originalColor = RgbColor(0);
        RgbColor targetColor = endColor;
        if (targetColor == RgbColor(0))
        {
            targetColor = RgbColor(random(peak), random(peak), random(peak));
        }
        RgbColor paramEndColor = targetColor;
        targetColor = targetColor;

        AnimUpdateCallback animUpdate = [=](const AnimationParam &param)
        {
            float progress = easeFunction(param.progress);

            RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
            setPixelColor(param.index, updatedColor);

            if (param.state == AnimationState_Completed)
            {
                repeatPulseAnimation(pixelIndex, paramEndColor, animationTime, easeFunction);
            }
        };

        animator->StartAnimation(pixelIndex, animationTime, animUpdate);
    }

    void toggleRepeatFadeAnimation(uint16_t pixelIndex, RgbColor startColor = RgbColor(0), RgbColor endColor = RgbColor(255), uint16_t animationTime = 200, AnimEaseFunction easeFunction = PulseLinearEase)
    {
        RgbColor paramStartColor = startColor;
        RgbColor paramEndColor = endColor;
        AnimUpdateCallback animUpdate = [=](const AnimationParam &param)
        {
            float progress = easeFunction(param.progress);

            RgbColor updatedColor = RgbColor::LinearBlend(startColor, endColor, progress);
            setPixelColor(param.index, updatedColor);

            if (param.state == AnimationState_Completed)
            {
                toggleRepeatFadeAnimation(pixelIndex, paramStartColor, paramEndColor, animationTime, easeFunction);
            }
        };

        animator->StartAnimation(pixelIndex, animationTime, animUpdate);
    }

    void startSingleFadeAnimation(vector<uint16_t> pixelIndex, RgbColor startColor = RgbColor(0), RgbColor endColor = RgbColor(255), uint16_t animationTime = 200, AnimEaseFunction easeFunction = NeoEase::CubicInOut)
    {
        AnimUpdateCallback animUpdate = [=](const AnimationParam &param)
        {
            float progress = easeFunction(param.progress);

            RgbColor updatedColor = RgbColor::LinearBlend(startColor, endColor, progress);
            setPixelColor(param.index, updatedColor);
        };

        for (size_t i = 0; i < pixelIndex.size(); i++)
        {
            animator->StartAnimation(pixelIndex[i], animationTime, animUpdate);
        }
    }

    void startSingleFadeAnimationForAllLeds(Nullable<RgbColor> startColor = nullptr, RgbColor endColor = RgbColor(255), uint16_t animationTime = 200, AnimEaseFunction easeFunction = NeoEase::CubicInOut)
    {
        for (uint16_t i = 0; i < _ledCounts; i++)
        {
            const uint16_t index = i;
            startSingleFadeAnimation({i}, (startColor == nullptr ? getPixelColor(index) : (RgbColor)startColor.Value), endColor, animationTime, easeFunction);
        }
    }

    void update()
    {
        if (animator->IsAnimating())
        {
            // the normal loop just needs these two to run the active animations
            animator->UpdateAnimations();
            lights->Show();
        }
    }
};

#endif