#include <Arduino_SensorKit.h>
#include <Arduino_SensorKit_BMP280.h>
#include <Arduino_SensorKit_LIS3DHTR.h>

#define Environment Environment_I2C

#define BUZZER 5
#define BUTTON 2
#define LED 6
#define POT A0
#define MIC A2
#define LIGHT A3

int pot_value;
int button_state = 0;
int mic_value;
int light_value;

float temperature = 0;
float humidity = 0;
float pressure = 0;
float height = 0;

enum State {
    Default,
    TempHum,
    PRES,
    STATE3
};
State currentState = Default;
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);

  pinMode(MIC , INPUT);
  pinMode(LIGHT , INPUT);
  pinMode(BUTTON , INPUT_PULLUP);
  //pinMode(POT , INPUT_PULLUP);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUZZER, OUTPUT);
  
  Environment.begin();

  Oled.begin();
  Oled.setFlipMode(true);

  Accelerometer.begin();
  Pressure.begin();

  temperature = Environment.readTemperature();
  humidity = Environment.readHumidity();
  pressure = Pressure.readPressure();
  height = Pressure.readAltitude();
  //attachInterrupt(digitalPinToInterrupt(BUTTON), attachInterrupt1, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(POT), attachInterrupt2, CHANGE);
}

void loop() {
  Oled.setFont(u8x8_font_amstrad_cpc_extended_r); 

  CheckCurrentState();
}

void attachInterrupt1(){
  if(digitalRead(BUTTON) == 1){
    button_state++;
    Serial.println(button_state);
  }
}

void CheckCurrentState(){
  bool onChange = false;
  int k = analogRead(POT);
  if (k <= 1025 && k > 800){
    onChange = currentState != Default;
    currentState = Default;
  }
  else if (k <= 800 && k > 600){
    onChange = currentState != TempHum;
    currentState = TempHum;
  }
  else if (k <= 600 && k > 400){
    onChange = currentState != PRES;
    currentState = PRES;
  }
  
  if (onChange){
    tone(BUZZER, 100);
    Oled.clearDisplay();
    delay(50);
    noTone(BUZZER);
    SwitchState();
  }
  else{
    SwitchState();
    delay(100);
  }
}

void SwitchState(){
  switch(currentState){
    case Default:
      DefaultRefresh();
      break;
    case TempHum:
      TempRefresh();
      break;
    case PRES:
      PressureRefresh();
      break;
  }
}
void PressureRefresh(){
  pressure = (pressure *9 + Pressure.readPressure())/10;
  //height = (height *19 + Pressure.readAltitude())/20;
  height = height - (height - Pressure.readAltitude())/20;
  Oled.setCursor(0, 1);
  Oled.println("Air Pressure:");
  Oled.println(pressure,0);
  Oled.setCursor(0, 3);
  Oled.println("Height:");
  Oled.println(height,2);
}
void DefaultRefresh(){
  Oled.setCursor(0, 3);
  Oled.println("Hello!");
  
  Oled.println("");
}
void TempRefresh(){
  temperature = (temperature *2 + Environment.readTemperature())/3;
  humidity = (humidity*9 + Environment.readHumidity())/10;

  Oled.setCursor(0, 1);
  Oled.println("Temperature:");
  Oled.print(temperature,2); //print temperature
  Oled.println(" C");
  Oled.setCursor(0, 3);
  Oled.println("Humidity:");
  Oled.print(humidity,1); //print humidity
  Oled.println(" %");
}
