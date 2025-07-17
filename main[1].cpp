#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPL3JEcFrnTM"   // connection to blynk app
#define BLYNK_TEMPLATE_NAME "IV Bag Monitoring"
#define BLYNK_AUTH_TOKEN "o3jQz1IQS9jTu9iON8mqOVtc9GFXiIgL"
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>


char auth[] = "o3jQz1IQS9jTu9iON8mqOVtc9GFXiIgL"; //wifi connection
char ssid[] = "POCO X5 Pro";
char pass[] = "12345678";

#define BLYNK_PRINT Serial

#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define DOUT 12  // HX711 Data Output pin
#define SCK  13  // HX711 Serial Clock pin
#define buzzer 17
#define sensorpin 34 //bubble sensor pin
#define servo 35 //servo motor pin
#define startbutton 23 //start button

Servo myServo;
int servoPin = 18;

double volume;
double percentage;


long readHX711() {
    while (digitalRead(DOUT));  // Wait for HX711 to become ready (DOUT goes low)

    long value = 0;
    
    // Read 24 bits
    for (int i = 0; i < 24; i++) {
        digitalWrite(SCK, HIGH);
        delayMicroseconds(1);
        value = (value << 1) | digitalRead(DOUT); // Shift in data bit-by-bit
        digitalWrite(SCK, LOW);
        delayMicroseconds(1);
    }

    // 25th pulse to set gain = 128 (default channel A)
    digitalWrite(SCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(SCK, LOW);
    delayMicroseconds(1);

    // Convert to signed 32-bit value
    if (value & 0x800000) {  // Check if the 24th bit is set (negative number)
        value |= 0xFF000000; // Sign extend to 32-bit
    }

    return value;
}


void setup() {


    // Set up serial monitor
    pinMode(DOUT, INPUT);
    pinMode(SCK, OUTPUT);
    Serial.begin(115200);

    WiFi.begin(ssid, pass);

    myServo.attach(servoPin);
    myServo.write(180);   // Full-speed anticlockwise
    delay(890);
    myServo.write(90);


    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Setup in");
    display.setCursor(0, 40);
    display.println("Progress");
    display.display();
  
    Blynk.begin(auth, ssid, pass);

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Setup");
    display.setCursor(0, 40);
    display.println("Complete!");
    display.display();

    pinMode(sensorpin,INPUT);

    pinMode(startbutton,INPUT_PULLDOWN);

  //buzzer
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  Serial.println("bff");
  Blynk.virtualWrite(V2,0);
  Blynk.virtualWrite(V1,0);
  Serial.println("baa");
  

}
void loop() {
    while(digitalRead(startbutton)==LOW){
  
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("System");
    display.setCursor(0, 40);
    display.println("Active...");
    display.display();
    digitalWrite(buzzer,HIGH);
    delay(100);
    digitalWrite(buzzer,LOW);
    while((digitalRead(sensorpin)==0)){
    long sum = 0;
    int count = 0;
    const int samples = 10;  // Number of samples

    for (int i = 0; i < samples; i++) {
        long rawValue = readHX711(); // Read a fresh value inside the loop

        if (rawValue >= 10000) {  // Ignore noise
            sum += rawValue;
            count++;
        }
        delay(10);  // Small delay for stable readings
    }

    if (count > 0) {
        double average = (double)sum / count;  // ✅ Use floating-point division
        Serial.print("Average Raw Value: ");
        Serial.println(average, 2);  // ✅ Print with 2 decimal places

        double weight = (average / 372800.0)+0.1;  // ✅ Keep floating-point division
        Serial.print("Weight: ");
        Serial.println(weight, 2);  // ✅ Print weight with 2 decimal places
        volume = (weight * 1000.0);
        Serial.println(volume,2);
        percentage=(volume/500.0)*100.0;
        Serial.println(percentage);
        Blynk.virtualWrite(V1,percentage);
    } else {
        Serial.println("No valid readings!");
    }
    

}
myServo.write(0);   // Full-speed clockwise
  delay(900);
  myServo.write(90);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("Air Bubble");
  display.setCursor(0, 40);
  display.println("Detected");
  display.display();
  while(100>=1){
    digitalWrite(buzzer,HIGH);
    delay(100);
    digitalWrite(buzzer,LOW);
    delay(100);
    Blynk.virtualWrite(V2,1);
  }
}
  















