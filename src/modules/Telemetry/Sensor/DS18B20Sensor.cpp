#include "configuration.h"

#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR && defined(HAS_ONE_WIRE) && __has_include(<DallasTemperature.h>)

// TODO: I think those are duplicates. should be removed, as those are in DS18B20Sensor.h.
// FIXMe.
#include "../mesh/generated/meshtastic/telemetry.pb.h"
#include "DS18B20Sensor.h"
#include "TelemetrySensor.h"
#include "main.h"
#include <DallasTemperature.h>
#include <OneWire.h>

DS18B20Sensor::DS18B20Sensor() : TelemetrySensor(meshtastic_TelemetrySensorType_SENSOR_UNSET, "DS18B20") {}

bool DS18B20Sensor::initDevice(TwoWire *bus, ScanI2C::FoundDevice *dev)
{
    LOG_INFO("Init sensor: %s", sensorName);
    // Initialize OneWire and DallasTemperature on the configured pin
#ifdef ONE_WIRE_BUS_PIN
    oneWire = new OneWire(ONE_WIRE_BUS_PIN);
    sensors = new DallasTemperature(oneWire);
    sensors->begin();

    // If no devices found, fail init
    if (sensors->getDeviceCount() == 0) {
        LOG_WARN("%s: no DS18B20 devices detected on pin %d", sensorName, ONE_WIRE_BUS_PIN);
        status = 0;
        initialized = true;
        return false;
    }

    setup();
    status = 1; // mark as running
    initialized = true;
    return true;
#else
    LOG_WARN("%s: ONE_WIRE_BUS_PIN not defined for this variant", sensorName);
    status = 0;
    initialized = true;
    return false;
#endif
}

void DS18B20Sensor::setup()
{
    if (!sensors)
        return;
    sensors->setResolution(10);
}

bool DS18B20Sensor::getMetrics(meshtastic_Telemetry *measurement)
{
    if (!sensors)
        return false;

    sensors->requestTemperatures();
    float t = sensors->getTempCByIndex(0);

    // DallasTemperature returns DEVICE_DISCONNECTED_C (-127) when not present
    if (t <= -100.0f) {
        LOG_WARN("%s: read failed (disconnected)", sensorName);
        return false;
    }

    measurement->variant.environment_metrics.has_temperature = true;
    measurement->variant.environment_metrics.temperature = t;
    LOG_DEBUG("%s getMetrics: temperature=%.2fC", sensorName, t);
    return true;
}

#endif
