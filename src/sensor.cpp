// Basic Config
#include "globals.h"
#include "sensor.h"

#if (HAS_SCD30)
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
SCD30 scd30_sensor;
bool scd30_sensor_ready = false;
#endif

#define SENSORBUFFER                                                           \
  10 // max. size of user sensor data buffer in bytes [default=20]

void sensor_init(void) {
  // this function is called during device startup
  // put your user sensor initialization routines here

#if (HAS_SCD30)
  Wire.begin();

  // initialize sensor, activate auto calibration
  if (scd30_sensor.begin(Wire, true)) {
      scd30_sensor.setMeasurementInterval(2);
      scd30_sensor.setTemperatureOffset(SCD30_TEMPERATURE_OFFSET);
      scd30_sensor.setAutoSelfCalibration(SCD30_AUTO_SELF_CALIBRATION);
      scd30_sensor_ready = true;
      ESP_LOGI(TAG, "SCD30 initialized");
  } else {
      ESP_LOGE(TAG, "SCD30 initialization failed");
  }
#endif
}

uint8_t sensor_mask(uint8_t sensor_no) {
  switch (sensor_no) {
  case 0:
    return (uint8_t)COUNT_DATA;
  case 1:
    return (uint8_t)SENSOR1_DATA;
  case 2:
    return (uint8_t)SENSOR2_DATA;
  case 3:
    return (uint8_t)SENSOR3_DATA;
  case 4:
    return (uint8_t)BATT_DATA;
  case 5:
    return (uint8_t)GPS_DATA;
  case 6:
    return (uint8_t)MEMS_DATA;
  case 7:
    return (uint8_t)RESERVED_DATA;
  default:
    return 0;
  }
}

uint8_t *sensor_read(uint8_t sensor) {
  static uint8_t buf[SENSORBUFFER] = {0};
  uint8_t length = 3;

  switch (sensor) {
  case 1:
    // insert user specific sensor data frames here
    ESP_LOGD(TAG, "read data from scd30");
    if (scd30_sensor_ready && scd30_sensor.dataAvailable()) {
        uint16_t co2 = 0;
        float temperature = 0;
        float humidity = 0;

        co2 = scd30_sensor.getCO2();
        temperature = scd30_sensor.getTemperature();
        humidity = scd30_sensor.getHumidity();
        payload.addSCD30(co2, temperature, humidity);

        ESP_LOGI(TAG, "SCD30 co2: %d | humi: %f | temp: %f", co2, humidity, temperature);
    }
    break;
  case 2:
    buf[0] = length;
    buf[1] = 0x01;
    buf[2] = 0x02;
    buf[3] = 0x03;
    break;
  case 3:
    buf[0] = length;
    buf[1] = 0x01;
    buf[2] = 0x02;
    buf[3] = 0x03;
    break;
  }

  return buf;
}