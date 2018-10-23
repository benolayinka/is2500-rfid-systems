#include <Wire.h>
#include <EEPROMex.h>
#include <stdlib.h>
#include <stdio.h>
#define EEPROM_BASE_ADDR 0xF

#define NXP_ADDR 0x55
#define VIEWS_EEPROM_ADDRESS 0
#define BLOCK_SIZE 16
#define FIND_BLOCKS 7

int findviewstringstart(byte * buffer, int length)
{
  for(int i = 0; i<length; i++)
  {
    if (buffer[i] == 'w')
    {
      if(buffer[i+1] == 's')
      {
        if (buffer[i+2] = '=') return i+3;
      }
    }
  }
  return -1;
}

void findandreplace()
{
  
}

void readblock(byte address, byte * buffer)
{
  Wire.beginTransmission(NXP_ADDR);
  Wire.write((byte)(address));
  Wire.endTransmission();
  Wire.requestFrom(NXP_ADDR,BLOCK_SIZE);
  Wire.readBytes(buffer, BLOCK_SIZE);
  return;
}

void writeblock(byte address, byte * buffer)
{
  Wire.beginTransmission(NXP_ADDR);
  Wire.write((byte)(address));
  Wire.write(buffer, BLOCK_SIZE);
  Wire.endTransmission();
}

void setup() {
  ADCSRA = 0;
  CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
  //CLKPR = _BV(CLKPS0);  // divide frequency by 2
  CLKPR = 0b0011; // divide frequency by 8
  
  int views = 0;
  views = EEPROM.readInt(VIEWS_EEPROM_ADDRESS);
  views++;
  EEPROM.writeInt(VIEWS_EEPROM_ADDRESS, views);
  Wire.begin();        // join i2c bus (address optional for master)

  #ifdef DEBUG
  Serial.begin(9600);  // start serial for output
  Serial.print("views: ");
  Serial.println(views);
  #endif

  byte buffer[BLOCK_SIZE];
  byte memblock[BLOCK_SIZE*FIND_BLOCKS];
  byte block;
  char views_string[5];
  
  if(views<10000)
  {
    snprintf (views_string, 5, "%04d", views);
  }
  else EEPROM.writeInt(VIEWS_EEPROM_ADDRESS, 0);
  
  block = 0;
  int viewstringstart = 0;
  
  //dump FIND_BLOCKS blocks to memory
  for (byte i=0; i<FIND_BLOCKS; i++)
  {
    readblock(i, buffer);
    memcpy(memblock+i*BLOCK_SIZE, buffer, BLOCK_SIZE);
  }
  
  //find views string
  viewstringstart = findviewstringstart(memblock, BLOCK_SIZE*FIND_BLOCKS);

  #ifdef DEBUG
  Serial.print("Views found at: ");
  Serial.println(viewstringstart);
  Serial.println((char)memblock[viewstringstart-2]);
  Serial.println((char)memblock[viewstringstart-1]);
  Serial.println((char)memblock[viewstringstart]);
  Serial.println((char)memblock[viewstringstart+1]);
  Serial.println((char)memblock[viewstringstart+2]);
  Serial.println((char)memblock[viewstringstart+3]);
  #endif
  
  //replace string with views
  for (int i=viewstringstart; i<viewstringstart+4; i++)
  {
    memblock[i]=views_string[i-viewstringstart];
  }

  #ifdef DEBUG
  for (byte i=0; i<FIND_BLOCKS; i++)
  {
    for (byte j=0; j<BLOCK_SIZE; j++)
    {
      Serial.print((char)memblock[j+i*BLOCK_SIZE]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  #endif
  
  //write block back
  for (byte i=1; i<FIND_BLOCKS; i++)
  {
    memcpy(buffer, memblock+i*BLOCK_SIZE, BLOCK_SIZE);
    writeblock(i, buffer);
    delay(10);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  for(int i=0; i<7; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(5);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(5); 
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
}
  
