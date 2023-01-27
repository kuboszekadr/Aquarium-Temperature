#ifndef Thermometer_h
#define Thermometer_h

#include "../SmartHome-MicroContoler-Lib/Events/Events.h"
#include "../SmartHome-MicroContoler-Lib/Sensors/Sensor.h"

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

namespace Sensors
{
  extern Measures measure[1];
  class Thermometer : public Sensor
  {
  public:
    Thermometer(uint8_t pin,
                uint8_t address[],
                uint8_t id_sensor,
                const char *name);

    bool makeReading();
    Events::EventType checkTrigger();

  private:
    uint8_t _pin;
    uint8_t _address[8] = {};
    OneWire _onewire;
    DallasTemperature *_sensor;
  };
}
#endif