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

//Neopixel declarations
const int PIXELPIN = 8;
const int PIXELCOUNT = 1;
const int PIXELBRIGHTNESS = 128;
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
  //none
  
//Hue declarations
int hues[] = {1, 2, 3, 4, 5};

//Clock declarations
long long currentdate = 0;

//Control declarations
int currentTime = 0;
int oldTime = 0;
int mode = 0;
int steps = 0;
long long pctime;

//Additional variables
int count = 0;
int status = 0;
int sex = 0;
long long birthDate = 0;
long long deathDate = 0;
const long long maleLifespan = 2368353600; //75.1 years in seconds
const long long femaleLifespan = 2538648000; //80.5 years in seconds
const long long averageLifespan = 2453500800; //77.8 years in seconds

void setup() {
  //set up serial monitor
  Serial.begin(9600);
  
  //set up buttons
  redButton.attachClick(redClick);
  blueButton.attachClick(blueClick);
  encoderButton.attachClick(encoderClick);
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
    
  //set up display
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
 
}

void loop() {
  redButton.tick();
  blueButton.tick();
  encoderButton.tick();
  
  // Check mode
  //0: Get DOB
  //1: Get Sex
  //2: Automatic operation
  //3: Manual operation
  //4: Existential panic

}


void getDOB(){
  //check step
  //0: Get year
  //1: Get month
  //2: Get year  
}

void getSex(){
  //0: Male
  //1: Female
  //2: Complex

  
}

void autoHorror(){
  //blink neopixel
  
  //beep
  
  //show hue
}

void manualMode(){
  //set lights to white
  //Encoder controls brightness
}

void panic(){
  //start the tea
}

void redClick(){
  Serial.println("CLICKED RED!");
}

void blueClick(){
  Serial.println("CLICKED BLUE!");
}

void encoderClick(){
  Serial.println("CLICKED ENCODER!");
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
