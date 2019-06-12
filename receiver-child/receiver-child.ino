/**
 * Receiving Arduino 
 *
 * Will receive an array of bytes to transmit via IR pulses to a paired Arduino/RF24
 */

//============

#include "IRremote.h"
#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN    6
#define CSN_PIN   7

//============

// Set up nRF24L01 radio on SPI bus pins 6 & 7
RF24 radio(CE_PIN, CSN_PIN);

// Address to read from
const uint64_t rxAddr = 0xF0F0F0F0C3LL;

// IR coms
IRsend irsend;
unsigned long irSignal;

// Data handling
const int payload_size = 4;
char receive_payload[payload_size+1];
bool newData = false;

//============

void setup(void) {

  Serial.begin(9600);

  radio.begin();
  
  radio.openReadingPipe(1, rxAddr);

  radio.enableDynamicPayloads();
  
  radio.startListening();

  Serial.println("Hello from receiver");
}

//============

void loop() {
  
    handleData();
    showData();

    // Delay so our serial doesn't get overwhelmed
    delay(100);
    
}

//============

void handleData() {
  if ( radio.available() ) {
    while ( radio.available() ) {
      
      // ================ Get the data
      uint8_t len = radio.getDynamicPayloadSize();
      
      Serial.print("len: ");
      Serial.println(len);
      
      // If a corrupt dynamic payload is received, it will be flushed
      if(!len){
        continue; 
      }
      
      radio.read( receive_payload, len );

      // add terminating null byte
      receive_payload[payload_size+1] = 0;
    
      // ================ Convert the data to hex
      convertData();

      // ================ Send the data over IR
      sendBytes();
      
      // ================ Etc...
      newData = true;
    }
  }
}

//================

void convertData() {
  char hex[4] = {0};
  char hexBuffer[payload_size*8+1] = {"0x"};
        
  for (int i = 0; i < payload_size; i++) {
    utoa( (unsigned int) receive_payload[i], hex, 16);
    strcat(hexBuffer, hex);
  }
    
  Serial.print("hexBuffer: ");
  Serial.println(hexBuffer);
    
  irSignal = strtoul(hexBuffer, 0, 16);
}

//================

void sendBytes() {
  if (sizeof(irSignal) > 0) {
    irsend.sendNEC(irSignal, 32); // 32 == bits, not bytes. so this is 4 bytes (* 8 bits)
  }
}

//================

void showData() {
  if (newData == true) {
    Serial.print(F("Got response size="));
    Serial.print(sizeof(receive_payload));
    Serial.print(F(" value="));
    Serial.println(receive_payload);
    newData = false;
  }
}
