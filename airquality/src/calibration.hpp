#pragma once
#include <Preferences.h>

const char* C_TEMPERATURE_SHIFT = "temperature_shift";
const char* C_HUMDITY_LOW_FROM = "humidity_low_from";
const char* C_HUMDITY_LOW_TO = "humidity_low_to";
const char* C_HUMDITY_HIGH_FROM = "humidity_high_from";
const char* C_HUMDITY_HIGH_TO = "humidity_high_to";

const char* P_TEMPERATURE_SHIFT = "pts";
const char* P_HUMDITY_LOW_FROM = "phlf";
const char* P_HUMDITY_LOW_TO = "phlt";
const char* P_HUMDITY_HIGH_FROM = "phhf";
const char* P_HUMDITY_HIGH_TO = "phht";

const char* nameToPref(const char* name) {
  if (strcmp(name, C_TEMPERATURE_SHIFT) == 0) return P_TEMPERATURE_SHIFT;
  if (strcmp(name, C_HUMDITY_LOW_FROM) == 0) return P_HUMDITY_LOW_FROM;
  if (strcmp(name, C_HUMDITY_LOW_TO) == 0) return P_HUMDITY_LOW_TO;
  if (strcmp(name, C_HUMDITY_HIGH_FROM) == 0) return P_HUMDITY_HIGH_FROM;
  if (strcmp(name, C_HUMDITY_HIGH_TO) == 0) return P_HUMDITY_HIGH_TO;

  return 0;
}

struct HumidityMap {
  float fromLow;
  float toLow;
  float fromHigh;
  float toHigh;

  float fix(float value) {
    return map(value, fromLow, fromHigh, toLow, toHigh);
  }
};

struct Calibration {
  float temperature;
  HumidityMap humidity;

  void set(const char* key, float value) {
    if (strcmp(key, C_TEMPERATURE_SHIFT) == 0) {
      temperature = value;
    } else if (strcmp(key, C_HUMDITY_LOW_FROM) == 0) {
      humidity.fromLow = value;
    } else if (strcmp(key, C_HUMDITY_LOW_TO) == 0) {
      humidity.toLow = value;
    } else if (strcmp(key, C_HUMDITY_HIGH_FROM) == 0) {
      humidity.fromHigh = value;
    } else if (strcmp(key, C_HUMDITY_HIGH_TO) == 0) {
      humidity.toHigh = value;
    }
  }
};

Calibration defaultCalibration(const char* mac) {
  if (strcmp(mac, "f008d1c8ad10") == 0) {
    return {
      .temperature = -2.7,
      .humidity = {
        .fromLow = 28,
        .toLow = 38,
        .fromHigh = 70,
        .toHigh = 75,
      },
    };
  } else if (strcmp(mac, "f008d1c846dc") == 0) {
    return {
      .temperature = -1.2,
      .humidity = {
        .fromLow = 32,
        .toLow = 38,
        .fromHigh = 70,
        .toHigh = 70,
      },
    };
  }

  return {};
}


Calibration loadCalibration(
  Preferences &preferences,
  Calibration fallback = {}
) {
  return {
    .temperature = preferences.getFloat(
      P_TEMPERATURE_SHIFT, fallback.temperature
    ),
    .humidity = {
      .fromLow = preferences.getFloat(
        P_HUMDITY_LOW_FROM, fallback.humidity.fromLow
      ),
      .toLow = preferences.getFloat(
        P_HUMDITY_LOW_TO, fallback.humidity.toLow
      ),
      .fromHigh = preferences.getFloat(
        P_HUMDITY_HIGH_FROM, fallback.humidity.fromHigh
      ),
      .toHigh = preferences.getFloat(
        P_HUMDITY_HIGH_TO, fallback.humidity.toHigh
      ),
    },
  };
}

Calibration initCalibration(Preferences &preferences, const char* mac) {
  auto fallback = defaultCalibration(mac);
  auto result = loadCalibration(preferences, fallback);

  preferences.putFloat(P_TEMPERATURE_SHIFT, result.temperature);
  preferences.putFloat(P_HUMDITY_LOW_FROM, result.humidity.fromLow);
  preferences.putFloat(P_HUMDITY_LOW_TO, result.humidity.toLow);
  preferences.putFloat(P_HUMDITY_HIGH_FROM, result.humidity.fromHigh);
  preferences.putFloat(P_HUMDITY_HIGH_TO, result.humidity.toHigh);

  return result;
}