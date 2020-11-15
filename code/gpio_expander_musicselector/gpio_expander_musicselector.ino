#include <Wire.h>
#include "Adafruit_MCP23008.h"

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Uses Adafruit MCP23008 I/O expander

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)
// Input #0 is on pin 10 so connect a button or switch from there to ground

// GPIO Expander
Adafruit_MCP23008 mcp;

// MP3 Player Com settings
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
int dt = 250;
int modePin = 33; // debugging button
int genrePin = A0;
int trackPin = A1;
int mcpLED[] = {0, 1, 2, 3, 4, 5, 6, 7};
int ledPinCount = 7;
  
void setup() {  
   
  // Initialize the IO Expander pins for LED Genre Indicator
  mcp.begin();      // use default address 0
  mcp.pullUp(0, HIGH);  // turn on a 100K pullup internally

  for (int i=0; i<= ledPinCount; i++) {
    mcp.pinMode(i, OUTPUT);
    mcp.digitalWrite(i, HIGH);
    delay(1000);
    mcp.digitalWrite(i, LOW);
    delay(1000);
  }

  // Debugging button
  pinMode(modePin, INPUT);    // set digital pin 33 as input
  
  // Initialize DFPlayer Mini MP3 player
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println("Music Selector with Track Button and Genre Selector");

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  while (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }
    delay(5000);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30

} // setup


void loop() {

  static int buttonVal = LOW;
  static int playerState = 0;
  static int sensorValue = 0;
  static int oldAI = 0;
  static int genre = 0;
  static int track = 0;

  // check the mode to get the folder
  buttonVal = digitalRead(modePin);
  sensorValue = analogRead(genrePin);

  if ((sensorValue > (oldAI + 50)) || (sensorValue < (oldAI - 50)))// see if the value has changed?
  {
    oldAI = sensorValue;
    genre = sensorValue/146;

    Serial.println("*****************************");
    Serial.println(genre);
    Serial.println("*****************************");

    if (playerState != 0) {
      myDFPlayer.playFolder(genre+1,track+1);
    }

    // Update the indicator LED
    for (int i=0; i<= ledPinCount; i++) {
        mcp.digitalWrite(i, LOW);
    }
    mcp.digitalWrite(genre, HIGH);
  }
  //Serial.println(sensorValue);

  // Check the buttons to get the track
  sensorValue = analogRead(trackPin);
  String buttonPress = "none";
 
  int j = 0;
  if(sensorValue == 0) {
    buttonPress = "none";
  }
  else {
    while (1024/pow(2,j) > sensorValue) {
      j++;
    }
    //Serial.print("1024/2^j = ");
    //Serial.println(1024/pow(2,j));
    buttonPress = j-1;
  }
  Serial.println(sensorValue);
  if (track != j) {
    track = j;
    myDFPlayer.playFolder(genre+1,track+1);
    playerState = 1;
  }

/*
  if (buttonVal == HIGH) {
    if (playerState == 0) {
      myDFPlayer.playFolder(2,3);  //play specific mp3 in SD:/2/003.mp3; Folder Name(1~99); File Name(1~255)
      Serial.println("Button State: HIGH");
      Serial.println("Beginning to play");
      playerState = 1;
      lightTimer = millis();
    }
    else {
      myDFPlayer.stop();  //stop playback
      Serial.println("Button State: HIGH");
      Serial.println("Time to Stop");
      playerState = 0;
      
    }
  }*/
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value =  myDFPlayer.read();
    printDetail(type, value); //Print the detail message from DFPlayer to handle different errors and states.
    Serial.println(type);
    Serial.println(value);
    playerState = 0;
  }

  delay(dt);
  
} // loop()

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
} // printDetail()
