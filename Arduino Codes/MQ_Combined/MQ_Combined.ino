#include <MQUnifiedsensor.h>

#define Board ("Arduino Pro Mini")
#define Voltage_Resolution (5)
#define ADC_Bit_Resolution (10)
#define RatioMQ2CleanAir (9.83)
#define RatioMQ4CleanAir (4.4)
#define RatioMQ135CleanAir (3.61)
#define RatioMQ7CleanAir (27.5)

MQUnifiedsensor MQ2_LPG(Board, Voltage_Resolution, ADC_Bit_Resolution, A2, "MQ-2");
MQUnifiedsensor MQ4_Smoke(Board, Voltage_Resolution, ADC_Bit_Resolution, A3, "MQ-4");
MQUnifiedsensor MQ135_CO2(Board, Voltage_Resolution, ADC_Bit_Resolution, A4, "MQ-135");
MQUnifiedsensor MQ135_NH4(Board, Voltage_Resolution, ADC_Bit_Resolution, A4, "MQ-135");
MQUnifiedsensor MQ7_CO(Board, Voltage_Resolution, ADC_Bit_Resolution, A6, "MQ-7");
MQUnifiedsensor MQ7_CH4(Board, Voltage_Resolution, ADC_Bit_Resolution, A6, "MQ-7");

void setup() {
  Serial.begin(9600);

  MQ2_LPG.setRegressionMethod(1);
  MQ2_LPG.setA(574.25);
  MQ2_LPG.setB(-2.222);

  MQ4_Smoke.setRegressionMethod(1);
  MQ4_Smoke.setA(10.00);
  MQ4_Smoke.setB(-0.58);

  MQ135_CO2.setRegressionMethod(1);
  MQ135_CO2.setA(660.00);
  MQ135_CO2.setB(-3.94);

  MQ135_NH4.setRegressionMethod(1);
  MQ135_NH4.setA(15.38);
  MQ135_NH4.setB(-0.49);

  MQ7_CO.setRegressionMethod(1);
  MQ7_CO.setA(99.042);
  MQ7_CO.setB(-1.518);

  MQ7_CH4.setRegressionMethod(1);
  MQ7_CH4.setA(99.042);
  MQ7_CH4.setB(-1.518);

  MQ2_LPG.init();
  MQ4_Smoke.init();
  MQ135_CO2.init();
  MQ135_NH4.init();
  MQ7_CO.init();
  MQ7_CH4.init();

  Serial.print("Calibrating please wait.");
  float calcR0_MQ2 = 0;
  float calcR0_MQ4 = 0;
  float calcR0_MQ135 = 0;
  float calcR0_MQ7_CO = 0;
  float calcR0_MQ7_CH4 = 0;

  for (int i = 1; i <= 10; i++) {
    MQ2_LPG.update();
    MQ4_Smoke.update();
    MQ135_CO2.update();
    MQ135_NH4.update();
    MQ7_CO.update();
    MQ7_CH4.update();

    calcR0_MQ2 += MQ2_LPG.calibrate(RatioMQ2CleanAir);
    calcR0_MQ4 += MQ4_Smoke.calibrate(RatioMQ4CleanAir);
    calcR0_MQ135 += MQ135_CO2.calibrate(RatioMQ135CleanAir);
    calcR0_MQ7_CO += MQ7_CO.calibrate(RatioMQ7CleanAir);
    calcR0_MQ7_CH4 += MQ7_CH4.calibrate(RatioMQ7CleanAir);

    Serial.print(".");
  }

  MQ2_LPG.setR0(calcR0_MQ2 / 10);
  MQ4_Smoke.setR0(calcR0_MQ4 / 10);
  MQ135_CO2.setR0(calcR0_MQ135 / 10);
  MQ7_CO.setR0(calcR0_MQ7_CO / 10);
  MQ7_CH4.setR0(calcR0_MQ7_CH4 / 10);

  Serial.println("  done!.");

  if (isinf(calcR0_MQ2) || isinf(calcR0_MQ4) || isinf(calcR0_MQ135) || isinf(calcR0_MQ7_CO) || isinf(calcR0_MQ7_CH4)) {
    Serial.println("Warning: Connection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1);
  }

  if (calcR0_MQ2 == 0 || calcR0_MQ4 == 0 || calcR0_MQ135 == 0 || calcR0_MQ7_CO == 0 || calcR0_MQ7_CH4 == 0) {
    Serial.println("Warning: Connection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1);
  }

  MQ2_LPG.serialDebug(true);
  MQ4_Smoke.serialDebug(true);
  MQ135_CO2.serialDebug(true);
  MQ135_NH4.serialDebug(true);
  MQ7_CO.serialDebug(true);
  MQ7_CH4.serialDebug(true);
}

void loop() {
  MQ2_LPG.update();
  MQ4_Smoke.update();
  MQ135_CO2.update();
  MQ135_NH4.update();
  MQ7_CO.update();
  MQ7_CH4.update();

  float lpgConcentration = MQ2_LPG.readSensor();
  float smokeConcentration = MQ4_Smoke.readSensor();
  float co2Concentration = MQ135_CO2.readSensor();
  float nh4Concentration = MQ135_NH4.readSensor();
  float coConcentration = MQ7_CO.readSensor();
  float ch4Concentration = MQ7_CH4.readSensor();

  Serial.print("LPG: ");
  Serial.print(lpgConcentration);
  Serial.print(" ppm   Smoke: ");
  Serial.print(smokeConcentration);
  Serial.print(" ppm   CO2: ");
  Serial.print(co2Concentration);
  Serial.print(" ppm   NH4: ");
  Serial.print(nh4Concentration);
  Serial.print(" ppm   CO: ");
  Serial.print(coConcentration);
  Serial.print(" ppm   CH4: ");
  Serial.print(ch4Concentration);
  Serial.println(" ppm\n");

  delay(1000);
}
