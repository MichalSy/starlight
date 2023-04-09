#ifndef StarManager_h
#define StarManager_h

#include <Arduino.h>
#include <vector>
#include "LightManager.cpp"
#include "ButtonManager.cpp"
#include "BluetoothManager.cpp"
#include "../Animation/AnimationModeBase.cpp"
#include "../Animation/AllPulseAnimationMode.cpp"
#include "../Animation/FadeCircleAnimationMode.cpp"
#include "../Animation/FadeRingAnimationMode.cpp"
#include "../Util/Nullable.cpp"

using namespace std;

class StarManager
{

private:
    StarManager() = default;

    LightManager *lightManager = LightManager::getInstance();
    ButtonManager *buttonManager = ButtonManager::getInstance();
    BluetoothManager *bluetoothManager = BluetoothManager::getInstance();

    vector<AnimationModeBase *> _allAnimations {new AllPulseAnimationMode(), new FadeRingAnimationMode(), new FadeRingAnimationMode(2500), new FadeCircleAnimationMode()};

    uint8_t _brightness = 255;
    Nullable<RgbColor> _currentColor = nullptr;
    uint8_t _currentAnimationIndex = 0;

    AnimationModeBase *_currentAnimationMode = NULL;

    bool _isRandomColorMode = false;
    RgbColor _randomColorStartColor;
    Nullable<RgbColor> _randomColorNextColor = nullptr;
    float _randomColorModeProgress;

    ulong _lastColorChangedPushed = 0;

public:
    static StarManager *getInstance()
    {
        static StarManager instance;
        return &instance;
    }

    void init()
    {
        bluetoothManager->init();
        bluetoothManager->setSettingsChangedEventFunction([&]()
                                                          { settingChanged(); });
        lightManager->init();

        buttonManager->registerButtonPushedEvent(0, [&]()
                                                 { switchMode(); });

        buttonManager->registerButtonPushedEvent(16, [&]()
                                                 { switchColor(); });

        _currentColor = lightManager->getRandomColor();
        startAnimation();
    }

    void update()
    {
        buttonManager->update();

        handleRandomColorMode();

        if (_currentAnimationMode)
        {
            _currentAnimationMode->update();
        }

        lightManager->update();
    }

    void handleRandomColorMode()
    {
        if (_isRandomColorMode)
        {
            // reset transition if no next color available
            if (_randomColorNextColor == nullptr)
            {
                _randomColorStartColor = _currentColor.Value;
                _randomColorModeProgress = 0;
                RgbColor nextColor = lightManager->getRandomColor();
                _randomColorNextColor = nextColor;
            }

            _currentColor = RgbColor::LinearBlend(_randomColorStartColor, _randomColorNextColor.Value, _randomColorModeProgress).Dim(_brightness);
            _randomColorModeProgress += 0.01;

            if (_randomColorModeProgress >= 1)
            {
                _randomColorNextColor = nullptr;
            }

            if (_currentAnimationMode)
            {
                _currentAnimationMode->setColor(_currentColor);
            }
        }
    }

    void switchMode()
    {
        _currentAnimationIndex++;
        if (_currentAnimationIndex > _allAnimations.size() - 1)
        {
            _currentAnimationIndex = 0;
        }
        startAnimation();
    }

    void switchColor()
    {
        Serial.println((String)"Last: " + _lastColorChangedPushed + " Current: " + millis());
        if (_lastColorChangedPushed + 600 > millis())
        {
            Serial.println("Switch Color to Random");
            updateCurrentColor("#");
        }
        else
        {
            Serial.println("Switch Color");
            _isRandomColorMode = false;
            _currentColor = lightManager->getRandomColor();
            if (_currentAnimationMode)
            {
                _currentAnimationMode->setColor(_currentColor);
            }
        }

        _lastColorChangedPushed = millis();
    }

    void settingChanged()
    {
        _brightness = (bluetoothManager->getBrightness() / 100.0) * 255;

        updateCurrentColor(bluetoothManager->getColor());

        uint16_t newAnimationIndex = bluetoothManager->getAnimationIndex();
        Serial.println((String) "New Animation Index: " + newAnimationIndex);
        if (newAnimationIndex != _currentAnimationIndex)
        {
            _currentAnimationIndex = newAnimationIndex;
            startAnimation();
        }
    }

    void updateCurrentColor(String newColor)
    {
        _isRandomColorMode = newColor == "#";
        Serial.println((String) "New Color: " + (!_isRandomColorMode ? newColor : "Random"));

        if (_isRandomColorMode)
        {
            _randomColorNextColor = nullptr;
            return;
        }
        else
        {
            HtmlColor col = HtmlColor();
            col.Parse<HtmlColorNames>(newColor);
            _currentColor = RgbColor(col).Dim(_brightness);
        }

        if (_currentAnimationMode)
        {
            _currentAnimationMode->setColor(_currentColor);
        }
    }

    void startAnimation()
    {
        _currentAnimationMode = _allAnimations[_currentAnimationIndex];
        _currentAnimationMode->setColor(_currentColor);
        _currentAnimationMode->start();
    }
};

#endif