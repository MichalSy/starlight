#ifndef ButtonManager_h
#define ButtonManager_h

#include <Arduino.h>
#include <map>

using namespace std;

struct ButtonPushState
{
    int pin;
    int lastState;
    function<void ()> pushedFunction;
};

class ButtonManager
{

private:
    ButtonManager() = default;

    std::map<int, ButtonPushState> _states;

public:
    static ButtonManager *getInstance()
    {
        static ButtonManager instance;
        return &instance;
    }

    void update()
    {
        for (std::map<int, ButtonPushState>::iterator it = _states.begin(); it != _states.end(); ++it)
        {
            int currentState = digitalRead(it->second.pin);

            if (it->second.lastState == HIGH && currentState == LOW)
            {
                it->second.pushedFunction();
            }

            it->second.lastState = currentState;
        }
    }

    void registerButtonPushedEvent(int pin, function<void ()> pushedFunction)
    {
        Serial.println((String) "Register Pin: " + pin);

        pinMode(pin, INPUT_PULLUP);
        _states[pin] = {pin, 0, pushedFunction};
    }
};

#endif