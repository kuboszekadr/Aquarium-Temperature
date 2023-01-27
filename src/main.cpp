#include "Device/Device.h"
#include "Sensors/Sensors.h"
#include "ESP32WebServer/ESP32WebServer.h"

#include "Logger/Logger.h"
#include "Logger/Loggers/API.hpp"
#include "Logger/Loggers/Serial.hpp"

#include "Notification/Notification.h"
#include "Events/Events.h"

#include "Thermometer.h"

#include <Arduino.h>
#include <SPIFFS.h>
#include <CronAlarms.h>

int status = WL_IDLE_STATUS;

uint8_t thermometer_address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Sensors::Thermometer thermometer(4, thermometer_address, 1, "WaterTemperature");

void initTasks();
void setupSensor();
void sendData();

void GmailNotification(
    const char *title,
    const char *message);

Logger logger = Logger("main");

void setup()
{
  Serial.begin(115200);

  Device::setup();

  Logger::addStream(Loggers::logToSerial);
  Logger::addStream(Loggers::logToAPI);

  Notification::addStream(GmailNotification);

  initTasks();
  setupSensor();

  logger.log("Setup complete");
  Notification::push("WaterTemperature-init", "Device started");
}

void loop()
{
  Cron.delay(); 
  Sensors::loop();

  Events::notifyListeners();
  sendData();
}

void initTasks()
{
  logger.log("Setting tasks...");
  Cron.create(
      "0 0 4 * * *",
      Device::setupTime,
      false);

  Cron.create(
      "0 */2 * * * *",
      WiFiManager::manageConnection,
      false);          

  Cron.create(
      "*/30 * * * * *",
      Device::sendHeartbeat,
      false);          
}

void sendData()
{
  if (Sensors::readings.size() > 0)
  {
    JsonVariant data = Sensors::readings.as<JsonVariant>();
    Device::device->postReadings(data, "WaterTemperature");
    ESP32WebServer::last_reading = data;
    Sensors::readings.clear();
  }
}

void setupSensor()
{
  logger.log("Setting sensors...");

  Config sensor_config = Config("sensor");
  sensor_config.load();

  thermometer.setSampling(
      sensor_config.data["sampling_amount"],
      sensor_config.data["sampling_interval"]);

  thermometer.setTriggerValues(
      sensor_config.data["trigger_low"],
      sensor_config.data["trigger_high"]);
}

void GmailNotification(const char *title, const char *message)
{
  Device::device->postNotification(title, message);
}