//#include "test.h"

// !!! dient reinen Testzwecken, keine Anwendungsbeispiele !!!
void tests(){
//	  // --- Test calcDistance
//	  coord_t A = {50.918408,11.568755}; // EAH
//	  coord_t B = {52.516277,13.377703}; // Brandenburger-Tor
//	  float dist = calcOrthodromicDistance(A, B); // Soll = 217,07km; Ist = 217,25km -> Fehler +0,18km (+0,0008%)
//	  coord_t A2 = {50.918408,11.568755}; // EAH
//	  coord_t B2 = {50.923765,11.584027}; // Paradiesbahnhof
//	  float dist2 = calcOrthodromicDistance(A2, B2); // Soll = 1,23km; Ist = 1,226km -> Fehler -0,003km (-0,002%)
//	  coord_t A3 = {50.918408,11.568755}; // EAH
//	  coord_t B3 = {50.918337,11.569486}; // EAH andere Straßenseite
//	  float dist3 = calcOrthodromicDistance(A3, B3); // Soll = 0,05km; Ist = 0,0518km -> Fehler -0,002km (-0,002%)
//	  coord_t A4 = {50.918408,11.568755}; // EAH
//	  coord_t B4 = {50.920582,11.569400}; //Netto Parkplatz
//	  float dist4 = calcOrthodromicDistance(A4, B4);
//	  // --> iO...
//
//	  // --- Test calcBearing
//	  float bearing = calcBearing(A,B); // Soll = 34,34°; Ist = 34,338° (NordOst) -> Fehler -0,002°
//	  //float angle = calcAngle(A,B); // soll = 34,34°; ist = 41,45°
//	  B.lat = 52.593038;
//	  B.lon = 10.645490;
//	  float bearing2 = calcBearing(A,B); // Soll = 341,52°; Ist = 341,517° (NordWest)
//	  B.lat = 49.933544;
//	  B.lon = 9.354535;
//	  float bearing3 = calcBearing(A,B); // Soll = 235,94°; Ist = 235,935° (SuedWest)
//	  B.lat = 49.975955;
//	  B.lon = 12.273984;
//	  float bearing4 = calcBearing(A,B); // Soll = 154,25°; Ist = 154,249° (SuedOst)
//	  B.lat = 49.933544;
//	  B.lon = A.lon;
//	  float bearing5 = calcBearing(A,B); // Soll = 180°; Ist = 180° (S)
//	  float tmp = B.lat;
//	  B.lat = A.lat;
//	  A.lat = tmp;
//	  float bearing6 = calcBearing(A,B); // Soll = 0°; Ist = 0° (N)
//	  // --> iO
//
//	  // --- Test mean
//	  uint32_t vals[8] = {1,2,3,4,5,6,7,8};
//	  float mean = calcMean(vals, sizeof(vals)/sizeof(vals[0])); // soll = 4,5; ist = 4,5
//	  // --> iO
//
//	  // Test com
//	  uint8_t arrTest[6];
//	  status stat = readData(busaddr_coordA, arrTest, 6);
//
//	  // brownout
//	  reset_cause_t cause = getResetCause();
//
//	  // pid
//	  controller ctrlVel;
//	  ctrlVel.eold = 0;
//	  ctrlVel.esum = 0;
//	  ctrlVel.esum_limit = 100;
//	  ctrlVel.kd = 0.002;
//	  ctrlVel.ki = 0.005;
//	  ctrlVel.kp = 0.01;
//	  ctrlVel.ta = 1;
//	  ctrlVel.y_max = 100;
//	  ctrlVel.y_min = -ctrlVel.y_max;
//	  float y_thrust = pid_controll(&ctrlVel, 10, 9);
//
//	  // crc
//	  uint8_t testData[] = { 0x22,0x33,0x44,0x55 };
//	  uint8_t testRes = CRC8(&testData, sizeof(testData)); // -> i.O.
//
//	  // tp
//	  coord_t cA = {50.918398,11.567270};
//	  coord_t cB = {50.918370,11.569480};
//	  coord_t cCP = {50.918113,11.568163};
//	  // coord_t cD = {50.918387,11.568161}; Projektion CP auf AB
//	  // error sollte 0,23km sein
//	  coord_t cTP = calcTargetpoint(cA, cB, cCP);
//
//
//	  uint8_t breakP = 123;
}
