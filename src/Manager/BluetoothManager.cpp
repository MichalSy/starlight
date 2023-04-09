#ifndef BluetoothManager_h
#define BluetoothManager_h

#include <NimBLEDevice.h>
// #include "StarManager.cpp"

#define SERVICE_UUID "70d07a6c-ba09-4c2f-995c-502eb3f1dd29"
#define CHARACTERISTIC_BRIGHTNESS_UUID "e1293490-510d-48a0-8b0f-9530d2b1d169"
#define CHARACTERISTIC_COLOR_UUID "476faf19-b5f5-402c-abc5-313032ffde3b"
#define CHARACTERISTIC_ANIMATION_UUID "c09f2e13-a6f4-44b0-a843-1891eeff8282"

using namespace std;

class BluetoothCallback : public BLECharacteristicCallbacks
{

private:
    function<void()> _eventFunction;

public:
    BluetoothCallback(function<void ()> eventFunction) {
        _eventFunction = eventFunction;
    }

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        if (pCharacteristic->getValue().length() > 0)
        {
            _eventFunction();
        }
    }

};



class BluetoothManager
{

private:
    BluetoothManager() = default;
    function<void()> _settingsChangedEvent;

    BLECharacteristic *_characteristicBrightness;
    BLECharacteristic *_characteristicColor;
    BLECharacteristic *_characteristicAnimation;

public:
    static BluetoothManager *getInstance()
    {
        static BluetoothManager instance;
        return &instance;
    }

    void init()
    {
        BLEDevice::init("Starlight");
        BLEServer *pServer = BLEDevice::createServer();

        BLEService *pService = pServer->createService(SERVICE_UUID);

        _characteristicBrightness = pService->createCharacteristic(CHARACTERISTIC_BRIGHTNESS_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
        _characteristicBrightness->setCallbacks(new BluetoothCallback([&]() { sendSettingsChangedEvent(); }));
        _characteristicBrightness->setValue(100);

        _characteristicColor = pService->createCharacteristic(CHARACTERISTIC_COLOR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
        _characteristicColor->setCallbacks(new BluetoothCallback([&]() { sendSettingsChangedEvent(); }));
        _characteristicColor->setValue((String)"#FF2800");

        _characteristicAnimation = pService->createCharacteristic(CHARACTERISTIC_ANIMATION_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
        _characteristicAnimation->setCallbacks(new BluetoothCallback([&]() { sendSettingsChangedEvent(); }));
        _characteristicAnimation->setValue(0);

        pService->start();

        BLEAdvertising *pAdvertising = pServer->getAdvertising();
        pAdvertising->start();
    }

    void setSettingsChangedEventFunction(function<void ()> eventFunction) {
        _settingsChangedEvent = eventFunction;
    }

    void sendSettingsChangedEvent() {
        Serial.println("New Data");
        _settingsChangedEvent();
    }

    int getBrightness() {
        if (_characteristicBrightness->getValue().length() > 0)
        {
            return _characteristicBrightness->getValue<int>();
        }
        return 0;
    }

    int getAnimationIndex() {
        if (_characteristicAnimation->getValue().length() > 0)
        {
            return _characteristicAnimation->getValue<int>();
        }
        return 0;
    }

    String getColor() {
        if (_characteristicColor->getValue().length() > 0)
        {
            return String(_characteristicColor->getValue());
        }
        return "#FFFFFF";
    }
};

#endif