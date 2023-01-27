#include "Thermometer.h"

Sensors::Measures Sensors::measure[1] = {Sensors::Measures::TEMP};

Sensors::Thermometer::Thermometer(
    uint8_t pin,
    uint8_t *address,
    uint8_t id_sensor,
    const char *name)
    : Sensor(
          id_sensor,
          measure,
          1,
          name,
          Events::EventType::TEMP_LOW,
          Events::EventType::TEMP_HIGH)
{
  _pin = pin;

  // Copy thermometer address
  memcpy(_address, address, 8);

  // initalize relevant variables
  _onewire = OneWire(_pin);
  _sensor = new DallasTemperature(&_onewire);

  _sensor->begin();
  // _sensor->request(_address);
}

// bool Sensors::Thermometer::isReady()
// {
//   return (millis() - _last_reading >= _sampling_interval) && _sensor->available();
// }

Events::EventType Sensors::Thermometer::checkTrigger()
{
}

bool Sensors::Thermometer::makeReading()
{
  // check if device can be requested
  // if (!_sensor->available())
  // {
  //   return false;
  // }

  _sensor->requestTemperatures();

  _readings[0] += _sensor->getTempCByIndex(0); // insert new reading data
  _readings_count++;

  _last_reading = millis();
  return true;
}