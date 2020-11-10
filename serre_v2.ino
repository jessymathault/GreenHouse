
#include <Time.h>
#include <TimeAlarms.h>
//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
//******************************************************
//Humidity and air temp sensor
#include "DHT.h"


/*******************************************************




********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
    // define some values used by the panel and buttons
    int lcd_key     = 0;
    int adc_key_in  = 0;
    #define btnRIGHT  0
    #define btnUP     1
    #define btnDOWN   2
    #define btnLEFT   3
    #define btnSELECT 4
    #define btnNONE   5
    
    #define maxIdleMenuPos 7
    #define maxMainMenuPos 2
    #define maxSubMenu1Pos 12
    #define maxSubMenu2Pos 6
  
    #define debounce 200
 
    char titleStr[ ] = "Ju' Plants ";
    boolean idleMenu = 1; //determines if playing in menu or idle
    int menuPosition = 0; // Main menu option position
    int idleMenuPosition = 0; //
    int subMenuPosition = 0;


// FAN - -----------------------------------------------------------
    #define fanPinV1 22
    #define fanPinV2 23
    #define timeToControlFan 300
    byte fanState = 0;
    byte fanTresh1 = 40;
    byte fanTresh2 = 85;
    byte fanTempTresh = 25;
    


// AIR SENSOR ------------------------------------------------------
    #define DHTTYPE DHT11   // DHT 11
    #define DHTPIN 14 // air temp and hum sensor
    #define timeToMeasureAir 15
    float airTemp = 0; // air sensor
    float airHumidity = 0; 
    DHT dht(DHTPIN, DHTTYPE);

    

// Ground Sensor ---------------------------------------------------
    
    #define timeBetweenMeasure 600
    float p1Hum = 0;
    float p2Hum = 100;
    float p3Hum = 65;
    float p4Hum = 87;
    float p5Hum = 5;
    float p6Hum = 68;
    float p7Hum = 0;
    float p8Hum = 0;

    int maxCh1 = 500;
    int maxCh2 = 500;
    int maxCh3 = 500;
    int maxCh4 = 500;
    int maxCh5 = 500;
    int maxCh6 = 500;
    int maxCh7 = 500;
    int maxCh8 = 500;

    float minAllCh = 15;

    boolean activateMeasureGroundp2 = false;

    boolean activateSetMeasurep2 = false;

    #define groundMeasureEnablePin 28

    #define flashingLEDPin 26

    #define flashPeriod 2

    boolean humidityLEDState = true;
        
// Light -----------------------------------------------------------
    #define hourLightOn 7
    #define hourLightOff 21
    #define LightPin 24
    boolean lightState = 0;


  





// ----------------------------------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------------------------------
void setup()
{
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print(titleStr); // print a simple message
 
 pinMode(fanPinV1, OUTPUT);
 pinMode(fanPinV2, OUTPUT);
 pinMode(groundMeasureEnablePin, OUTPUT);
 digitalWrite(groundMeasureEnablePin, HIGH);
 pinMode(flashingLEDPin, OUTPUT);
 digitalWrite(flashingLEDPin, HIGH);
 digitalWrite(fanPinV1, LOW);
 digitalWrite(fanPinV2, LOW);
 
 pinMode(A8, INPUT);
 pinMode(A9, INPUT);
 pinMode(A10, INPUT);
 pinMode(A11, INPUT);
 pinMode(A12, INPUT);
 pinMode(A13, INPUT);
 pinMode(A14, INPUT);
 pinMode(A15, INPUT);
 
 pinMode(LightPin, OUTPUT);
 digitalWrite(LightPin, LOW);
 
 
 dht.begin();
 

 setTime(0,0,0,12,23,15); //
  
  // light
  
  setAlarms();
   
}

void setAlarms()
{
  Alarm.alarmRepeat(hourLightOn,0,0, turnOnLight);  // 8:30am every day
  Alarm.alarmRepeat(hourLightOff,0,0,turnOffLight);  // 9:30pm every day 
 
  Alarm.timerRepeat(timeToMeasureAir, measureAirTempAndHum); // mesure air    
  Alarm.timerRepeat(timeToControlFan, automaticFanControl); //fan control
  
  Alarm.timerRepeat(timeBetweenMeasure, measureGround); //ground measure
  //Alarm.timerRepeat(10, measureGround); //ground measure
  
  Alarm.timerRepeat(3, changeIdleMenuPos); // idle Change   

  //Alarm.timerRepeat(4, flashLed); // Flash Led   
}



// ----------------------------------------------------------------------------------------------
// MAIN LOOP
// ---------------------------------------------------------------------------------------------- 
void loop()
{
  
  
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons
 Alarm.delay(100);
 updateDisplay(lcd_key);

 if (activateMeasureGroundp2 == true)
 {
    Alarm.delay(3000);
    measureGroundp2();
    activateMeasureGroundp2 = false;
 }

 if (activateSetMeasurep2 == true)
 {
    Alarm.delay(3000);
    setNewMaximumsp2();
    activateSetMeasurep2 = false;
 }

 
 
}




// ----------------------------------------------------------------------------------------------
// read the buttons
// ----------------------------------------------------------------------------------------------
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50){   
   delay(debounce);
   if (adc_key_in < 50){
   return btnRIGHT;}}  
 if (adc_key_in < 250){   
   delay(debounce);
   if (adc_key_in < 250){
     return btnUP;}}
 if (adc_key_in < 450){
   delay(debounce);
   if (adc_key_in < 450){
     return btnDOWN;}} 
 if (adc_key_in < 650){  
   delay(debounce);
   if (adc_key_in < 650){
     return btnLEFT; }}
 if (adc_key_in < 850){  
   delay(debounce);
   if (adc_key_in < 850){
     return btnSELECT; }} 

 return btnNONE;  // when all others fail, return this...
}




// ----------------------------------------------------------------------------------------------
// Updating display
// ----------------------------------------------------------------------------------------------
void updateDisplay( int newCommand)
{
  
  boolean selectPressed = 0;
  
  if (newCommand == 0){ // right button
    menuPosition = menuPosition +1;
    if (menuPosition > maxMainMenuPos) menuPosition = 0;   
    subMenuPosition = 0;
  }  
  if (newCommand == 1){ // up button
    subMenuPosition = subMenuPosition - 1;
    if (menuPosition == 1){
    if (subMenuPosition < 0) subMenuPosition = maxSubMenu1Pos;}
    if (menuPosition == 2){
    if (subMenuPosition < 0) subMenuPosition = maxSubMenu2Pos;}  
      
  }
  if (newCommand == 2){ // down button
    subMenuPosition = subMenuPosition + 1;
    if (menuPosition == 1){
    if (subMenuPosition > maxSubMenu1Pos) subMenuPosition = 0; }
    if (menuPosition == 2){
    if (subMenuPosition > maxSubMenu2Pos) subMenuPosition = 0; }
  }
  if (newCommand == 3){ // left button
    menuPosition = menuPosition - 1;
    if (menuPosition < 0) menuPosition = maxMainMenuPos;   
    subMenuPosition = 0;
  }
  if (newCommand == 4){ // select button
    selectPressed = 1;
  }
  if (newCommand == 5){ // no new button [idle]
     lcd.setCursor(0,1);
     //lcd.print("      ");
     if (idleMenu) 
     {
     }
        
  }
  
  switch (menuPosition){
    case 0: // case idle
       idleMenu = 1;
       lcd.setCursor(0,0);
       lcd.print(titleStr);
       lcd.print(String(hour(now())));
       lcd.print(":");
       lcd.print(String(minute(now())));
       lcd.print("   ");

       lcd.setCursor(0,1);
       switch (idleMenuPosition){
         case 0:
           lcd.print("Air Temp :     ");
           lcd.setCursor(11,1);
           lcd.print(String(airTemp));
           lcd.setCursor(0,1);
           break;
         case 1:
           lcd.print("Air %Hum :     ");
           lcd.setCursor(11,1);
           lcd.print(String(airHumidity));
           lcd.setCursor(0,1);
           break;
         case 2:
           lcd.print("P1 %Hum: ");
           lcd.print(String(p1Hum));
           lcd.print("%    ");
           lcd.setCursor(0,1);
           break;
         case 3:
           lcd.print("P2 %Hum: ");
           lcd.print(String(p2Hum));
           lcd.print("%    ");
           lcd.setCursor(0,1);
           break;
         case 4:
           lcd.print("P3 %Hum: ");
           lcd.print(String(p3Hum));
           lcd.print("%    ");
           lcd.setCursor(0,1);
           break;
         case 5:
           lcd.print("P4 %Hum: ");
           lcd.print(String(p4Hum));
           lcd.print("%     ");
           lcd.setCursor(0,1);
           break;
         case 6:
           lcd.print("P5 %Hum: ");
           lcd.print(String(p5Hum));
           lcd.print("%      ");
           lcd.setCursor(0,1);
           break;
         case 7:
           lcd.print("P6 %Hum: ");
           lcd.print(String(p6Hum));
           lcd.print("%     ");
           lcd.setCursor(0,1);
           break; 
         default:
           lcd.print("Weird");
       }
       break;
  
    case 1: // case states
      idleMenu = 0;
      lcd.setCursor(0,0);
      lcd.print("Conditions      "); // print a simple message
      lcd.setCursor(0,1);
      switch (subMenuPosition){
           case 0:
             lcd.print("Fan state : ");
             if (fanState == 0){
                lcd.print("Off ");
             }
             else if (fanState == 1){
                lcd.print("Low ");
             }
             else if (fanState == 2){
                lcd.print("High");
             }
             else {
                lcd.print("Err ");
             }
             break;
             
           case 1:
             lcd.print("Light On : ");
             lcd.print(String(hourLightOn));
             lcd.print(":00 ");
             break;
             
           case 2:
             lcd.print("Light Off: ");
             lcd.print(String(hourLightOff));
             lcd.print(":00");
             break;
             
           case 3:
             lcd.print("Air Temp :      ");
             lcd.setCursor(11,1);
             lcd.print(String(airTemp));
             lcd.setCursor(0,1);
             break;
           case 4:
             lcd.print("Air %Hum :      ");
             lcd.setCursor(11,1);
             lcd.print(String(airHumidity));
             lcd.setCursor(0,1);
             break;  
             
           case 5:
             lcd.print("P1  %Hum : ");
             lcd.print(String(p1Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 6:
             lcd.print("P2  %Hum : ");
             lcd.print(String(p2Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 7:
             lcd.print("P3  %Hum : ");
             lcd.print(String(p3Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 8:
             lcd.print("P4  %Hum : ");
             lcd.print(String(p4Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 9:
             lcd.print("P5  %Hum : ");
             lcd.print(String(p5Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 10:
             lcd.print("P6  %Hum : ");
             lcd.print(String(p6Hum));
             lcd.print("%    ");
             lcd.setCursor(0,1);
             break;
           case 11:
             lcd.print("Love ya  :            ");
             break;
           case 12:
             lcd.print(String(hour(now())));
             lcd.print(":");
             lcd.print(String(minute(now())));
             lcd.print(":");
             lcd.print(String(second(now())));
             lcd.print("         ");
             break;
           default:
             lcd.print("Weird1");
      }
      break;
    
    case 2: // case states
      idleMenu = 0;
      lcd.setCursor(0,0);
      lcd.print("Actions         "); // print a simple message
      lcd.setCursor(0,1);
  
      switch (subMenuPosition){
           case 0:
             lcd.print("Start Fan   ->  ");
             if (selectPressed) startFan();
             break;
           case 1:
             lcd.print("Stop Fan    ->  ");
             if (selectPressed) stopFan();
             break;
           case 2:
             lcd.print("Start Light ->  ");
             if (selectPressed) turnOnLight();
             break;
           case 3:
             lcd.print("Stop Light  ->  ");
             if (selectPressed) turnOffLight();
             break;
           case 4:
             lcd.print("Change Time ->   ");
             if (selectPressed) changeTime();
             break;

           case 5:
             lcd.print("Measure Hum ->   ");
             if (selectPressed) measureGround();
             break;

           case 6:
             lcd.print("Set As Max ->    ");
             if (selectPressed) setNewMaximums();
             break;
             
           default:
             lcd.print("Weird1");
      }
      break;
    
    default:
      lcd.setCursor(0,0);
      lcd.print("Weirder"); 
    } 
  
}
//=====================================================================================================

//----------------------------------------------------------------------------------------------
// Changge Time
//----------------------------------------------------------------------------------------------

void changeTime()
{
    byte setHour = hour();
    byte setMinute = minute();
    byte currentlySetting = 0;
    byte command = 5;
    boolean done = 0;
    lcd.setCursor(0,0);
    lcd.print("Set Time        "); 
    
    while (done == 0)
    {
      lcd.setCursor(0,1);
      lcd.print("Hour: ");
      lcd.print(String(setHour));
      lcd.print(" Min: ");
      lcd.print(String(setMinute));
      lcd.print("  ");
      
      command = read_LCD_buttons();
      
      if (command == 0){ // right button
        currentlySetting = 1;
      }  
      if (command == 1){ // up button
        if (currentlySetting == 0) 
        {
          setHour = setHour + 1;
          if (setHour == 24)
          {
            setHour = 0;
          }
        }

        else if (currentlySetting == 1)
        {
          setMinute = setMinute + 1;
          if (setMinute == 60)
          {
            setMinute = 0;
          }
        }
        
        
      }
      if (command == 2){ // down button
        if (currentlySetting == 0) 
        {
          setHour = setHour - 1;
          if (setHour == 255)
          {
            setHour = 23;
          }
        }

        else if (currentlySetting == 1)
        {
          setMinute = setMinute - 1;
          if (setMinute == 255)
          {
            setMinute = 59;
          }
        }
      }
      if (command == 3){ // left button
        currentlySetting = 0;
      }
      if (command == 4){ // select button
        setTime(setHour, setMinute, second(), day(), month(), year());
        done = 1;
      }
      if (command == 5){ // no new button [idle]
        
      }
    }
    
}





// ----------------------------------------------------------------------------------------------
// Control fan
// ----------------------------------------------------------------------------------------------
void startFan()
{
  if (fanState == 1)
  {
    digitalWrite(fanPinV1, LOW);
    digitalWrite(fanPinV2, HIGH);
    fanState = 2;
  }
  else
  {
    digitalWrite(fanPinV1, HIGH);
    digitalWrite(fanPinV2, LOW);
    fanState = 1;
  }
}

void stopFan()
{
  digitalWrite(fanPinV1, LOW);
  digitalWrite(fanPinV2, LOW);
  fanState = 0;
}


void automaticFanControl()
{
  byte airHum = 0;
  airHum = (byte) airHumidity;
  byte airT = 0;
  airT = (byte) airTemp;
  
  if ((airT > fanTempTresh)&&(airHum < fanTresh1))
  {
    digitalWrite(fanPinV1, HIGH);
    digitalWrite(fanPinV2, LOW);
    fanState = 1;
  }

  else if ((airHum > fanTresh1)&&(airHum < fanTresh2))
  {
    digitalWrite(fanPinV1, HIGH);
    digitalWrite(fanPinV2, LOW);
    fanState = 1;
  }

  else if (airHum > fanTresh2)
  {
    digitalWrite(fanPinV1, LOW);
    digitalWrite(fanPinV2, HIGH);
    fanState = 2;
  }
  
  else
  {
    digitalWrite(fanPinV1, LOW);
    digitalWrite(fanPinV2, LOW);
    fanState = 0;
  }
}


// ----------------------------------------------------------------------------------------------
// Air temp Measure
// ----------------------------------------------------------------------------------------------
void measureAirTempAndHum()
{
  
    airTemp = dht.readTemperature();
    airHumidity = dht.readHumidity();

}

void turnOnLight()
{
  lightState = 1;
  digitalWrite(LightPin, lightState);
}

void turnOffLight()
{
  lightState = 0;
  digitalWrite(LightPin, lightState);
}

void changeIdleMenuPos()
{
  idleMenuPosition = idleMenuPosition + 1;
  if (idleMenuPosition > maxIdleMenuPos) idleMenuPosition = 0;

  flashLed();
}
    

void measureGround()
{
  digitalWrite(groundMeasureEnablePin, LOW);
  activateMeasureGroundp2 = true;
  //Alarm.timerOnce(2, measureGroundp2);
}

void measureGroundp2()
{
  int temp0 = 0;
  int temp1 = 0;
  int temp2 = 0;
  int temp3 = 0;
  int temp4 = 0;
  int temp5 = 0;
  int temp6 = 0;

  temp0 = analogRead(8);
  temp1 = analogRead(9);
  temp2 = analogRead(10);
  temp3 = analogRead(11);
  temp4 = analogRead(12);
  temp5 = analogRead(13);
  temp6 = analogRead(14);

  p1Hum = (float)temp0/maxCh1*100;
  p2Hum = (float)temp1/maxCh2*100;
  p3Hum = (float)temp2/maxCh3*100;
  p4Hum = (float)temp3/maxCh4*100;
  p5Hum = (float)temp4/maxCh5*100;
  p6Hum = (float)temp5/maxCh6*100;

  digitalWrite(groundMeasureEnablePin, HIGH);
}

void setNewMaximums (){
  digitalWrite(groundMeasureEnablePin, LOW);
  activateSetMeasurep2 = true;
  //Alarm.timerOnce(2, setNewMaximumsp2);
}

void setNewMaximumsp2 ()
{
  maxCh1 = analogRead(8);
  maxCh2 = analogRead(9);
  maxCh3 = analogRead(10);
  maxCh4 = analogRead(11);
  maxCh5 = analogRead(12);
  maxCh6 = analogRead(13);
  maxCh7 = analogRead(14);
  digitalWrite(groundMeasureEnablePin, HIGH);
}

void flashLed()
{
  boolean flash = false;
  if (p1Hum < minAllCh) flash = true;
  if (p2Hum < minAllCh) flash = true;
  if (p3Hum < minAllCh) flash = true;
  if (p4Hum < minAllCh) flash = true;
  if (p5Hum < minAllCh) flash = true;
  if (p6Hum < minAllCh) flash = true;

  if (flash) {
    if (humidityLEDState == false){
      digitalWrite(flashingLEDPin, HIGH);
      humidityLEDState = true;
    }
    else if (humidityLEDState == true){
      digitalWrite(flashingLEDPin, LOW);
      humidityLEDState = false;
    }
  }
  else
  {
    digitalWrite(flashingLEDPin, LOW);
    humidityLEDState = false;
  }
}


