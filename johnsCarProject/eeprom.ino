#include <EEPROM.h>

#define EEPROM_ADDR_DST_OFFSET 0x00
#define EEPROM_ADDR_METRIC     0x01

int8_t toggleEEPROM(int addr)
{
  int8_t value = EEPROM.read(addr);
  if(value == 0) {
    value = 1;
  } else {
    value = 0;
  }
  EEPROM.write(addr, value);
  return value;
}





char getDSTOffset()
{
  return EEPROM.read(EEPROM_ADDR_DST_OFFSET);
}

char toggleDSTOffset()
{
  return toggleEEPROM(EEPROM_ADDR_DST_OFFSET);
}



char getMPHMode()
{
  return EEPROM.read(EEPROM_ADDR_METRIC);
}

char toggleMPHMode()
{
  return toggleEEPROM(EEPROM_ADDR_METRIC);
}

