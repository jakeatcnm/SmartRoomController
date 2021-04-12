/* Project:  Tempus Mortis Smart Room Controller
 * Description: Memento Mori
 * Author: JAKE MILLINGTON
 * Date: 08-APR-2021
 */

#include <OneButton.h>
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>
#include <mac.h>
#include <wemo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <hue.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <math.h>
 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3c
 

//Button declarations
const int REDBUTTON = 17;
const int BLUEBUTTON = 14;
const int ENCODERBUTTON = 5;
OneButton redButton(REDBUTTON, false, false);
OneButton blueButton(BLUEBUTTON, false, false);
OneButton encoderButton(ENCODERBUTTON, false);



//Encoder declarations
const int PIN_A = 16;
const int PIN_B = 15;
const int GREEN_LED = 6;
const int RED_LED = 7;
Encoder encoder(PIN_A, PIN_B);
int pos = 0;
int oldPos = 0;
int hueColor = 0;
int hueSat = 0;
int hueBright = 255;

//Neopixel declarations
const int PIXELPIN = 8;
const int PIXELCOUNT = 1;
const int PIXELBRIGHTNESS = 128;
const int BLINKLENGTH = 20;
long blinkRate = 0;
bool blinking = false;
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, NEO_GRB + NEO_KHZ800);


//Display declarations
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//Audio declarations

//Ethernet declarations
 /*
  * Ethernet Port Wiring
  * 3.3V to Teensy 3.3V
  * GND to Teensy GND
  * MISO to Teensy DI (Pin 12)
  * MOSI to Teensy DO (Pin 11)
  * SCLK to Teensy SCK (Pin 13)
  * SCNn to Teensy Pin 10 (for Chip Select)
  * RSTn to Teensy Pin 9
  * INTn and NC (No Connection) not connected
  */


//Wemo declarations
Wemo wemo;
  
//Hue declarations
int hues[] = {1, 2, 3, 4, 5};
int hueIncrement = 0;


//Clock declarations
long long currentdate = 0;

//Control declarations
int currentTime = 0;
int blinkTime = 0;
int oldTime = 0;
int mode = 0;
int steps = 0;
long long pctime;

//Additional variables
int count = 0;
int status = 0;
int sex = 0;
int birthYear = 1970;
int birthMonth = 1;
int birthDay = 1;
tmElements_t bd;
time_t birthDate;
long long deathDate = 0;
long long remainingLife = 0;
int value = 0;
const long long maleLifespan = 2368353600; //75.1 years in seconds
const long long femaleLifespan = 2538648000; //80.5 years in seconds
const long long averageLifespan = 2453500800; //77.8 years in seconds
long long myLifespan = 0;
bool panicked = false;

void setup() {
  //set up serial monitor
  Serial.begin(9600);
  Serial.println("Beginning self-test");
  
  //set up buttons
  redButton.attachClick(redClick);
  blueButton.attachClick(blueClick);
  encoderButton.attachClick(encoderClick);
  //encoderButton.attachDoubleClick(encoderDouble);
  redButton.setClickTicks(500);
  blueButton.setClickTicks(500);
  encoderButton.setClickTicks(500);


  //set up encoder
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED,HIGH);
    delay(1000);
    digitalWrite(GREEN_LED,LOW);
    encoder.write(0);
    
  //set up Neopixel
    pixel.begin();
    pixel.clear();
    pixel.setPixelColor(0, 255, 0, 0);
    pixel.show();
    delay(1000);
    pixel.clear();
    pixel.show();
    

  //setup I2C
  Wire.begin();
  for (int i = 0; i <= 127; i++)
  {
    Wire.beginTransmission (i);
    //Serial.printf("Wire transmission end returned: %i \n",Wire.endTransmission());
    if (Wire.endTransmission () == 0)
      {
      Serial.printf("Found address: %03i (0x%02X) \n",i,i);  
      count++;
      delay (1);
      } 
  }
  Serial.printf("Done: Found %i device(s). \n",count);

  //setup Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  //set up audio

  //set up ethernet
    status = Ethernet.begin(mac);
  if (!status) {
    Serial.printf("failed to configure Ethernet using DHCP \n");
    //no point in continueing 
    while(1);
    }
  //print your local IP address
  Serial.print("My IP address:");
  for (byte thisbyte = 0; thisbyte < 4; thisbyte++) {
    //print value of each byte of the IP address
    Serial.print(Ethernet.localIP()[thisbyte], DEC);
    if (thisbyte < 3) Serial.print(".");
    }
  Serial.println();

  //set up Wemo
    //nothing to do
    
  //set up hue
    //nothing to do
    
  //set up clock
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0); 
  display.printf("%i\n", getTeensy3Time());
  display.display();
  delay(500);
  display.clearDisplay();
  display.printf("TEMPUS\nMORTIS");
  display.display();
  Serial.print("\nWelcome to Tempus Mortis.\nStarting in 5 seconds.\n");
  delay(5000);
}

void loop() {
  redButton.tick();
  blueButton.tick();
  encoderButton.tick();
  
  // Check mode
  //0: Get Sex
  //1: Get Birthdate
  //2: Automatic operation
  //3: Manual operation
  //4: Existential panic

  switch(mode){
    case 0:
      getSex();
      break;
    case 1:
      getDOB();
      break;
    case 2:
      autoHorror();
      break;
    case 3:
      manualMode();
      break;
    case 4:
      panic();
      break;
    default:
      mode = 0;
      break;
  }

}


void getDOB(){
  //check step
  //0: Get year
  //1: Get month
  //2: Get year  

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0); 
  display.printf("Please enter your\nbirth date.\n");

  switch(steps){
    case 0:
      //year
      value = encoder.read();
      value = value / 10;
      birthYear = value;
      display.printf("Year: %i \n", birthYear + 1970);
      
      break;
    case 1:
      pos = encoder.read();
      if(pos < 10){
        encoder.write(10);
      }
      else if(pos > 129){
        encoder.write(129);
      }
      value = pos / 10;
      birthMonth =  value;
      display.printf("Month: %i \n", birthMonth);
      break;
    case 2: 
      pos = encoder.read();
      if(pos < 10){
        encoder.write(10);
      }
      else if(pos > 318){
        encoder.write(318);
      }
      value = pos / 10;
      birthDay =  value;
      display.printf("Day: %i \n", birthDay);
      break;
    default:
      //SCREAM!
      Serial.println("WTF!?!?!? YOU SHOULD NOT EVER BE HERE!");
      steps = 0;
      mode =0;
  }
    display.display();
  //set birthdate as long long
  //add lifespan to birthdate to calculate deathdate
  
}

void getSex(){
  //0: Male
  //1: Female
  //2: Complex

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0); 
  display.printf("Please enter your\ngender.\n");
  pos = encoder.read();
  
  //Keep encoder in range
  if(pos < 0){
    pos = 0;
    encoder.write(pos);
  }
  else if(pos > 30){
    pos = 30;
    encoder.write(pos);
  }

  if(pos <= 10){
    sex = 0;
    display.printf("\n>   Male +\n");
  }
  else if(pos <=20){
    sex = 1;
    
    display.printf("\n> - Female +\n");
  }
  else{
    sex = 2;
    
    display.printf("\n> - It's complicated\n");
  }
  
  display.display();
  
}

void autoHorror(){
  remainingLife = deathDate - getTeensy3Time();

  display.clearDisplay();
  display.setCursor(0,0); 
  display.printf("Life left: %i\n", remainingLife);
  display.printf("Saturation: %i\n", hueSat);
  display.printf("Brightness: %i\n", hueBright);
  display.display();
  //blink neopixel
  deathBlink();
  //beep
  
  //show hue
  hueIncrement = myLifespan / 255;
  hueSat = (255 - (remainingLife / hueIncrement)) ;
  hueBright = remainingLife / hueIncrement;
  Serial.println(hueSat);
  Serial.println(hueBright);
  for (int j = 0; j < 5; j++){
    //setHue(hues[j],true,HueRed,hueBright,hueSat);
    delay(10);
  }
  
}

void deathBlink(){
  blinkRate = remainingLife / 1000000;
  currentTime = millis(); 
  if(currentTime > blinkTime){
    pixel.setPixelColor(0,255, 0, 0);
    pixel.setBrightness(255);
    pixel.show();
    delay(20);
    pixel.clear();
    pixel.show();
    blinkTime = currentTime + blinkRate;
  }
}

void manualMode(){
  hueSat = 0;
  display.clearDisplay();
  display.setCursor(0,0); 
  display.printf("Saturation: %i\n", hueSat);
  display.printf("Brightness: %i\n", hueBright);
  display.display();
  //set lights to white
  pos = encoder.read();
  if(pos > 255){
    pos = 255;
    encoder.write(255);
  }
  if(pos < 0){
    pos = 0;
    encoder.write(0);
  }
  hueBright = pos;
  for (int j = 0; j < 5; j++){
    //setHue(hues[j],true,0,hueBright,0);
    delay(10);
  }
  //Encoder controls brightness
}

void panic(){

}

void redClick(){
  Serial.println("CLICKED RED!");
  if(mode == 2){
    mode = 3;
    encoder.write(255);
  }
  else if(mode == 3){
    mode = 2;
  }
}

void blueClick(){
    if(!panicked){
   
    wemo.switchON(3); 
    panicked = true;
  }
  else{
    wemo.switchOFF(3);
    panicked = false;
  }
  Serial.println("CLICKED BLUE!");
  if(mode == 2){
    mode = 3;
  }
  else if(mode == 3){
    mode = 3;
  }
  else if(mode == 4){
    mode = 2;
  }
}

void encoderClick(){
  Serial.println("CLICKED ENCODER!");
  if(mode == 0){
    mode = 1;
    encoder.write(0);
  }
  else if (mode == 1){
    steps +=1;
    encoder.write(0);
    if(steps == 3){
      bd.Year = birthYear;

      bd.Month = birthMonth;
      bd.Day = birthDay;
      Serial.println(bd.Day);
      
      Serial.println(bd.Month);
      
      Serial.println(bd.Year);
      birthDate = makeTime(bd);
      Serial.println(birthDate);
      switch(sex){
        case 0:
          deathDate = maleLifespan + birthDate;
          myLifespan = maleLifespan;
          break;
        case 1: 
          deathDate = femaleLifespan + birthDate;
          myLifespan = femaleLifespan;
          break;
        default:
          deathDate = averageLifespan + birthDate;
          myLifespan = averageLifespan;
      }
      //Serial.println(deathDate);
      
      steps = 0;
      mode = 2;
    }
  }
}

void encoderDouble(){
  mode = 0;
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

long long processSyncMessage() {
  long long pctime = 0L;
  const long long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}
