#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(2);

#define MY_RX_PIN 16  
#define MY_TX_PIN 17  

#define BAUD_RATE 9600    
#define TX_BYTE_COUNT 17    
#define RX_BYTE_COUNT 16    

char message[18] = "KHOAKYTHUATTKVM!1";

void setup() {
  Serial.begin(9600);  
  delay(1000);
  mySerial.begin(BAUD_RATE, SERIAL_8N1, MY_RX_PIN, MY_TX_PIN);
  delay(1000);
  Serial.println("ESP32 UART2 Initialized!");
}

void loop() {
  while(mySerial.available()) {
    mySerial.read();
  }
  
  Serial.println("\nSending 17 bytes to FPGA via UART2...");
  mySerial.write((uint8_t*)message, TX_BYTE_COUNT);
  mySerial.flush();
  
  Serial.print("Sent: ");
  for (int i = 0; i < TX_BYTE_COUNT; i++) {
    Serial.print(message[i]);
  }
  Serial.println();
  
  Serial.println("Waiting for 16 bytes response from FPGA...");
  uint8_t rxBuffer[RX_BYTE_COUNT];
  int count = 0;
  unsigned long startTime = millis();
  
  delay(100);  
  
  while ((millis() - startTime < 3000) && (count < RX_BYTE_COUNT)) {
    if (mySerial.available()) {
      rxBuffer[count++] = mySerial.read();
    }
  }
  
  if (count == RX_BYTE_COUNT) {
    Serial.print("Received: ");
    for (int i = 0; i < RX_BYTE_COUNT; i++) {
      Serial.print((char)rxBuffer[i]);
    }
    Serial.println();
  } else {
    Serial.printf("Timeout or incomplete data: received %d / %d bytes\n", count, RX_BYTE_COUNT);
    Serial.print("Received (HEX): ");
    for (int i = 0; i < count; i++) {
      Serial.printf("%02X ", rxBuffer[i]);
    }
    Serial.println();
  }
  
  delay(5000);
}