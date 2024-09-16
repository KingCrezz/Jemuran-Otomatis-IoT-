#include "DHT.h" //include library DHT
#include "WiFi.h" //include library untuk jaringan wifi
#include "HTTPClient.h"
#include <Stepper.h> //Stepper Library
#include <ESP32Servo.h>

//definisikan pin untuk DHT
#define DHTPIN 26 //masuk pin dht
#define DHTTYPE DHT22
#define relay 32 //input  pin ungu, hijau minus, biru plus
//kaki modul stepper
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

#define hujan 35
#define hujan1 36
#define kecerahan 33

//objek untuk DHT
DHT dht(DHTPIN, DHTTYPE);

Servo myServo;
Servo myServo1;
const int servoPin = 14;
const int servo1Pin = 16;

const int stepsPerRevolution = 500; //banyaknya putaran stepper
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); //Motor Stepper

//siapkan variabel untuk wifi dan password
const char* ssid = "POCO";
const char* pass = "ayamgoreng";

//siapkan variabel host/server yang menampung aplikasi web dan database
const char* host = "laundryciplak.000webhostapp.com";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//aktifkan serial
  //aktifkan sensor DHT
  dht.begin();
  pinMode(relay, OUTPUT);
  myStepper.setSpeed(50); //kecepatan putar motor stepper
  myServo.attach(servoPin);
  myServo1.attach(servo1Pin);
  //koneksi ke wifi
  WiFi.begin(ssid, pass);
  Serial.println("Connecting....");

  while(WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("........");
    delay(500);
  }

  //apabila berhasil terkoneksi
  Serial.println("Connected");
}

void loop() {
  //put your main code here, to run repeatedly:
  //baca nilai suhu dan kelembaban
  float suhu = dht.readTemperature();
  int kelembaban = dht.readHumidity();
  int ldr = analogRead(kecerahan);
  int data_air = analogRead(hujan);
  int baju = analogRead(hujan1);
  //tampilkan nilai sensor ke serial monitor
  Serial.println("-----------------------");
  Serial.println("Suhu : " + String(suhu) + "℃");
  Serial.println("Kelembaban : " + String(kelembaban) + "%");
  Serial.println("Cahaya : " + String(ldr));
  Serial.println("Air : " + String(data_air));
  Serial.println("Baju : " + String(baju));

  if(baju <= 4040) {
    myServo.write(90);
    Serial.println("Baju Masih Basah");
    delay(1000);
  } else {
    myServo.write(0);
    Serial.println("Baju Sudah Kering");
    delay(1000);
  }

  //Stepper
  Serial.println("Berputar");
  myStepper.step(stepsPerRevolution);

  //kirim data ke server
  WiFiClient client;
  //inisialisasi port web server 80 
  
  //kondisi pasti terkoneksi
  //kirim data sensor ke database web
  String Link;
  HTTPClient http;

  Link = "http://" +String(host) + "/halaman/kirimdata.php?suhu=" +String(suhu) + "&kelembaban=" +String(kelembaban) + "&ldr=" +String(ldr) + "&hujan=" +String(data_air);
  
  if (data_air <= 4000) {
    digitalWrite(relay, HIGH);
    myServo1.write(0);
    Serial.println("HUJAN !");
  }
  else {
    digitalWrite(relay, LOW);
    myServo1.write(45);
    Serial.println("Tidak Hujan");
  }

//eksekusi alamat link
  http.begin(Link);
  http.GET();
  //baca respon dari php setelah berhasil kirim nilai sensor
  String respon = http.getString();
  Serial.println(respon);
  http.end();
  
  delay(1000);
}
