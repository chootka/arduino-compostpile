/**
 * Sending Arduino 
 *
 * Will receive an array of bytes via IR and transmit to a Raspberry Pi via NRF24
 */

//============
 
#include <IRremote.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN    6
#define CSN_PIN   7

//============

// Set up nRF24L01 radio on SPI bus pins 6 & 7
RF24 radio(CE_PIN, CSN_PIN);

// Address to write to
const uint64_t txAddr = 0xF0F0F0F0B2LL;

// IR coms
int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Data handling
const int payload_size = 4;
byte send_payload[payload_size+1]; // +1 to allow room for a terminating NULL char
bool newData = false;

//============

void setup(void) {
  
  Serial.begin(9600);
  
  radio.begin();
  
  radio.enableDynamicPayloads();
  
  radio.openWritingPipe(txAddr);

  radio.startListening();

  // Start the IR receiver
  irrecv.enableIRIn();

  Serial.println("Hello from sender");
}

//============

void loop() {
  
  if (irrecv.decode(&results)) {
    handleData(&results);
    irrecv.resume(); // Receive the next value
  }
  
  // Delay so our serial doesn't get overwhelmed
  delay(100);
  
}

//================

void handleData(decode_results *results) {
  
  if (results->decode_type == NEC) {
   
    //============ Convert the data to hex
    convertData(results);

    //============ Transmit via radio
    transmit();

    //============ Etc...
    newData = true;
    showData();
  
  } else {
    Serial.println("Got garbage");
  }
  
}

//================

void convertData(decode_results *results) {

    // read data as HEX
    char hexResults[9];
    String(results->value, HEX).toCharArray(hexResults, 9);

    Serial.print("original HEX results:" );
    Serial.println(hexResults);
    
    // convert to byte array
    unsigned long n = strtoul(hexResults, NULL, 16);
    for(int i=3; i>=0; i--) {       // start with lowest byte of number
      send_payload[i] = n & 0xFF;   // or: = byte( number);
      n >>= 8;                      // get next byte into position
    }

    // add null byte at the end
    send_payload[payload_size+1] = 0;
}

//================

void transmit() {
  
  // First, stop listening so we can talk.
  radio.stopListening();
      
  // Send it via radio back to the RPi. This will block until complete
  radio.write( send_payload, payload_size );
    
  // Now, continue listening
  radio.startListening();
  
}

//================

void showData() {
  
  if (newData == true) {
    Serial.print(F("Now sending length "));
    
    Serial.println(payload_size+1);
    
    Serial.print(F("And payload "));
    
    for(int i=0; i<payload_size+1; i++) {
      Serial.print("0x");
      Serial.println(send_payload[i], HEX);
    }
    newData = false;
  }
  
}

//============

byte getHexVal(char cx) {
  if (cx <= '0') {
    return 0;
  }
  if (cx <= '9') {
    return cx - '0';
  }
  if (cx >= 'A' && cx <= 'F') {
    return cx - 'A' + 10;
  }
  return 0;
}

char getByteAt(char* from, byte offset) {
  return getHexVal(from[offset]) * 16 + getHexVal(from[offset + 1]);
}
