#include <Adafruit_ESP8266.h>
// Must call begin() on the stream(s) before using Adafruit_ESP8266 object.
#define ESP_RX   12
#define ESP_TX   13
#define ESP_RST  11
#define ESP_SSID "admin" // Your network name here
#define ESP_PASS "dathaymatkhau" // Your network password here
#define HOST     "LINE1|MECH1|POWF|0"     // Host to contact
#define SERVER   "184.106.153.149" // api.thingspeak.com
#define PORT     80                     // 80 = HTTP default port
#define SERVERIP "192.168.1.91"
// replace with your channel's thingspeak API key
#define apiReadKey "W68W3JCEG0AHU7CL"
#define apiWriteKey "2QVOB3V2KNRR0EWQ"
#define channel "80425"
//Leonardo
//Serial_ & dbgTerminal = Serial;
//HardwareSerial & espSerial = Serial1;

////UNO & M328P
//#include <SoftwareSerial.h>
//SoftwareSerial espSerial(10, 11); // RX, TX
//HardwareSerial & monitor = Serial;

//MEGA2560 & Due
HardwareSerial & monitor = Serial;
HardwareSerial & espSerial = Serial1;

#define BUFFER_SIZE 128
// Variables will change:
char buf[BUFFER_SIZE];
int result = 0;
boolean OK = true;
boolean TIMEOUT = false;
int i;
int m = 0;
int lp = 0;
Adafruit_ESP8266 server(&espSerial, &Serial, ESP_RST);

char inputChar[50];
//String ssid = "Nha Song";
//String pwd = "nhasongty";
//String ssid = "admin";
//String pwd = "dathaymatkhau";
String ssid = "ABCDE";
String pwd = "khongcopass";
//String ssid = "binh";
//String pwd = "123456789";
String serverip = "192.168.1.91";
String port = "8080";
String cmd;
int ch_id, packet_len;
char *pb;
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  //monitor and AT command serial init
  monitor.begin(115200);           // Serial monitor
  espSerial.begin(115200);             // ESP8266
  monitor.println(F("http://www.nguyenthaibinh.xyz"));

  // Hard reset
  //  Serial.print(F("Hard reset..."));
  //  result = server.hardReset();
  //  delay(5000);

  // Hard reset
  Serial.print(F("Hard reset..."));
  do
  {
    result = server.hardReset();
    delay(1000);
  }
  while (result);
  // Soft reset
  Serial.print(F("Soft reset..."));
  do
  {
    result = server.softReset();
    delay(2000);
  }
  while (!result);
  // Quit current access point
  Serial.print(F("Quit access point..."));
  do
  {
    result = server.closeAP();
    delay(500);
  }
  while (!result);

CONNECT_WIFI:

  cmd = "AT+CWJAP=" + String("\"") + ssid + String("\"") + String(",") + String("\"") + pwd + String("\"");
  espSerial.println(cmd);                    // Connect AP
  monitor.print("\n" + cmd);
  delay(4000);
  result = CheckResponse();                         // Check "OK"
  if (result == OK) {
    Serial.print(" OK");
    delay(300);
  }
  else {
    Serial.print(" ERROR");
    delay(1000);
    goto CONNECT_WIFI;
  }
//  digitalWrite(42, HIGH);
  pinMode(42, INPUT);    
  digitalWrite(34, HIGH);
  pinMode(34, OUTPUT);  
}

void loop() {
  buttonState = digitalRead(42);
  if (buttonState != HIGH) {
    sendErrorStatus("Power low");
    delay(10000);
  }  
  listenPowerStatus();
  delay(2000);
}

void listenPowerStatus() {
  // Connect server
  do
  {
    result = server.connectTCP(F(SERVER), PORT);
    delay(200);
  }
  while (!result);
  // prepare GET string
  String getStr = "GET /channels/";
  getStr += channel;
  getStr += "/fields/1/last";
  getStr += "\r\n\r\n";
  // Send
  if (server.requestURL(getStr)) {
    do {
      delay(50);
      espSerial.readBytesUntil('\n', buf, BUFFER_SIZE);
    }
    while (espSerial.available() > 0);
  }
//  clearSerialBuffer();
  monitor.println(String(String(buf).charAt(7)));
  if (String(String(buf).charAt(7)) == "1") {
    digitalWrite(34, LOW);
  }
  else {
    digitalWrite(34, HIGH);
  }  
}

void sendErrorStatus(String stat) {
  // Connect server
  do
  {
    result = server.connectTCP(F(SERVER), PORT);  
    delay(500); 
  }
  while (!result);
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiWriteKey;
  getStr += "&field1=";
  getStr += stat;
  getStr += "\r\n\r\n";  
  // Send
  result = server.requestURL(getStr);     
//  clearSerialBuffer();
}

void clearSerialBuffer(void) {
  while ( espSerial.available() > 0 ) {
    espSerial.read();
  }
}
void clearBuffer(void) {
  for (int i = 0; i < BUFFER_SIZE; i++ ) {
    buf[i] = 0;
  }
}
int readSerial() {
  int i = 0;
  while (1) {
    while (espSerial.available() > 0) {
      char inChar = espSerial.read();

      if (i > 0) {
        if (inChar == '\n') {
          inputChar[i] = '\0';
          espSerial.flush();
          return 0;
          //break;
        }
      }
      if (inChar != '\r') {
        inputChar[i] = inChar;
        i++;
      }
    }
  }
}

boolean CheckResponse() {
  if (espSerial.find("K")) {     // Check "OK"
    return OK;
  }
  else {
    return TIMEOUT;
  }
}

boolean CheckContinueSend() {
  if (espSerial.find(">")) {     // Check "OK"
    return OK;
  }
  else {
    return TIMEOUT;
  }
}
