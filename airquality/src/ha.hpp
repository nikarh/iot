#include <ArduinoHA.h>
#include "calibration.hpp"

char* p(const char prefix[12], const char* placeholder) {
  int len = strlen_P(placeholder);
  char* result = (char*) malloc(len + 14);
  result[12] = '_';
  result[len + 13] = '\0';
  memcpy(result, prefix, 12);
  memcpy(result+13, placeholder, len);
  return result;
}

struct HA {
  HASensorNumber sensor_temp;
  HASensorNumber sensor_humi;
  HASensorNumber sensor_press;
  HASensorNumber sensor_co2;

  HANumber temperature_shift;
  HANumber humidity_low_from;
  HANumber humidity_low_to;
  HANumber humidity_high_from;
  HANumber humidity_high_to;

  HAButton calibrate_co2;

  HA(
    const char pfx[12], 
    void (*onCalibrationChange)(HANumeric number, HANumber* sender)
  ):
    sensor_temp(
      p(pfx, "temperature"),
      HASensorNumber::PrecisionP1
    ),
    sensor_humi(
      p(pfx, "humidity"),
      HASensorNumber::PrecisionP0
    ),
    sensor_press(
      p(pfx, "pressure"),
      HASensorNumber::PrecisionP2
    ),
    sensor_co2(
      p(pfx, "co2"),
      HASensorNumber::PrecisionP0
    ),
    temperature_shift(
      p(pfx, C_TEMPERATURE_SHIFT),
      HASensorNumber::PrecisionP1
    ),
    humidity_low_from(
      p(pfx, C_HUMDITY_LOW_FROM),
      HASensorNumber::PrecisionP1
    ),
    humidity_low_to(
      p(pfx, C_HUMDITY_LOW_TO),
      HASensorNumber::PrecisionP1
    ),
    humidity_high_from(
      p(pfx, C_HUMDITY_HIGH_FROM),
      HASensorNumber::PrecisionP1
    ),
    humidity_high_to(
      p(pfx, C_HUMDITY_HIGH_TO),
      HASensorNumber::PrecisionP1
    ),
    calibrate_co2(p(pfx, "calibrate_co2"))
  {
    sensor_temp.setIcon("mdi:thermometer");
    sensor_temp.setName("Temperature");
    sensor_temp.setUnitOfMeasurement("C");

    sensor_humi.setIcon("mdi:water-percent");
    sensor_humi.setName("Humidity");
    sensor_humi.setUnitOfMeasurement("%");

    sensor_press.setIcon("mdi:arrow-collapse-down");
    sensor_press.setName("Air pressure");
    sensor_press.setUnitOfMeasurement("hPa");

    sensor_co2.setIcon("mdi:molecule-co2");
    sensor_co2.setName("CO2");
    sensor_co2.setUnitOfMeasurement("PPM");

    temperature_shift.setName("Calibration temperature difference");
    temperature_shift.setUnitOfMeasurement("C");
    temperature_shift.setMin(-6);
    temperature_shift.setMax(+6);
    temperature_shift.setOptimistic(true);
    temperature_shift.setStep(0.1);
    temperature_shift.onCommand(onCalibrationChange);

    humidity_low_from.setName("Calibration humidity lowFrom");
    humidity_low_from.setUnitOfMeasurement("%");
    humidity_low_from.setMin(-100);
    humidity_low_from.setMax(+100);
    humidity_low_from.setOptimistic(true);
    humidity_low_from.setStep(0.1);
    humidity_low_from.onCommand(onCalibrationChange);

    humidity_low_to.setName("Calibration humidity lowTo");
    humidity_low_to.setUnitOfMeasurement("%");
    humidity_low_to.setMin(-100);
    humidity_low_to.setMax(+100);
    humidity_low_to.setOptimistic(true);
    humidity_low_to.setStep(0.1);
    humidity_low_to.onCommand(onCalibrationChange);

    humidity_high_from.setName("Calibration humidity highFrom");
    humidity_high_from.setUnitOfMeasurement("%");
    humidity_high_from.setMin(-100);
    humidity_high_from.setMax(+100);
    humidity_high_from.setOptimistic(true);
    humidity_high_from.setStep(0.1);
    humidity_high_from.onCommand(onCalibrationChange);

    humidity_high_to.setName("Calibration humidity highTo");
    humidity_high_to.setUnitOfMeasurement("%");
    humidity_high_to.setMin(-100);
    humidity_high_to.setMax(+100);
    humidity_high_to.setOptimistic(true);
    humidity_high_to.setStep(0.1);
    humidity_high_to.onCommand(onCalibrationChange);

    calibrate_co2.setName("Start CO2 calibration");
  }

  void init(Calibration cal) {
    temperature_shift.setCurrentState(cal.temperature);
    humidity_low_from.setCurrentState(cal.humidity.fromLow);
    humidity_low_to.setCurrentState(cal.humidity.toLow);
    humidity_high_from.setCurrentState(cal.humidity.fromHigh);
    humidity_high_to.setCurrentState(cal.humidity.toHigh);
  }
};
