#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1327.h>

const int led_Pin = 35;
const int redButton = 14;
const int buzzerPin = 37;
const int SDA_PIN = 17;
const int SCL_PIN = 18;


Adafruit_SSD1327 display(128, 128, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode (redButton, INPUT_PULLUP);
pinMode (led_Pin, OUTPUT);
pinMode (buzzerPin, OUTPUT);
digitalWrite(redButton,LOW);


Wire.begin(SDA_PIN, SCL_PIN);
display.begin(0x3D);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1327_WHITE);
display.display();


}

void loop() {
  // put your main code here, to run repeatedly:
int redButtonState = digitalRead(redButton);

if (redButtonState == LOW){
  digitalWrite(led_Pin, HIGH); 
  delay(300);
  tone(buzzerPin, 500);
  display.clearDisplay();
  display.setCursor(30, 64);
  display.setTextSize(3);
  display.println("STOP");
  display.display();




  Serial.println("Red button pressed - LED ON");
  digitalWrite(led_Pin, LOW);
  delay(300);
  tone(buzzerPin, 700);

  display.clearDisplay();
  display.display();

}else{
  digitalWrite(led_Pin, LOW);
  noTone(buzzerPin);
  Serial.println("red Button released - LED OFF");
  
  display.clearDisplay();
  display.setCursor(2, 64);
  display.setTextSize(1);
  display.println("Safe to pass now.");
  display.display();

}
}