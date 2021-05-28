#define strainGF 2.14
#define systemVoltage 5
#define adsStrainMultiplier 0.0078125F
#define adsADCMultiplier 0.1875F
#define megaADCMultiplier 4.9F
#define brakeTempMultiplier 200
#define brakeTempOffsetV 0.5

//#define wheelCirc 3.24*2*8

float dimensionalizeStrainGuage(float raw, float offset = 0) {
  return ((1000 * 4 * raw * adsStrainMultiplier) / (strainGF * systemVoltage)) + offset; //units: microstrain
}

float dimensionalizeAdsADC(float raw, float offset = 0) {
  return (raw * adsADCMultiplier) + offset; //units: mv
}

float dimensionalizeMegaADC(float raw, float offset = 0) {
  return (raw * megaADCMultiplier) + offset; //units: mv
}

float dimensionalizeBrakeTemp(float raw, float offset = 0) {
  return ((dimensionalizeMegaADC(raw) - brakeTempOffsetV) * brakeTempMultiplier); //units: C
}
