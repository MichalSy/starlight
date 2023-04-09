#include "AnimationModeBase.cpp"
#include "../Manager/LightManager.cpp"

using namespace std;

class FadeRingAnimationMode : public AnimationModeBase
{

private:
    const vector<vector<uint16_t>> _allRings = {_ledRing1, _ledRing2, _ledRing3, _ledRing4};

    uint32_t circleLifetime;
    uint32_t activateAfterTime;
    uint32_t fadeInTime;

    void activateRing(unsigned int ledRing)
    {
        uint16_t firstPin = _allRings[ledRing][0];
        lightManager->startSingleFadeAnimation(_allRings[ledRing], lightManager->getPixelColor(firstPin), getCurrentColor(), fadeInTime);
    }

    void deactivateRing(unsigned int ledRing)
    {
        uint16_t firstPin = _allRings[ledRing][0];
        lightManager->startSingleFadeAnimation(_allRings[ledRing], lightManager->getPixelColor(firstPin), _colorTransparent, fadeInTime * 2, NeoEase::Linear);
    }

public:
    FadeRingAnimationMode(uint32_t lifetimeInMs = 1300) : AnimationModeBase()
    {
        circleLifetime = lifetimeInMs;
        activateAfterTime = circleLifetime / 5;
        fadeInTime = activateAfterTime;
    }

    void setLifeTimeInMs(uint32_t lifetimeInMs = 1300)
    {
        circleLifetime = lifetimeInMs;
        activateAfterTime = circleLifetime / 5;
        fadeInTime = activateAfterTime;
    }

    void initForTrigger() override
    {
        addTrigger(0, [&]()
                   { lightManager->setAllLightsToColor(_colorTransparent); });

        for (size_t i = 0; i < 4; i++)
        {
            uint32_t starTime = 100 + (i * activateAfterTime);
            addTrigger(starTime, [=]()
                       { activateRing(i); });

            uint32_t fadeOutTime = circleLifetime - ((4 - i) * fadeInTime);
            addTrigger(fadeOutTime, [=]()
                       { deactivateRing(i); });
        }

        addTrigger(circleLifetime + fadeInTime, [&]()
                   { start(); });
    };
};