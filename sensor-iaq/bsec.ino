#include <EEPROM.h>
#include "bsec.h"
/* Configure the BSEC library with information about the sensor
    18v/33v = Voltage at Vdd. 1.8V or 3.3V
    3s/300s = BSEC operating mode, BSEC_SAMPLE_RATE_LP or BSEC_SAMPLE_RATE_ULP
    4d/28d = Operating age of the sensor in days
    generic_18v_3s_4d
    generic_18v_3s_28d
    generic_18v_300s_4d
    generic_18v_300s_28d
    generic_33v_3s_4d
    generic_33v_3s_28d
    generic_33v_300s_4d
    generic_33v_300s_28d
*/
const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

#define STATE_SAVE_PERIOD  UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);

// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

String bsecOutput;
String output;

void bsecUpdateTemperatureOffset(float temperatureOffset) {
    iaqSensor.setTemperatureOffset(temperatureOffset);
}

// Entry point for the example
void bsecSetup(void)
{
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length
    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
    output = "BSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
    msgln("BSEC", output);
    checkIaqSensorStatus();

    iaqSensor.setConfig(bsec_config_iaq);
    bsecUpdateTemperatureOffset(configTemperatureOffset);
    checkIaqSensorStatus();

    loadState();

    bsec_virtual_sensor_t sensorList[10] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();

    // Print the header
    msgln("BSEC", "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent");
}

// Function that is looped forever
void bsecLoop(void)
{
  unsigned long time_trigger = millis();
    if (iaqSensor.run()) { // If new data is available
        bsecOutput = "uptime=" + String(time_trigger);
        bsecOutput += ",rawTemperature=" + String(iaqSensor.rawTemperature);
        bsecOutput += ",rawPressure=" + String(iaqSensor.pressure);
        bsecOutput += ",rawHumidity=" + String(iaqSensor.rawHumidity);
        bsecOutput += ",gasResistance=" + String(iaqSensor.gasResistance);
        bsecOutput += ",iaq=" + String(iaqSensor.iaq);
        bsecOutput += ",iaqAccuracy=" + String(iaqSensor.iaqAccuracy);
        bsecOutput += ",temperature=" + String(iaqSensor.temperature);
        bsecOutput += ",humidity=" + String(iaqSensor.humidity);
        bsecOutput += ",staticIaq=" + String(iaqSensor.staticIaq);
        bsecOutput += ",co2Equivalent=" + String(iaqSensor.co2Equivalent);
        bsecOutput += ",breathVocEquivalent=" + String(iaqSensor.breathVocEquivalent);
        msgln("BSEC", bsecOutput);
        updateState();
//        showCalibrationState(iaqSensor.iaqAccuracy);
//        showIAQState(iaqSensor.iaq);
    } else {
        checkIaqSensorStatus();
    }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
    if (iaqSensor.status != BSEC_OK) {
        if (iaqSensor.status < BSEC_OK) {
            output = "BSEC error code : " + String(iaqSensor.status);
            msgln("BSEC", output);
            for (;;)
            errLeds(); /* Halt in case of failure */
        } else {
            output = "BSEC warning code : " + String(iaqSensor.status);
            msgln("BSEC", output);
        }
    }

    if (iaqSensor.bme680Status != BME680_OK) {
        if (iaqSensor.bme680Status < BME680_OK) {
            output = "BME680 error code : " + String(iaqSensor.bme680Status);
            msgln("BSEC", output);
            for (;;)
            errLeds(); /* Halt in case of failure */
        } else {
            output = "BME680 warning code : " + String(iaqSensor.bme680Status);
            msgln("BSEC", output);
        }
    }
    iaqSensor.status = BSEC_OK;
}

void errLeds(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

void loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    // Existing state in EEPROM
    msgln("BSEC", "Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
//      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  } else {
    // Erase the EEPROM with zeroes
    msgln("BSEC", "Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
        EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void updateState(void)
{
  bool update = false;
  /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
  if (stateUpdateCounter == 0) {
    if (iaqSensor.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD milliseconds */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    msgln("BSEC", "Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
//      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}
