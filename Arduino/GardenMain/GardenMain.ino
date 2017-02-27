//--- Libraries ------------------------------------------------------------------
#include <SparkFun_RHT03.h>
#include <LiquidCrystal.h>

//--- I/O Pinss ------------------------------------------------------------------
// G_ is for ground, A_ is for ambient
#define G_MOIST_DATA_PIN    A0
#define G_TEMP_DATA_PIN     A1
#define A_LIGHT_DATA_PIN    A2
#define A_TEMP_DATA_PIN     A3
#define G_MOIST_VCC_PIN     12
#define A_RHT03_DATA_PIN    11
// LCD pins:
//   * LCD RS pin to digital pin 10
//  * LCD Enable pin to digital pin 9
//  * LCD D4 pin to digital pin 8
//  * LCD D5 pin to digital pin 7
//  * LCD D6 pin to digital pin 6
//  * LCD D7 pin to digital pin 5
//  * LCD R/W pin to ground

//--- Constants ------------------------------------------------------------------
#define NUMBER_SAMPLES      12      //One minute / 5 seconds each sample
#define TASK_5_SEC          1000*1  //Sample time
#define TASK_1_MIN          1000*5  //Send data to display and Raspberry Pi
#define HIGH_TEMP_ALERT     300     //Celcious*10
#define LOW_TEMP_ALERT      150     //Celcious*10
#define LOW_MOIST_ALERT     500     //Calibrated
#define LOW_HUM_ALERT       10      //in %
// This is like a counter. If for 12 hours the light is low, show alert
#define LOW_LIGHT_COUNTER   720     // 1 min * 60 min/hour * 12 hour 
#define LOW_LIGHT_ALERT     500     //Calibrated

//--- Objects --------------------------------------------------------------------
RHT03 rht;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//--- Variables ------------------------------------------------------------------
unsigned long task_5_sec =  0;
unsigned long task_1_min =  0;

int task5secFlag = 0;
int task1minFlag = 0;
int sample =       0;
int humFail =      0;

int gTempArray[NUMBER_SAMPLES] =  {};
int gmoistArray[NUMBER_SAMPLES] = {};
int aLightArray[NUMBER_SAMPLES] = {};
int aTempArray[NUMBER_SAMPLES] =  {};
int aHumArray[NUMBER_SAMPLES] =    {};

//--- Prototypes -----------------------------------------------------------------
void Task5sec();
void Task1min();

void ReadTemp();
void ReadMoist();
void ReadLight();
void ReadHum();

int GetAverage(int data[]);
void PrintLCD();
void ProcessData();

//--- Setup ----------------------------------------------------------------------
void setup(){
  Serial.begin(9600);
  pinMode(G_MOIST_VCC_PIN, OUTPUT);
  rht.begin(A_RHT03_DATA_PIN);
  lcd.begin(20, 4);
}

//--- Main Loop ------------------------------------------------------------------
void loop(){

  if (TASK_5_SEC <= (millis() - task_5_sec)){
    task5secFlag = 1;
    task_5_sec = millis();
  }

  if (TASK_1_MIN <= (millis()- task_1_min)){
    task1minFlag = 1;
    task_1_min = millis();
  }

  if (task5secFlag){
    Task5sec();
    task5secFlag = 0;
  }

  if (task1minFlag){
    Task1min();
    task1minFlag = 0;
  }
  void ReadHum();
  delay(1000);

}

//--- Functions ------------------------------------------------------------------

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
  int gTempe = GetAverage(gTempArray);
  int gMoist = GetAverage(gmoistArray);
  int aLight = GetAverage(aLightArray);
  int aTempe = GetAverage(aTempArray);

  // ProcessData();
  // UpdateDisplay();
  // SendData();



  //int aHumid = GetAverage(aHumArray);
  
  // //Tell Raspberry Pi we have data
  // Serial.println("D");
  // //Data in pot A
  // Serial.print("ta");
  // Serial.print(tempA);
  // Serial.print(" ma");
  // Serial.print(moistA);
  // Serial.print(" la");
  // Serial.print(lightA);
  // //Data in pot B
  // Serial.print(" tb");
  // Serial.print(tempB);
  // Serial.print(" mb");
  // Serial.print(moistB);
  // Serial.print(" lb");
  // Serial.print(lightB);
  // //Data Humidity and ambient temp
  // Serial.print(" hh");
  // Serial.print(hum);
  // Serial.print(" tt");
  // Serial.print(airTemp);
  // //End of transmission
  // Serial.println("E");

}

void ReadTemp(){
  int input, temp;

  input = analogRead(G_TEMP_DATA_PIN);
  temp = (((float)input*4.88) - 500.0);
  gTempArray[sample] = temp;
  
  input = analogRead(A_TEMP_DATA_PIN);
  temp = (((float)input*4.88) - 500.0);
  aTempArray[sample] = temp;
}

void ReadLight(){
  int input = analogRead(A_LIGHT_DATA_PIN);
  aLightArray[sample] = input;
}

void ReadMoist(){
  int input = analogRead(G_MOIST_DATA_PIN);
  gmoistArray[sample] = input;
}


int GetAverage(int data[]){
  int avg=0;

  for (int i = 0; i < NUMBER_SAMPLES; i++){
    avg += data[i];
  }

  avg = avg / NUMBER_SAMPLES;

  return avg;
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

    aHumArray[NUMBER_SAMPLES] = (int)(latestHumidity*10);
    //airTempArray[NUMBER_SAMPLES] = (int)(latestTempC);
    
    humFail=0;
  }else{
    humFail++;
    Serial.println("Sensor fail!");
    delay(1000);
  }
}

