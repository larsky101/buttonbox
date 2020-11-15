
int sensorPin = A1;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int oldAI = 0;
int genre = 0;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("Initializing Analog Read on Pin A1... (May take 3~5 seconds)"));
  
}


void loop() {
  // read the value from the sensor:

  sensorValue = analogRead(sensorPin);
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
  Serial.println(buttonPress);
  // turn the ledPin on
  //digitalWrite(ledPin, HIGH);

  //Serial.println(sensorValue, DEC);
  //Serial.println(sensorValue, BIN);
  //Serial.println(bitPlaces);

}
