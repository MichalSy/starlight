#include <Arduino.h>
#include "Manager/StarManager.cpp"

StarManager *starManager = StarManager::getInstance();

void setup()
{
  Serial.begin(9600);

  while (!Serial)
  {
    ;
  } // wait for serial port to connect

  starManager->init();
}

void loop()
{
  starManager->update();
  delay(10);
}