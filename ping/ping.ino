/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 7); // CE, CSN

const uint64_t address = 0xF0F0F0F0C1LL;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.println("hello from ping!");
}
void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  
  delay(1000);
}
