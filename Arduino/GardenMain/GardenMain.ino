#define MOIST_PIN         A0
#define TEMP_PIN          A1
#define LIGHT_PIN         A2
#define HIGH_TEMP_LED_PIN 6
#define LOW_TEMP_LED_PIN  5

#define NUMBER_SAMPLES    10 //One minute / 5 seconds each sample
#define TASK_50_MS        50
#define TASK_500_MS       500
#define TASK_5_SEC        1000*5  //Sample time
#define TASK_1_MIN        1000*60 //Send data to display and Raspberry Pi
#define HIGH_TEMP         225
#define LOW_TEMP          210

unsigned long task_50_ms =  0;
unsigned long task_500_ms = 0;

int task50msFlag =          0;
int task500msFlag =         0;
int sample =                0;

int tempArray[NUMBER_SAMPLES] =   {};
int moistArray[NUMBER_SAMPLES] =  {};
int lightArray[NUMBER_SAMPLES] =  {};




void Task50ms();
void Task500ms();
void ReadTemp();
void ReadMoist();
void ReadLight();
int GetAverage(int data[]);


void setup(){
  Serial.begin(9600);
  pinMode(HIGH_TEMP_LED_PIN, OUTPUT);
  pinMode(LOW_TEMP_LED_PIN, OUTPUT);
}

void loop(){
  
  if (TASK_50_MS <= (millis()-task_50_ms)){
    task50msFlag = 1;
    task_50_ms = millis();
  }

  if (TASK_500_MS <= (millis()- task_500_ms)){
    task500msFlag = 1;
    task_500_ms = millis();
  }

  if (task50msFlag){
    Task50ms();
    task50msFlag = 0;
  }

  if (task500msFlag){
    Task500ms();
    task500msFlag = 0;
  }

}


void Task50ms(){
  if (sample == NUMBER_SAMPLES){
    sample = 0;
  }
  ReadMoist();
  ReadTemp();
  ReadLight();
  sample++;
}


void Task500ms(){
  int temp = GetAverage(tempArray);
  int moist = GetAverage(moistArray);
  int light = GetAverage(lightArray);

//  Serial.print("Temp [C]: ");
//  Serial.println(temp);
//  Serial.print("Moisture: ");
//  Serial.println(moist);
//  Serial.print("Light: ");
//  Serial.println(light);
//  Serial.println("-----------------");

  Serial.println("D");
  Serial.print("ta");
  Serial.print(temp);
  Serial.print(" ma");
  Serial.print(moist);
  Serial.print(" la");
  Serial.print(light);
  Serial.println("E");



  StatusLeds(temp, moist, light);
}


void ReadTemp(){
  int input = analogRead(TEMP_PIN);
//  Serial.print("RAW: ");
//  Serial.println(input);
  int temp = (((float)input*4.88) - 500.0);

  tempArray[sample] = temp;
}


void ReadLight(){
  int input = analogRead(LIGHT_PIN);
  lightArray[sample] = input;
}

void ReadMoist(){
  int input = analogRead(MOIST_PIN);
  input = 0;
  moistArray[sample] = input;
}


int GetAverage(int data[]){
  int avg=0;

  for (int i = 0; i < NUMBER_SAMPLES; i++){
    avg += data[i];
  }

  avg = avg / NUMBER_SAMPLES;

  return avg;
}

void StatusLeds(int t, int m, int l){
  if (t <= LOW_TEMP){
    digitalWrite(LOW_TEMP_LED_PIN, HIGH);
    digitalWrite(HIGH_TEMP_LED_PIN, LOW);
  }else if (t >= HIGH_TEMP){
    digitalWrite(LOW_TEMP_LED_PIN, LOW);
    digitalWrite(HIGH_TEMP_LED_PIN, HIGH);
  }else{
    digitalWrite(LOW_TEMP_LED_PIN, LOW);
    digitalWrite(HIGH_TEMP_LED_PIN, LOW);
  }

}
