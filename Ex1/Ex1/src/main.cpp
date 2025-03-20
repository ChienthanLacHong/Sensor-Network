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
// #include <HardwareSerial.h>
// #include <CRC16.h>

// HardwareSerial mySerial(2);

// #define MY_RX2 16 
// #define MY_TX2 17  

// #define FRAME_START 0xAA
// #define FRAME_END   0x55
// #define BLOCK_SIZE  17 
// #define TIMEOUT     2000 

// uint8_t message[BLOCK_SIZE] = {'H', 'O', 'C', 'V', 'I', 'E', 'N', 'B', 'U', 'U', 'C', 'H', 'I', 'N', 'H', '!', '1'};

// uint8_t frame[2 + BLOCK_SIZE + 2 + 1]; 
// uint8_t rxBuffer[128]; 

// uint16_t calculateCRC(uint8_t *data, uint16_t length) {
//     CRC16 crc;
//     crc.setPolynome(0x8005);
//     crc.add(data, length);
//     return crc.calc(); 
// }

// void sendBit(uint8_t byte) {
//     for (int i = 7; i >= 0; i--) {  
//         uint8_t bitToSend = (byte >> i) & 0x01;
//         mySerial.write(bitToSend);  
//         Serial.printf("%d", bitToSend);
//         delayMicroseconds(100);  
//     }
//     Serial.print(" ");  
// }

// void sendFrame(uint8_t *data, uint8_t numBlocks) {
//     frame[0] = FRAME_START;
//     uint16_t frameLength = numBlocks * BLOCK_SIZE;
//     frame[1] = (frameLength >> 8) & 0xFF;
//     frame[2] = frameLength & 0xFF;

//     memcpy(&frame[3], data, BLOCK_SIZE);

//     uint16_t crc = calculateCRC(&frame[1], frameLength + 2);
//     frame[3 + frameLength] = (crc >> 8) & 0xFF;
//     frame[4 + frameLength] = crc & 0xFF;
//     frame[5 + frameLength] = FRAME_END;

//     Serial.println("Sent Frame to FPGA:");
//     for (int i = 0; i < 6 + frameLength; i++) {
//         Serial.printf("%02X ", frame[i]);
//     }
//     Serial.println("\n");

//     Serial.println("Sent as String:");
//     for (int i = 3; i < 3 + frameLength; i++) {
//         Serial.printf("%c", frame[i]);
//     }
//     Serial.println("\n");

//     Serial.println("Sent Bits to FPGA:");
//     for (int i = 0; i < 6 + frameLength; i++) {
//         sendBit(frame[i]);
//     }
//     Serial.println("\n");

//     mySerial.write(frame, 6 + frameLength);
// }

// bool receiveFrame(uint8_t *buffer, uint16_t &length) {
//     uint32_t startTime = millis();
//     uint16_t index = 0;

//     Serial.println("Waiting for FPGA response...");

//     while (millis() - startTime < TIMEOUT) {
//         if (mySerial.available()) {
//             uint8_t byteReceived = mySerial.read();
//             Serial.printf("Received Byte %d: 0x%02X\n", index, byteReceived);

//             if (index < sizeof(rxBuffer)) {
//                 buffer[index++] = byteReceived;
//             } else {
//                 Serial.println("Buffer overflow! Frame too large.");
//                 return false;
//             }

//             if (index == 3) {
//                 length = (buffer[1] << 8) | buffer[2];  
//                 Serial.printf("Frame Length: %d bytes\n", length);
//                 if (length + 6 > sizeof(rxBuffer)) {
//                     Serial.println("Invalid frame length!");
//                     return false;
//                 }
//             }

//             if (index >= length + 6) {
//                 Serial.println("Checking Frame...");

//                 if (buffer[0] != FRAME_START || buffer[index - 1] != FRAME_END) {
//                     Serial.println("Invalid Frame Start/End!");
//                     return false;
//                 }

//                 uint16_t receivedCRC = (buffer[index - 3] << 8) | buffer[index - 2];
//                 uint16_t calculatedCRC = calculateCRC(&buffer[1], length + 2);
//                 Serial.printf("Received CRC: 0x%04X | Calculated CRC: 0x%04X\n", receivedCRC, calculatedCRC);

//                 if (receivedCRC != calculatedCRC) {
//                     Serial.println("CRC Error!");
//                     return false;
//                 }

//                 Serial.println("Frame Received Successfully!");
//                 return true;
//             }
//         }
//     }
//     Serial.println("Timeout Waiting for FPGA Response!");
//     return false;
// }

// void setup() {
//     Serial.begin(9600);
//     mySerial.begin(9600, SERIAL_8N1, MY_RX2, MY_TX2);
//     delay(2000);
//     Serial.println("ESP32 UART Initialized!");
// }

// void loop() {
//     sendFrame(message, 1);

//     uint16_t rxLength;
//     if (receiveFrame(rxBuffer, rxLength)) {
//         Serial.println("Received Response from FPGA:");
//         for (int i = 0; i < rxLength + 6; i++) {
//             Serial.printf("%02X ", rxBuffer[i]);
//         }
//         Serial.println("\n");

//         Serial.println("Received as String:");
//         for (int i = 3; i < 3 + rxLength; i++) {
//             Serial.printf("%c", rxBuffer[i]);
//         }
//         Serial.println("\n");
//     }

//     delay(5000);
// }