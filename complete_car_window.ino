// constants won't change. They're used here to set pin numbers:
const int buttonPinUp = 10;    // the number of the pushbutton pin
const int buttonPinDown = 9;    // the number of the pushbutton pin
const int buttonPinKillSwitch = A1;
const int resistor = A0;

const int ledPinDown = 11;      // the number of the LED pin
const int ledPinUp = 12;      // the number of the LED pin
const int ledPinPower = 13;      // the number of the LED pin

// Variables will change:
int ledStateDown;         // the current state of the output pin
int ledStateUp;         // the current state of the output pin
int ledStatePower;         // the current state of the output pin


int buttonStateUp = LOW;             // the current reading from the input pin
int buttonStateDown = LOW;             // the current reading from the input pin
int lastButtonStateUp = LOW;   // the previous reading from the input pin
int lastButtonStateDown = LOW;   // the previous reading from the input pin]

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
unsigned long currentMillis = 0;
unsigned long previousMillis= 0;

void setup() {
  pinMode(buttonPinUp, INPUT);
  pinMode(ledPinUp, OUTPUT);
  pinMode(buttonPinDown, INPUT);
  pinMode(ledPinDown, OUTPUT);
  pinMode(ledPinPower, OUTPUT);
  pinMode(buttonPinKillSwitch, INPUT);
  pinMode(resistor, INPUT);
  Serial.begin(9600);
  // set initial LED state
  digitalWrite(ledPinDown, ledStateDown);
  digitalWrite(ledPinUp, ledStateUp);
  digitalWrite(ledPinPower, ledStatePower);
}

void power(int power){
  int timer = 0;
  long previousMillis = millis();
  digitalWrite(ledPinPower, power);
  while ((millis() - previousMillis) < 75){
    Serial.println("ticking");
  }
  
  Serial.print("Power: ");
  Serial.println(power);
}

long rollWindow(long previousMillis){
  long holdMillis;
  while ((analogRead(buttonPinKillSwitch) < analogRead(resistor)) && digitalRead(buttonPinUp) == HIGH)
      {
        currentMillis = millis();                               // Continue counting currentMillis
        holdMillis = currentMillis - previousMillis;            // Take timestamped previousMillis from currentMillis
        ledStateUp = HIGH;
        power(HIGH);
        digitalWrite(ledPinUp, ledStateUp); 
        
        Serial.print("UP for ");
        Serial.print(holdMillis);
        Serial.println(" ms.");
  
        if (holdMillis >= 8000)         // Break out of the loop after ~(4000 ms)
          break;                        // ie.: Stop rolling car window
      }
      
      while ((analogRead(buttonPinKillSwitch) < analogRead(resistor)) && digitalRead(buttonPinDown) == HIGH)
      {
        currentMillis = millis();                               // Continue counting currentMillis
        holdMillis = currentMillis - previousMillis;            // Take timestamped previousMillis from currentMillis
        ledStateDown = HIGH;
        power(HIGH);
        digitalWrite(ledPinDown, ledStateDown);
        
        Serial.print("DOWN for ");
        Serial.print(holdMillis);
        Serial.println(" ms.");
        if (holdMillis >= 8000)         // Break out of the loop after ~(4000 ms)
          break;                        // ie.: Stop rolling car window
      }
  return holdMillis;
}


void automaticWindow(long previousMillis, int ledState, int ledPin){
  int duration = 0;
  bool continueLoop = true;
  int buttonDelay = 0;
  int tempDelay;
  Serial.println("Automatic Widnow");
  power(HIGH);
  digitalWrite(ledPin, ledState);         //Write LED state back to LED 
  while ((analogRead(buttonPinKillSwitch) < analogRead(resistor)) && continueLoop && (duration <= 3900)){
          currentMillis = millis(); 
          duration = currentMillis - previousMillis;
          
          Serial.println(duration);
          if ((duration > 50) && ((digitalRead(buttonPinUp) == HIGH) || (digitalRead(buttonPinDown) == HIGH))){
            continueLoop = false;
          }
        }
  Serial.println("Stop Automatic Window");
  power(LOW);
  digitalWrite(ledPin, !ledState);         //Write LED state back to LED
  
  while ((analogRead(buttonPinKillSwitch) < analogRead(resistor)) &&  (digitalRead(buttonPinUp) == HIGH) || (digitalRead(buttonPinDown) == HIGH))
  {
    continue;
  }
}


void loop() {
  // read the state of the switch into a local variable:
  int readingUp = digitalRead(buttonPinUp);
  int readingDown = digitalRead(buttonPinDown);
  
  unsigned long holdMillis = 0;
  bool whileLooped = false;
  
  int killSwitch = 0;
  int ledStateDown = LOW;         // the current state of the output pin
  int ledStateUp = LOW;         // the current state of the output pin
  int ledStatePower = LOW;         // the current state of the output pin
  

  Serial.print(analogRead(buttonPinKillSwitch));
  Serial.print("   ___   ");
  Serial.println(analogRead(resistor));


  
  currentMillis = millis();
  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    
    // if the button state has changed:
    if ((digitalRead(buttonPinUp) == HIGH) || (digitalRead(buttonPinDown) == HIGH)) {// || readingDown != buttonStateDown) {
      buttonStateUp = digitalRead(buttonPinUp);
      buttonStateDown = digitalRead(buttonPinDown);
     
      previousMillis = currentMillis;

      holdMillis = rollWindow(previousMillis);
      
      if (whileLooped) {                    //If the button was held, print how long it was held for
      // Diagnostic for Serial Monitoring
       
        Serial.print("You held it for ");
        Serial.print((holdMillis)/1000.0);
        Serial.println(" seconds");
      }
      Serial.print("UP ");
      Serial.println(buttonStateUp);
      Serial.print("DOWN ");
      Serial.println(buttonStateDown);

      if (holdMillis <= 200) {
        if (buttonStateUp == HIGH){
          Serial.println("Quick UP Tap!");
          previousMillis = currentMillis;         //Timestamp
          ledStateUp = HIGH;
          automaticWindow(previousMillis, ledStateUp, ledPinUp);
         
        } else if (buttonStateDown == HIGH){
          Serial.println("Quick DOWN Tap!");
          previousMillis = currentMillis;         //Timestamp
          holdMillis = 0;
          ledStateDown = HIGH;
          automaticWindow(previousMillis, ledStateDown, ledPinDown);
        }
      } 
        ledStatePower = LOW;
        ledStateDown = LOW; 
        ledStateUp = LOW;
        power(LOW);
        digitalWrite(ledPinDown, ledStateDown); 
        digitalWrite(ledPinUp, ledStateUp);
        
    }
        lastButtonStateUp = readingUp;
        lastButtonStateDown = readingDown;
}
if (readingUp != lastButtonStateUp || readingDown != lastButtonStateDown) {
    // reset the debouncing timer
    Serial.print(buttonStateUp);
    Serial.print(" Debouncing ");
    Serial.println(readingUp);
    lastDebounceTime = millis();
    
  }

}
