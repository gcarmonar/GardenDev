// http://beningo.com/videos/

//--- Libraries ------------------------------------------------------------------
#include <SparkFun_RHT03.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

//--- Objects --------------------------------------------------------------------
RHT03 rht;
SemaphoreHandle_t xSerialSemaphore;
SemaphoreHandle_t xDataSensorsSemaphore;

//--- Tasks ----------------------------------------------------------------------
void TaskReadSensors( void *pvParameters );
void TaskPrintLCD( void *pvParameters );
void TaskSendSerial( void *pvParameters );
void TaskDebug( void *pvParameters );

//--- I/O Pins -------------------------------------------------------------------
#define G_MOIST_SENSOR_A_PIN  A0
#define G_TEMP_SENSOR_A_PIN   A1
#define G_MOIST_SENSOR_B_PIN  A3
#define G_TEMP_SENSOR_B_PIN   A4
#define A_LIGHT_SENSOR_PIN    A2
#define A_TEMP_SENSOR_PIN     A5
#define ALERT_LED_PIN         13
#define RHT03_DATA_PIN        12
#define LCD_BUTTON_PIN        11
#define G_MOIST_ENABLE_B_PIN  10
#define G_MOIST_ENABLE_A_PIN  9
// LCD Pins:
// D4-D7: Pin 4 to 7
// EN: Pin 2
// RS: Pin 3

//--- Constants ------------------------------------------------------------------
#define	DEBUG 				true	
#define NUMBER_SAMPLES      60/5    //Num of samples to remember (filtering)

// Alerts Parameters
#define HIGH_TEMP_ALERT     300     //Celcious*10
#define LOW_TEMP_ALERT      150     //Celcious*10
#define LOW_MOIST_ALERT     500     //Calibrated
#define LOW_HUM_ALERT       10      //in %
#define LOW_LIGHT_ALERT     500     //Calibrated
#define LOW_LIGHT_COUNT     1*60*10 //Times that we meassured low light (=10 h)



//--- Setup ----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Semaphores. For data 
  if ( xSerialSemaphore == NULL )  
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  
  }

  if ( xDataSensorsSemaphore == NULL )  
  {
    xDataSensorsSemaphore = xSemaphoreCreateMutex();  
    if ( ( xDataSensorsSemaphore ) != NULL )
      xSemaphoreGive( ( xDataSensorsSemaphore ) );  
  }




  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskReadSensors
    ,  (const portCHAR *)"ReadSensors"  // A name just for humans
    ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskPrintLCD
    ,  (const portCHAR *) "PrintLCD"
    ,  256  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskReadSensors( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  int tempArrayA[NUMBER_SAMPLES]  = {};
  int moistArrayA[NUMBER_SAMPLES] = {};
  int lightArrayA[NUMBER_SAMPLES] = {};
  int tempArrayB[NUMBER_SAMPLES]  = {};
  int moistArrayB[NUMBER_SAMPLES] = {};
  int lightArrayB[NUMBER_SAMPLES] = {};
  int humArray[NUMBER_SAMPLES]    = {};
  int airTempArray[NUMBER_SAMPLES]= {};

  // digital pin 2 has a pushbutton attached to it. Give it a name:
  uint8_t pushButton = 2;

  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);

  for (;;) // A Task shall never return or exit.
  {
    // read the input pin:
    int buttonState = digitalRead(pushButton);

    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the state of the button:
      Serial.println(buttonState);

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskAnalogRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);

    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the value you read:
      Serial.println(sensorValue);

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}