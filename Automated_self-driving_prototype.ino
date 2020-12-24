
#include <SPI.h>
#include <MFRC522.h>
#include "HX711.h"

#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);


///////////////ultrasonic sensor 1//////////////////////////////

int trigPin1 = 22;
int echoPin1 = 23;

long duration1;
int distance1;

//////////////ultrasonic snesor 2//////////////////////////////

int trigPin2 = 24;
int echoPin2 = 25;

long duration2;
int distance2;

//////////////ultrasonic sensor 3///////////////////////////

int trigPin3 = 26;
int echoPin3 = 27;

long duration3;
int distance3;

///////////////////////motor 1////////////////////////////

int Speed1 = 9;
int clockwise1 = 8;
int anticlockwise1 = 7;

////////////////////////motor 2///////////////////////////
int Speed2 = 10;
int clockwise2 = 12;
int anticlockwise2 = 13;

/////////////////////////Rfid////////////////////////////

int Status = 0;

///////////////Loadcell///////////////////////

HX711 scale(3, 2);

float calibration_factor = -469;
float units;
float ounces;


void setup() {
  pinMode(trigPin1, OUTPUT);  //declare pin trigPin1 as output
  pinMode(echoPin1, INPUT);   //declare echoPin1 as input

  pinMode(trigPin2, OUTPUT);    //declare pin trigPin2 as output
  pinMode(echoPin2, INPUT);   //declare echoPin2 as input

  pinMode(trigPin3, OUTPUT);    //declare pin trigPin3 as output
  pinMode(echoPin3, INPUT);   //declare echoPin3 as input

  pinMode(Speed1, OUTPUT);  //declare pin Speed1 as output
  pinMode(clockwise1, OUTPUT);  //declare pin clockwise1 as output
  pinMode(anticlockwise1, OUTPUT);  //declare pin anticlockwise1 as output

  pinMode(Speed2, OUTPUT);  //declare pin Speed1 as output
  pinMode(clockwise2, OUTPUT);  //declare pin clockwise2  as output
  pinMode(anticlockwise2, OUTPUT);  //declare pin anticlockwise2 as output
  Serial.begin(9600);   //initialize serial monitor
  //  while (!Serial);
  SPI.begin();    //initialize the SPI bus
  mfrc522.PCD_Init();   //initialize MFRC522
  Serial.println("Put your card to the reader...");
  Serial.println();


  Serial.begin(9600);
  Serial.println("HX711 weighing");
  scale.set_scale(calibration_factor);    //adjust to this calibration factor
  scale.tare();
  Serial.println("Readings:");

}

void loop() {

  read_card();
  load_cell();

  while (Status == 1) {
    read_card();
    if (Status == 0 || units >= 900) { //berat di load cell melebihi had muatan (900g) dan rfid tidak memberi akses
      Serial.print("Break loop");
      MotorStop();
      break;
    }
    else if ( Status == 1 || units >= 0) {  //berat di load cell tidak melebihi had muatan dan rfid masih memberi akses
      load_cell();
      ultrasonic();
      mainfunction();
    }
  }

  Serial.print("Status = ");
  Serial.println(Status);
}

void read_card() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print("UID tag :");
  String content = "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "Put your UID card here") //tuksr UID dengan UID kad yang ingin diberikan akses
  {

    if (Status == 0) {  //Rfid memberi akses
      Status = 1;
      Serial.println("Authorized access");
      Serial.println();
      delay(1000);
    }
    else {
      Status = 0;
      Serial.println("Stop function");    //Rfid tidak memberi akses dan memberhentikan cart sekiranya di scan sekali lagi
      delay(1000);
    }
  }
  else   {
    Serial.println(" Access denied");   //Rfid tidak memberi akses kepada UID yang tidak dikenali

  }
}
//////////////////Load cell///////////////

void load_cell() {
  Serial.print("Reading:");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }


  Serial.print(units);
  Serial.println(" grams");
  delay(1000);

}

//////////////mainfuntion/////////////////
void mainfunction() {
  if (distance2 <= 50 && distance2 > 15) { //range sensor detect adalah 15cm ke 50cm
    MotorLeft();
    Serial.print("turning right");
  }
  else if (distance3 <= 50 && distance3 > 15) { //range sensor detect adalah 15cm ke 50cm
    MotorRight();
    Serial.print("turning left");
  }


  else if (distance1 <= 50 && distance1 > 15) { //range sensor detect adalah 15cm ke 50cm
    MotorFoward();
  }
  else {
    MotorStop();
  }

}

//////fungsi motor kehadapan//////////////
void MotorFoward() {
  Serial.print("depan");
  digitalWrite(clockwise1, HIGH);   //mengarahkan motor 1 untuk berputar kearah clockwise
  digitalWrite(anticlockwise1, LOW);  //mengarahkan motor 1 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed1, 180);   //menetapkan kelajuan motor 1

  digitalWrite(clockwise2, HIGH);   //mengarahkan motor 2 untuk berputar kearah clockwise
  digitalWrite(anticlockwise2, LOW);  //mengarahkan motor 2 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed2, 180);   //menetapkan kelajuan motor 2
}
///////fungsi motor kekanan//////////////
void MotorRight() {
  Serial.print("kanan");
  digitalWrite(clockwise1, LOW);   //mengarahkan motor 1 untuk tidak berputar kearah clockwise
  digitalWrite(anticlockwise1, LOW);  //mengarahkan motor 1 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed1, 0);   //menetapkan kelajuan motor 1

  digitalWrite(clockwise2, HIGH);    //mengarahkan motor 2 untuk berputar ke arah clockwise
  digitalWrite(anticlockwise2, LOW);   //mengarahkan motor 2 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed2, 180);   //menetapkan kelajuan motor 2
}
//////fungsi motor kekiri//////////////
void MotorLeft() {
  Serial.print("kiri");
  digitalWrite(clockwise1, HIGH);   //mengarahkan motor 1 untuk berputar kearah clockwise
  digitalWrite(anticlockwise1, LOW);  //mengarahkan motor 1 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed1, 180);   //menetapkan kelajuan motor 1

  digitalWrite(clockwise2, LOW);   //mengarahkan motor 2 untuk berputar ke arah clockwise
  digitalWrite(anticlockwise2, LOW);  //mengarahkan motor 2 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed2, 0);   //menetapkan kelajuan motor 2
}
//////fungsi motor berhenti//////////////
void MotorStop() {
  Serial.print("stop");
  digitalWrite(clockwise1, LOW);  //mengarahkan motor 1 untuk tidak berputar kearah clockwise
  digitalWrite(anticlockwise1, LOW);  //mengarahkan motor 1 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed1, 0);    //menetapkan kelajuan motor 1

  digitalWrite(clockwise2, LOW);  //mengarahkan motor 2 untuk tidak berputar ke arah clockwise
  digitalWrite(anticlockwise2, LOW);  //mengarahkan motor 2 untuk tidak berputar ke arah anticlockwise
  analogWrite(Speed2, 0);   //menetapkan kelajuan motor 2
}
////////////ultrasonic 1,2,3 reading/////////
void ultrasonic() {
  Serial.print("ultasonic chechking");
  Ultrasoniccheck1();
  Ultrasoniccheck2();
  Ultrasoniccheck3();
  delay(50);
}
////////////ultrasonic1 reading/////////
void Ultrasoniccheck1() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2; //convert to cm
  Serial.print("Distance1:");
  Serial.println(distance1);
}
////////////ultrasonic2 reading/////////
void Ultrasoniccheck2() {
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2; //convert to cm

  Serial.print("Distance2:");
  Serial.println(distance2);
}
////////////ultrasonic3 reading/////////
void Ultrasoniccheck3() {
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);

  duration3 = pulseIn(echoPin3, HIGH);
  distance3 = duration3 * 0.034 / 2; //convert to cm

  Serial.print("Distance3:");
  Serial.println(distance3);

}
