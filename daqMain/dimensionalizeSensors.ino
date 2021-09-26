#define strainGF 2.14
#define systemVoltage 5
#define adsStrainMultiplier 0.0078125F
#define adsADCMultiplier 0.1875F
#define megaADCMultiplier 4.9F
#define brakeTempMultiplier 200
#define brakeTempOffsetV 0.5
#define steeringAngleMinV 0.5
#define steeringAngleMaxV 4.5
#define steeringAngleMin 0
#define steeringAngleMax 180

//#define wheelCirc 3.24*2*8

float dimensionalizeStrainGuage(float raw, float offset = 0) {
  return ((1000 * 4 * raw * adsStrainMultiplier) / (strainGF * systemVoltage)) - offset; //units: microstrain
}

float dimensionalizeAdsADC(float raw, float offset = 0) {
  return (raw * adsADCMultiplier) - offset; //units: mv
}

float dimensionalizeMegaADC(float raw, float offset = 0) {
  return (raw * megaADCMultiplier) - offset; //units: mv
}

float dimensionalizeBrakeTemp(float raw, float offset = 0) {
  return ((dimensionalizeMegaADC(raw)/1000 - brakeTempOffsetV) * brakeTempMultiplier); //units: C
}

float dimensionalizeBrakePress(float raw, float offset = 0) {
  return map(dimensionalizeMegaADC(raw)/1000, 0.5, 4.5, 0, 1000); //units: PSI
}

float dimensionalizeSteeringAngle(float raw, float offset = 0) {
  return map(dimensionalizeMegaADC(raw)/1000, steeringAngleMinV, steeringAngleMaxV, steeringAngleMin, steeringAngleMax); //units: deg
}
