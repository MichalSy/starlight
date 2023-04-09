#include "AnimationModeBase.cpp"
#include "../Manager/LightManager.cpp"
#include <vector>

using namespace std;

class FadeCircleAnimationMode : public AnimationModeBase
{

private:
    const vector<uint16_t> _ledArm1 = {0, 1, 2, 3};
    const vector<uint16_t> _ledArm2 = {4, 5, 6, 7};
    const vector<uint16_t> _ledArm3 = {8, 9, 10, 11};
    const vector<uint16_t> _ledArm4 = {12, 13, 14, 15};
    const vector<uint16_t> _ledArm5 = {16, 17, 18, 19};

    const vector<vector<uint16_t>> _armList = {_ledArm1, _ledArm2, _ledArm3, _ledArm4, _ledArm5};

    uint32_t circleLifetime = 2000;
    uint32_t activateAfterTime = circleLifetime / 5;
    uint32_t fadeInTime = activateAfterTime;

    void activateArm(unsigned int ledArm)
    {
        uint16_t firstPin = _armList[ledArm][0];
        lightManager->startSingleFadeAnimation(_armList[ledArm], lightManager->getPixelColor(firstPin), getCurrentColor(), fadeInTime);
    }

    void deactivateArm(unsigned int ledArm)
    {
        uint16_t firstPin = _armList[ledArm][0];
        lightManager->startSingleFadeAnimation(_armList[ledArm], lightManager->getPixelColor(firstPin), _colorTransparent, fadeInTime * 3, NeoEase::Linear);
    }

public:
    void initForTrigger() override
    {
        addTrigger(0, [&]()
                   { lightManager->setAllLightsToColor(_colorTransparent); });

        for (size_t i = 0; i < 5; i++)
        {
            uint32_t starTime = 100 + (i * activateAfterTime);
            addInterval(starTime, circleLifetime, [=]()
                        { activateArm(i); });
            addInterval(starTime + fadeInTime + 5, circleLifetime, [=]()
                        { deactivateArm(i); });
        }
    };
};