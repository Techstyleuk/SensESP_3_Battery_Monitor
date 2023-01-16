// SensESP Battery Monitor for Apres 

#include <Wire.h>

/////////////////////Added for INA219
#include <Adafruit_INA219.h>
//////////////////end of Add

#include "sensesp_onewire/onewire_temperature.h"


#include <Arduino.h>


#include "sensesp/sensors/analog_input.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/sensors/sensor.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp_app_builder.h"
#include "sensesp/transforms/linear.h"
#include "sensesp/transforms/analogvoltage.h"
#include "sensesp/transforms/curveinterpolator.h"
#include "sensesp/transforms/voltagedivider.h"
#include "sensesp/sensors/digital_input.h"
#include "sensesp/transforms/frequency.h"


using namespace sensesp;

  // State of Charge lookup
class SoCAInterpreter : public CurveInterpolator {
 public:
  SoCAInterpreter(String config_path = "")
      : CurveInterpolator(NULL, config_path) {
    // Populate a lookup table to translate the Battery A Voltage values returned by
    // our INA219_A to % (Ratio)
    clear_samples();
    // addSample(CurveInterpolator::Sample(knownOhmValue, knownPascal));
    add_sample(CurveInterpolator::Sample(2.5, 0));
    //add_sample(CurveInterpolator::Sample(11.31, .1));
    //add_sample(CurveInterpolator::Sample(11.58, .2));
    //add_sample(CurveInterpolator::Sample(11.75, .3));
    //add_sample(CurveInterpolator::Sample(11.9, .4));
    //add_sample(CurveInterpolator::Sample(12.06, .5));
    //add_sample(CurveInterpolator::Sample(12.2, .6));
    //add_sample(CurveInterpolator::Sample(12.32, .7));
    //add_sample(CurveInterpolator::Sample(12.42, .8)); 
    //add_sample(CurveInterpolator::Sample(12.5, .9));
    add_sample(CurveInterpolator::Sample(6.0, 1)); 
    add_sample(CurveInterpolator::Sample(20.0, 1)); 
  }
};
class SoCBInterpreter : public CurveInterpolator {
 public:
  SoCBInterpreter(String config_path = "")
      : CurveInterpolator(NULL, config_path) {
    // Populate a lookup table to translate the Battery B Voltage values returned by
    // our INA219_B to % (Ratio)
    clear_samples();
    // addSample(CurveInterpolator::Sample(knownOhmValue, knownPascal));
    add_sample(CurveInterpolator::Sample(10.5, 0));
    //add_sample(CurveInterpolator::Sample(11.31, .1));
    //add_sample(CurveInterpolator::Sample(11.58, .2));
    //add_sample(CurveInterpolator::Sample(11.75, .3));
    //add_sample(CurveInterpolator::Sample(11.9, .4));
    //add_sample(CurveInterpolator::Sample(12.06, .5));
    //add_sample(CurveInterpolator::Sample(12.2, .6));
    //add_sample(CurveInterpolator::Sample(12.32, .7));
    //add_sample(CurveInterpolator::Sample(12.42, .8)); 
    //add_sample(CurveInterpolator::Sample(12.5, .9));
    add_sample(CurveInterpolator::Sample(12.6, 1)); 
    add_sample(CurveInterpolator::Sample(20.0, 1)); 
  }
};
class SoCCInterpreter : public CurveInterpolator {
 public:
  SoCCInterpreter(String config_path = "")
      : CurveInterpolator(NULL, config_path) {
    // Populate a lookup table to translate the Battery C Voltage values returned by
    // our INA219_C to % (Ratio)
    clear_samples();
    // addSample(CurveInterpolator::Sample(knownOhmValue, knownPascal));
    add_sample(CurveInterpolator::Sample(10.5, 0));
    //add_sample(CurveInterpolator::Sample(11.31, .1));
    //add_sample(CurveInterpolator::Sample(11.58, .2));
    //add_sample(CurveInterpolator::Sample(11.75, .3));
    //add_sample(CurveInterpolator::Sample(11.9, .4));
    //add_sample(CurveInterpolator::Sample(12.06, .5));
    //add_sample(CurveInterpolator::Sample(12.2, .6));
    //add_sample(CurveInterpolator::Sample(12.32, .7));
    //add_sample(CurveInterpolator::Sample(12.42, .8)); 
    //add_sample(CurveInterpolator::Sample(12.5, .9));
    add_sample(CurveInterpolator::Sample(12.6, 1)); 
    add_sample(CurveInterpolator::Sample(20.0, 1)); 
  }
};

reactesp::ReactESP app;


////////////////////INA219

  Adafruit_INA219 ina219_A;
  Adafruit_INA219 ina219_B(0x41);
  Adafruit_INA219 ina219_C(0x44);

  const float RshuntA = 0.1;
  const float RshuntB = 0.1;
  const float RshuntC = 0.1;

  //float read_A_current_callback() { return (ina219_A.getCurrent_mA() / 1000);}
  float read_A_current_callback() { return ((ina219_A.getShuntVoltage_mV() / 1000) / RshuntA);}
  float read_A_shuntvoltage_callback() { return (ina219_A.getShuntVoltage_mV() / 1000);}
  float read_A_busvoltage_callback() { return (ina219_A.getBusVoltage_V());}
  float read_A_loadvoltage_callback() { return (ina219_A.getBusVoltage_V() + (ina219_A.getShuntVoltage_mV() / 1000));}
  float read_A_power_callback() { return ((ina219_A.getBusVoltage_V() + (ina219_A.getShuntVoltage_mV() / 1000)) * (ina219_A.getCurrent_mA() / 1000));}

    //float read_B_current_callback() { return (ina219_B.getCurrent_mA() / 1000);}
  float read_B_current_callback() { return ((ina219_B.getShuntVoltage_mV() / 1000) / RshuntB);}
  float read_B_shuntvoltage_callback() { return (ina219_B.getShuntVoltage_mV() / 1000);}
  float read_B_busvoltage_callback() { return (ina219_B.getBusVoltage_V());}
  float read_B_loadvoltage_callback() { return (ina219_B.getBusVoltage_V() + (ina219_B.getShuntVoltage_mV() / 1000));}
  float read_B_power_callback() { return ((ina219_B.getBusVoltage_V() + (ina219_B.getShuntVoltage_mV() / 1000)) * (ina219_B.getCurrent_mA() / 1000));}

    //float read_C_current_callback() { return (ina219_C.getCurrent_mA() / 1000);}
  float read_C_current_callback() { return ((ina219_C.getShuntVoltage_mV() / 1000) / RshuntC);}
  float read_C_shuntvoltage_callback() { return (ina219_C.getShuntVoltage_mV() / 1000);}
  float read_C_busvoltage_callback() { return (ina219_C.getBusVoltage_V());}
  float read_C_loadvoltage_callback() { return (ina219_C.getBusVoltage_V() + (ina219_C.getShuntVoltage_mV() / 1000));}
  float read_C_power_callback() { return ((ina219_C.getBusVoltage_V() + (ina219_C.getShuntVoltage_mV() / 1000)) * (ina219_C.getCurrent_mA() / 1000));}
/////////////////////INA219 end of Add

//Some general battery related messages
//Battery A - Typical Start
const char* Abatname = "/A Battery/Name";
const char* Abatlocation = "/A Battery/Location";
const char* Abatchemistry = "/Start Battery/Chemistry";
const char* Abatcapacity = "/Start Battery/Capacity (Ah)";

//Battery B - Typical House 1
const char* Bbatname = "/B Battery/Name";
const char* Bbatlocation = "/B Battery/Location";
const char* Bbatchemistry = "/B Battery/Chemistry";
const char* Bbatcapacity = "/B Battery/Capacity (Ah)";

//Battery C
const char* Cbatname = "/C Battery/Name";
const char* Cbatlocation = "/C Battery/Location";
const char* Cbatchemistry = "/C Battery/Chemistry";
const char* Cbatcapacity = "/C Battery/Capacity (Ah)";

// The setup function performs one-time application initialization.
void setup() {
#ifndef SERIAL_DEBUG_ENABLED
  SetupSerialDebug(115200);
#endif

  Wire.begin(21,22);                // join i2c bus (address optional for master)
  //
  //Serial.begin(9600);          // start serial communication at 9600bps
  
  //Serial.println(F("BME280 Forced Mode Test."));

  //if (!bme280.begin(BME280_ADDRESS_ALT, BME280_CHIPID)) {
  //if (!bme280.begin()) {
    //Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      //"try a different address!"));
    //while (1) delay(10);// could need a delay here:
  //} 
  //  

  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("SensESP")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi("My WiFi SSID", "my_wifi_password")
                    //->set_sk_server("192.168.10.3", 80)
                    ->enable_uptime_sensor()
                    ->get_app();

/// 1-Wire Temp Sensors 

  DallasTemperatureSensors* dts = new DallasTemperatureSensors(25); //digital 2

 // A Battery Temperature - electrical.batteries.A.temperature
  auto* A_bat_temp =
      new OneWireTemperature(dts, 1000, "/A Battery Temperature/oneWire");

  A_bat_temp->connect_to(new Linear(1.0, 0.0, "/A Battery Temperature/linear"))
      ->connect_to(
          new SKOutputFloat("/electrical.batteries.A.temperature",
                             "/A Battery Temperature/sk_path"));

 // B Battery Temperature - electrical.batteries.B.temperature
  auto* B_bat_temp =
      new OneWireTemperature(dts, 1000, "/B Battery Temperature/oneWire");

  B_bat_temp->connect_to(new Linear(1.0, 0.0, "/B Battery Temperature/linear"))
      ->connect_to(
          new SKOutputFloat("/electrical.batteries.B.temperature",
                             "/B Battery Temperature/sk_path"));

  // C Battery Temperature - electrical.batteries.C.temperature
  auto* C_bat_temp =
      new OneWireTemperature(dts, 1000, "/C Battery Temperature/oneWire");

  C_bat_temp->connect_to(new Linear(1.0, 0.0, "/C Battery Temperature/linear"))
      ->connect_to(
          new SKOutputFloat("/electrical.batteries.C.temperature",
                             "/C Battery Temperature/sk_path"));

///////////////////////////INA219 start
  ina219_A.begin();  // Initialize first board (default address 0x40)
  ina219_B.begin();  // Initialize second board with the address 0x41 ----> A0 soldered = 0x41
  ina219_C.begin();  // Initialize second board with the address 0x44 ----> A1 soldered = 0x44

//ina219_A
  auto* ina219_A_current =
      new RepeatSensor<float>(1000, read_A_current_callback);

  auto* ina219_A_shuntvoltage = 
      new RepeatSensor<float>(1000, read_A_shuntvoltage_callback);

  auto* ina219_A_busvoltage = 
      new RepeatSensor<float>(1000, read_A_busvoltage_callback);   

  auto* ina219_A_loadvoltage = 
      new RepeatSensor<float>(1000, read_A_loadvoltage_callback); 

    auto* ina219_A_power = 
      new RepeatSensor<float>(1000, read_A_power_callback);    

  // Send the temperature to the Signal K server as a Float
  ina219_A_current->connect_to(new SKOutputFloat("electrical.batteries.A.current"));

  ina219_A_loadvoltage->connect_to(new SKOutputFloat("electrical.batteries.A.voltage"));

  //ina219_A_shuntvoltage->connect_to(new SKOutputFloat("electrical.batteries.A.shuntv"));

  //ina219_A_busvoltage->connect_to(new SKOutputFloat("electrical.batteries.A.vin-v"));

  ina219_A_power->connect_to(new SKOutputFloat("electrical.batteries.A.power"));

  ina219_A_loadvoltage->connect_to(new SoCAInterpreter("/Battery A/Voltage/curve"))
      ->connect_to(new SKOutputFloat("electrical.batteries.A.capacity.stateOfCharge", "/Battery A State of Charge/sk_path"));


//ina219_B
  auto* ina219_B_current =
      new RepeatSensor<float>(1000, read_B_current_callback);

  auto* ina219_B_shuntvoltage = 
      new RepeatSensor<float>(1000, read_B_shuntvoltage_callback);

  auto* ina219_B_busvoltage = 
      new RepeatSensor<float>(1000, read_B_busvoltage_callback);   

  auto* ina219_B_loadvoltage = 
      new RepeatSensor<float>(1000, read_B_loadvoltage_callback); 

    auto* ina219_B_power = 
      new RepeatSensor<float>(1000, read_B_power_callback);    

  // Send the temperature to the Signal K server as a Float
  ina219_B_current->connect_to(new SKOutputFloat("electrical.batteries.B.current"));

  ina219_B_loadvoltage->connect_to(new SKOutputFloat("electrical.batteries.B.voltage"));

  //ina219_B_shuntvoltage->connect_to(new SKOutputFloat("electrical.batteries.B.shuntv"));

  //ina219_B_busvoltage->connect_to(new SKOutputFloat("electrical.batteries.B.vin-v"));

  ina219_B_power->connect_to(new SKOutputFloat("electrical.batteries.B.power"));

  ina219_B_loadvoltage->connect_to(new SoCBInterpreter("/Battery B/Voltage/curve"))
      ->connect_to(new SKOutputFloat("electrical.batteries.B.capacity.stateOfCharge", "/Battery B State of Charge/sk_path"));

  //ina219_C
  auto* ina219_C_current =
      new RepeatSensor<float>(1000, read_C_current_callback);

  auto* ina219_C_shuntvoltage = 
      new RepeatSensor<float>(1000, read_C_shuntvoltage_callback);

  auto* ina219_C_busvoltage = 
      new RepeatSensor<float>(1000, read_C_busvoltage_callback);   

  auto* ina219_C_loadvoltage = 
      new RepeatSensor<float>(1000, read_C_loadvoltage_callback); 

    auto* ina219_C_power = 
      new RepeatSensor<float>(1000, read_C_power_callback);    

  // Send the temperature to the Signal K server as a Float
  ina219_C_current->connect_to(new SKOutputFloat("electrical.batteries.C.current"));

  ina219_C_loadvoltage->connect_to(new SKOutputFloat("electrical.batteries.C.voltage"));

  //ina219_C_shuntvoltage->connect_to(new SKOutputFloat("electrical.batteries.C.shuntv"));

  //ina219_C_busvoltage->connect_to(new SKOutputFloat("electrical.batteries.C.vin-v"));

  ina219_C_power->connect_to(new SKOutputFloat("electrical.batteries.C.power"));

  ina219_C_loadvoltage->connect_to(new SoCCInterpreter("/Battery C/Voltage/curve"))
        ->connect_to(new SKOutputFloat("electrical.batteries.C.capacity.stateOfCharge", "/Battery C State of Charge/sk_path"));

///////////////////////end of INA add

  //Abatname->connect_to(new SKOutputFloat("electrical.batteries.A.name");
  //Abatlocation->connect_to(new SKOutputFloat("electrical.batteries.A.location"));
  //Abatchemistry->connect_to(new SKOutputFloat("electrical.batteries.A.chemistry"));


  // Start networking, SK server connections and other SensESP internals
  sensesp_app->start();

}

void loop() { app.tick(); }
