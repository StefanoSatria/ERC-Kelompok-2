#include "mpu9250.h"
bfs::Mpu9250 imu;

//define pins
const int led = 13;
const int buzzer = 3;
const int trigPin = 6;
const int echoPin = 7;

//define variables
float ax, ay, az;
int xsample = 0;
int ysample = 0;
int zsample = 0;
long duration;
int distance;

//define limits and sample
#define samples 50
#define maxVal 20 // max change limit
#define minVal - 20 // min change limit

void setup() {
  Serial.begin(9600);
  initializationIMU();
  initializationPINOUT();
  initializationUSS();
}

void loop() {
  // Check if data read 
  if (imu.Read()) {
    int value1 = imu.accel_x_mps2(); // reading x out
    int value2 = imu.accel_y_mps2(); //reading y out
    int value3 = imu.accel_z_mps2(); //reading z out
    int xValue = xsample - value1; // finding change in x
    int yValue = ysample - value2; // finding change in y
    int zValue = zsample - value3;//finding change in z
    delay(100);

    //sending values to processing IMU
    Serial.println("Sensor Gempa: ");
    Serial.print("x=");
    Serial.println(xValue);
    Serial.print("y=");
    Serial.println(yValue);
    Serial.print("z=");
    Serial.println(zValue);

    //process USS
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    Serial.println("Sensor Banjir: ");
    Serial.print("Distance= ");
    Serial.println(distance);
    Serial.print("\n");


    //condition check and output
    if (xValue < minVal || xValue > maxVal || yValue < minVal || yValue > maxVal || zValue < minVal || zValue > maxVal) {
        Serial.print("Terjadi Gempa");
        signalon();
    } 
    if (xValue > minVal && xValue < maxVal && yValue > minVal && yValue < maxVal && zValue > minVal && zValue < maxVal) {
        Serial.print("Tidak Terjadi Gempa");
        signaloff();
    }
    if (distance < 10) {
        Serial.println(", Terjadi Banjir");
        signalon();
    }

  if (distance > 10) {
        Serial.println(", Tidak Terjadi Banjir");
        signaloff();
    }
  }
  Serial.print("\n");
  Serial.print("\n");
delay(50);
}

void signalon() {
  digitalWrite(led, HIGH);
  tone(buzzer, 1000, 5000);
  delay(1000);
}

void signaloff() {
  digitalWrite(led, LOW);
  noTone(buzzer);
}

void initializationIMU() {
  while (!Serial) {}
  Wire.begin();
  Wire.setClock(400000);
  imu.Config( & Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while (1) {}
  }
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while (1) {}
  }

  for (int i = 0; i < samples; i++) // taking samples for calibration
  {
    xsample += imu.accel_x_mps2();
    ysample += imu.accel_y_mps2();
    zsample += imu.accel_z_mps2();
  }
  xsample /= samples; // taking avg for x
  ysample /= samples; // taking avg for y
  zsample /= samples; // taking avg for z
}

void initializationPINOUT() {
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void initializationUSS() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
