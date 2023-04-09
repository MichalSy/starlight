#ifndef AnimationModeBase_h
#define AnimationModeBase_h

#include <Arduino.h>
#include <vector>
#include "../Manager/LightManager.cpp"
#include "../Util/Nullable.cpp"

using namespace std;

struct AnimationTrigger
{
    uint32_t startTime;
    function<void()> triggerFunction;
    uint32_t intervalTime;
};

class AnimationModeBase
{
    

private:
    std::vector<AnimationTrigger> _triggerList;
    std::vector<AnimationTrigger> _intervalList;
    

protected:
    LightManager *lightManager = LightManager::getInstance();
    RgbColor _colorTransparent;
    Nullable<RgbColor> _currentColor = nullptr;
    uint32_t _startTime;
    uint32_t _deltaTime;

    vector<uint16_t> _ledRing1 = {0, 4, 8, 12, 16};
    vector<uint16_t> _ledRing2 = {1, 5, 9, 13, 17};
    vector<uint16_t> _ledRing3 = {2, 6, 10, 14, 18};
    vector<uint16_t> _ledRing4 = {3, 7, 11, 15, 19};

public:
    void start()
    {
        lightManager->stopAllAnimations();
        _startTime = millis();

        initForTrigger();
    }

    void update()
    {
        _deltaTime = millis() - _startTime;

        for (size_t i = 0; i < _triggerList.size(); i++)
        {
            AnimationTrigger currentTrigger = _triggerList[i];
            if (currentTrigger.intervalTime > 0)
            {
                // is interval
                uint32_t checkTime = currentTrigger.startTime;                
                if (checkTime <= _deltaTime)
                {
                    currentTrigger.startTime = currentTrigger.startTime + currentTrigger.intervalTime;
                    currentTrigger.triggerFunction();
                    _triggerList[i] = currentTrigger;
                    break;
                }
            }
            else
            {
                // normal trigger
                if (currentTrigger.startTime <= _deltaTime)
                {
                    currentTrigger.triggerFunction();
                    _triggerList.erase(_triggerList.begin() + i);
                    break;
                }
            }
        }
    }

    void addTrigger(uint32_t startTime, function<void()> triggerFunction)
    {
        // Serial.println((String) "Add Trigger for: " + startTime);
        _triggerList.push_back({startTime, triggerFunction});
    }

    void addInterval(uint32_t startTime, uint32_t intervalTime, function<void()> triggerFunction)
    {
        // Serial.println((String) "Add Trigger for: " + startTime);
        _triggerList.push_back({startTime, triggerFunction, intervalTime});
    }

    void setColor(Nullable<RgbColor> color)
    {
        _currentColor = color;
    }

    RgbColor getCurrentColor()
    {
        if (_currentColor == nullptr)
        {
            setNextRandomColor();
        }
        return _currentColor.Value;
    }

    void setNextRandomColor()
    {
        _currentColor = lightManager->getRandomColor();
    }

    virtual void initForTrigger(){};

    virtual void updateAnimation(uint32_t currentLifetimeMs){};
};

#endif