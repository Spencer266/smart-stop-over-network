#include "DHT.h"            
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define OLED_CLK    9 //D0
#define OLED_MOSI  10 //D1
#define OLED_RESET 11
#define OLED_DC    12
#define OLED_CS    13

#define DHTPIN      2

#define TRIG_PIN    3
#define ECHO_PIN    4

#define BUTTON_PIN  5

#define MQ_DIGITAL  6
#define MQ_ANALOG  A0

typedef struct data {
  float humid;
  int temp;
  int COVal;
  int seats;
} Data;

int nodeNo = 1; // resistor =))
int revData = 0;
int distance;
int timer = -1000;

long long int waitTime = 320; //s
Data pack;
String sendData;

DHT dht(DHTPIN, DHT11);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();

  display.setTextSize(1);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  const unsigned long duration=pulseIn(ECHO_PIN, HIGH);
  distance= duration/29/2;
  if (distance < 50) pack.seats = 1;
  else pack.seats = 0;

  if (timer <= 0) {
    if (digitalRead(BUTTON_PIN) == 1) {
      timer = waitTime;
    }
  }
  
  pack.humid = dht.readHumidity();    //Đọc độ ẩm
  pack.temp = dht.readTemperature();  //Đọc nhiệt độ;
  pack.COVal = analogRead(MQ_ANALOG);
  
  if (Serial.available() == 1) {
    revData = Serial.read();
    if (revData == nodeNo + 48) {
      sendData = dataToString(&pack);
      Serial.print(sendData);
    }
    if (revData == 0) {
      // reserved for special purpose
    }
  } else if (Serial.available() > 1) {
    Serial.read();
  } else {
    
  }
  
  displayData(&pack);
  delay(200);
  
  if (timer > 0) timer = timer - 1;
}

String dataToString(Data *d) {
  String res = "";
  res += String(d->temp) + "\n";
  res += String(d->humid) + "\n";
  res += String(d->COVal) + "\n";
  res += String(d->seats) + "$";
  return res;
}

void displayData(Data *d) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.print(d->temp); display.println(" *C");
  display.print("Humidity: ");
  display.print(d->humid); display.println(" %");
  display.print("CO rate: ");
  display.println(d->COVal);
  display.print("Seats: ");
  if (d->seats > 0)
    display.println("Occupied");
  else 
    display.println("Available");

  //display.display();
  
  if (timer > 0) {
    //display.setTextColor(0xF800);
    display.print("\n");
    if (timer > 60) {
      display.print("Arrive in: ");
      display.print(timer/60);
      display.println(" mins");
    } else if (timer <= 60 && timer > 5) {
      display.println("Arriving...");
    } else {
      display.println("Have a happy trip!");
    }
  }
  display.display();
}
