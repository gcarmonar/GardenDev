//--- Libraries ------------------------------------------------------------------
#include <SparkFun_RHT03.h>

//--- I/O Pinss ------------------------------------------------------------------
#define MOIST_SENSOR_A_PIN  A0
#define TEMP_SENSOR_A_PIN   A1
#define LIGHT_SENSOR_A_PIN  A2
#define MOIST_SENSOR_B_PIN  A3
#define TEMP_SENSOR_B_PIN   A4
#define LIGHT_SENSOR_B_PIN  A5
#define HIGH_TEMP_LED_PIN   5
#define LOW_TEMP_LED_PIN    6
#define RHT03_DATA_PIN      2

//--- Constants ------------------------------------------------------------------
#define NUMBER_SAMPLES      5       //One minute / 5 seconds each sample
#define TASK_50_MS          50
#define TASK_1_SEC          1000
#define TASK_5_SEC          1000*1  //Sample time
#define TASK_1_MIN          1000*5 //Send data to display and Raspberry Pi
#define HIGH_TEMP_ALERT     300     //Celcious*10
#define LOW_TEMP_ALERT      150     //Celcious*10
#define LOW_MOIST_ALERT     500     //Calibrated
#define LOW_HUM_ALERT       10      //in %

//--- Objects --------------------------------------------------------------------
RHT03 rht;

//--- Variables ------------------------------------------------------------------
unsigned long task_50_ms =  0;
unsigned long task_1_sec =  0;
unsigned long task_5_sec =  0;
unsigned long task_1_min =  0;

int task50msFlag = 0;
int task1secFlag = 0;
int task5secFlag = 0;
int task1minFlag = 0;
int sample =       0;
int humFail =      0;

int tempArrayA[NUMBER_SAMPLES] =  {};
int moistArrayA[NUMBER_SAMPLES] = {};
int lightArrayA[NUMBER_SAMPLES] = {};
int tempArrayB[NUMBER_SAMPLES] =  {};
int moistArrayB[NUMBER_SAMPLES] = {};
int lightArrayB[NUMBER_SAMPLES] = {};
int humArray[NUMBER_SAMPLES] =    {};
int airTempArray[NUMBER_SAMPLES] ={};


//--- Prototypes -----------------------------------------------------------------
void Task50ms();
void Task1sec();
void Task5sec();
void Task1min();
void ReadTemp();
void ReadMoist();
void ReadLight();
void ReadHum();
//void SendData(int sensors[]);
int GetAverage(int data[]);
void StatusLeds(int tempStatus, int moistStatus, boolean humStatus);

//--- Setup ----------------------------------------------------------------------
void setup(){
  Serial.begin(9600);
  pinMode(HIGH_TEMP_LED_PIN, OUTPUT);
  pinMode(LOW_TEMP_LED_PIN, OUTPUT);
  rht.begin(RHT03_DATA_PIN);
}

//--- Main Loop ------------------------------------------------------------------
void loop(){
  
  if (TASK_50_MS <= (millis() - task_50_ms)){
    task50msFlag = 1;
    task_50_ms = millis();
  }

  if (TASK_1_SEC <= (millis() - task_1_sec)){
    task1secFlag = 1;
    task_1_sec = millis();
  }

  if (TASK_5_SEC <= (millis() - task_5_sec)){
    task5secFlag = 1;
    task_5_sec = millis();
  }

  if (TASK_1_MIN <= (millis()- task_1_min)){
    task1minFlag = 1;
    task_1_min = millis();
  }

  if (task50msFlag){
    Task50ms();
    task50msFlag = 0;
  }

  if (task1secFlag){
    Task1sec();
    task1secFlag = 0;
  }

  if (task5secFlag){
    Task5sec();
    task5secFlag = 0;
  }

  if (task1minFlag){
    Task1min();
    task1minFlag = 0;
  }

}

//--- Functions ------------------------------------------------------------------
void Task50ms(){
  
}


void Task1sec(){
//  int temp = GetAverage(tempArray);
//  int moist = GetAverage(moistArray);
//  int light = GetAverage(lightArray);
//
//  Serial.print("Temp [C]: ");
//  Serial.println(temp);
//  Serial.print("Moisture: ");
//  Serial.println(moist);
//  Serial.print("Light: ");
//  Serial.println(light);
//  Serial.println("-----------------");

  //ReadHum();

}

void Task5sec(){
  if (sample == NUMBER_SAMPLES){
    sample = 0;
  }
  ReadMoist();
  ReadTemp();
  ReadLight();
  sample++;
}

void Task1min(){
  int tempStatus;
  int moistStatus;
  boolean humStatus;
  int tempA = GetAverage(tempArrayA);
  int moistA = GetAverage(moistArrayA);
  int lightA = GetAverage(lightArrayA);
  int tempB = GetAverage(tempArrayB);
  int moistB = GetAverage(moistArrayB);
  int lightB = GetAverage(lightArrayB);
  int hum = GetAverage(humArray);
  int airTemp = GetAverage(airTempArray);
  
  //Tell Raspberry Pi we have data
  Serial.println("D");
  //Data in pot A
  Serial.print("ta");
  Serial.print(tempA);
  Serial.print(" ma");
  Serial.print(moistA);
  Serial.print(" la");
  Serial.print(lightA);
  //Data in pot B
  Serial.print(" tb");
  Serial.print(tempB);
  Serial.print(" mb");
  Serial.print(moistB);
  Serial.print(" lb");
  Serial.print(lightB);
  //Data Humidity and ambient temp
  Serial.print(" hh");
  Serial.print(hum);
  Serial.print(" tt");
  Serial.print(airTemp);
  //End of transmission
  Serial.println("E");
  //Alerts calculations
  tempStatus = 0;
  tempStatus = ((tempA >= HIGH_TEMP_ALERT)<<3)&0x8 | ((tempA <= LOW_TEMP_ALERT)<<2)&0x4
               | ((tempB >= HIGH_TEMP_ALERT)<<1)&0x2 | (tempB <= LOW_TEMP_ALERT)&0x1;
  moistStatus = ((moistA <= LOW_MOIST_ALERT)<<1)&0x2 | (moistB <= LOW_MOIST_ALERT)&0x1;
  humStatus = (hum <= LOW_HUM_ALERT);
  Serial.println(tempStatus);
  StatusLeds(tempStatus, moistStatus, humStatus);
}

void ReadTemp(){
  int input = analogRead(TEMP_SENSOR_A_PIN);
  int temp = (((float)input*4.88) - 500.0);
  tempArrayA[sample] = temp;

  input = analogRead(TEMP_SENSOR_B_PIN);
  temp = (((float)input*4.88) - 500.0);
  tempArrayB[sample] = temp;
}

void ReadLight(){
  int input = analogRead(LIGHT_SENSOR_A_PIN);
  lightArrayA[sample] = input;

  input = analogRead(LIGHT_SENSOR_B_PIN);
  lightArrayB[sample] = input;
}

void ReadMoist(){
  int input = analogRead(MOIST_SENSOR_A_PIN);
  moistArrayA[sample] = input;

  input = analogRead(MOIST_SENSOR_B_PIN);
  moistArrayB[sample] = input;
}


int GetAverage(int data[]){
  int avg=0;

  for (int i = 0; i < NUMBER_SAMPLES; i++){
    avg += data[i];
  }

  avg = avg / NUMBER_SAMPLES;

  return avg;
}

void StatusLeds(int tempStatus, int moistStatus, boolean humStatus){
  switch (tempStatus){
    case 0:
      digitalWrite(LOW_TEMP_LED_PIN, LOW);
      digitalWrite(HIGH_TEMP_LED_PIN, LOW);
      break;
    case 1:
    case 4:
    case 5:
      digitalWrite(LOW_TEMP_LED_PIN, HIGH);
      digitalWrite(HIGH_TEMP_LED_PIN, LOW);
      break;
    case 2:
    case 8:
    case 10:
      digitalWrite(LOW_TEMP_LED_PIN, LOW);
      digitalWrite(HIGH_TEMP_LED_PIN, HIGH);
      break;
    default:
      digitalWrite(LOW_TEMP_LED_PIN, HIGH);
      digitalWrite(HIGH_TEMP_LED_PIN, HIGH);
  }
}

void ReadHum(){
  int updateRet = rht.update();

  if (updateRet == 1){
    // The humidity(), tempC(), and tempF() functions can be called -- after 
    // a successful update() -- to get the last humidity and temperature
    // value 
    float latestHumidity = rht.humidity();
    float latestTempC = rht.tempC();
    
    // Now print the values:
    Serial.println("Humidity: " + String(latestHumidity, 1) + " %");
    Serial.println("Temp (C): " + String(latestTempC, 1) + " deg C");

    humArray[NUMBER_SAMPLES] = (int)(latestHumidity*10);
    airTempArray[NUMBER_SAMPLES] = (int)(latestTempC);
    
    humFail=0;
  }else{
    humFail++;
    Serial.println("Sensor fail!");
    delay(1000);
  }
}

