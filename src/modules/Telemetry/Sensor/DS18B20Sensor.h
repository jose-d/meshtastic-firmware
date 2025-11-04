#include "configuration.h"

#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR && defined(HAS_ONE_WIRE) && __has_include(<DallasTemperature.h>)

#include "../mesh/generated/meshtastic/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <DallasTemperature.h>
#include <OneWire.h>

class DS18B20Sensor : public TelemetrySensor
{
  public:
    DS18B20Sensor();
    virtual bool getMetrics(meshtastic_Telemetry *measurement) override;
    virtual bool initDevice(TwoWire *bus, ScanI2C::FoundDevice *dev) override;

  private:
    void setup();
    OneWire *oneWire = nullptr;
    DallasTemperature *sensors = nullptr;
};

#endif
