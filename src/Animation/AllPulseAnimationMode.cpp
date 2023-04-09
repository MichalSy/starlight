#include "AnimationModeBase.cpp"
#include "../Manager/LightManager.cpp"

using namespace std;

class AllPulseAnimationMode : public AnimationModeBase
{

private:
public:
    void initForTrigger() override
    {

        addTrigger(0, [&]()
                    {
                        lightManager->setAllLightsToColor(_colorTransparent);
                    });

        addTrigger(100, [&]()
                    {
                        lightManager->startSingleFadeAnimation(_ledRing1, _colorTransparent, getCurrentColor(), 1000);
                    });

        addTrigger(600, [&]()
                    {
                        lightManager->startSingleFadeAnimation(_ledRing2, _colorTransparent, getCurrentColor(), 1000);
                    });

        addTrigger(1200, [&]()
                    {
                        lightManager->startSingleFadeAnimation(_ledRing3, _colorTransparent, getCurrentColor(), 1000);
                    });

        addTrigger(1800, [&]()
                    {
                        lightManager->startSingleFadeAnimation(_ledRing4, _colorTransparent, getCurrentColor(), 1000);
                    });

        addTrigger(3000, [&]()
                    {
                        lightManager->startSingleFadeAnimationForAllLeds(nullptr, _colorTransparent, 1000);
                    });

        addTrigger(4100, [&]()
                    {
                        // Serial.println((String) "Reset after: " + _deltaTime + "ms");
                        start();
                    });
    };
};