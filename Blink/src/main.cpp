#include <Arduino.h>

const int redLedPin = 42;
const int buttonPin = 41;



void setup (){
Serial.begin (115200);
pinMode (redLedPin, OUTPUT);
pinMode (buttonPin, INPUT_PULLUP);


digitalWrite(buttonPin, LOW);
}



void loop (){
int redButtonState = digitalRead(buttonPin);
  if(redButtonState == LOW){
    digitalWrite(redLedPin, HIGH);
    delay(500);
} else{
    digitalWrite(redLedPin, LOW);
    delay(500);
}

}