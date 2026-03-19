#include "initGT911.h"
#include <Wire.h>

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

// Interrupt handling
volatile bool gt911IRQ = false;

#if defined(ESP8266)
void ICACHE_RAM_ATTR _gt911_irq_handler()
{
  noInterrupts();
  gt911IRQ = true;
  interrupts();
}
#elif defined(ESP32)
void IRAM_ATTR _gt911_irq_handler()
{
  noInterrupts();
  gt911IRQ = true;
  interrupts();
}
#else
void _gt911_irq_handler()
{
  noInterrupts();
  gt911IRQ = true;
  interrupts();
}
#endif

initGT911::initGT911(TwoWire *twi, uint8_t addr) : _wire(twi ? twi : &Wire)
{
  _addr = addr;
}

void initGT911::reset()
{
  delay(1);

  pinMode(_intPin, OUTPUT);
  pinMode(_rstPin, OUTPUT);

  digitalWrite(_intPin, LOW);
  digitalWrite(_rstPin, LOW);

  delay(11);

  digitalWrite(_intPin, _addr == GT911_I2C_ADDR_28);

  delayMicroseconds(110);
  pinMode(_rstPin, INPUT);

  delay(6);
  digitalWrite(_intPin, LOW);

  delay(51);
}

void initGT911::i2cStart(uint16_t reg)
{
  _wire->beginTransmission(_addr);
  _wire->write(reg >> 8);
  _wire->write(reg & 0xFF);
}

bool initGT911::write(uint16_t reg, uint8_t data)
{
  i2cStart(reg);
  _wire->write(data);
  return _wire->endTransmission() == 0;
}

uint8_t initGT911::read(uint16_t reg)
{
  i2cStart(reg);
  if (_wire->endTransmission() != 0)
  {
    GT911_Log("I2C read single byte: endTransmission error");
    return 0;
  }

  uint8_t got = _wire->requestFrom((int)_addr, 1);
  if (got == 0 || !_wire->available())
  {
    GT911_Log("I2C read single byte: no data");
    return 0;
  }
  return _wire->read();
}

bool initGT911::writeBytes(uint16_t reg, uint8_t *data, uint16_t size)
{
  i2cStart(reg);
  for (uint16_t i = 0; i < size; i++)
  {
    _wire->write(data[i]);
  }
  return _wire->endTransmission() == 0;
}

bool initGT911::readBytes(uint16_t reg, uint8_t *data, uint16_t size)
{
  if (size == 0)
    return true;

  // Start write of register pointer
  i2cStart(reg);
  if (_wire->endTransmission() != 0)
  {
    GT911_Log("readBytes I2C error: endTransmission");
    return false; // I2C error
  }

  uint16_t index = 0;
  const unsigned long overallTimeout = 2000; // ms
  unsigned long startTime = millis();

  while (index < size)
  {
    if ((millis() - startTime) > overallTimeout)
    {
      GT911_Log("readBytes timeout");
      return false;
    }

    uint8_t req = (uint8_t)min<uint16_t>(size - index, I2C_BUFFER_LENGTH);
    uint8_t got = _wire->requestFrom((int)_addr, (int)req);

    if (got == 0)
    {
      // small backoff and retry once
      delay(5);
      yield();
      got = _wire->requestFrom((int)_addr, (int)req);
      if (got == 0)
      {
        GT911_Log("I2C read error: no data available after retry");
        return false;
      }
    }

    uint8_t readCount = 0;
    while (readCount < got && index < size)
    {
      if (_wire->available())
      {
        data[index++] = _wire->read();
        readCount++;
      }
      else
      {
        // unexpected early end; break to outer loop to retry remaining
        break;
      }
    }

    // small yield to avoid WDT and give bus time
    delayMicroseconds(50);
    yield();
  }

  GT911_Logf("readBytes: read %d bytes\n", index);
  return index == size;
}

uint8_t initGT911::calcChecksum(uint8_t *buf, uint8_t len)
{
  uint8_t ccsum = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    ccsum += buf[i];
  }

  return (~ccsum) + 1;
}

uint8_t initGT911::readChecksum()
{
  return read(GT911_REG_CHECKSUM);
}

int8_t initGT911::readTouches()
{
  uint32_t timeout = millis() + 20;
  do
  {
    uint8_t flag = read(GT911_REG_COORD_ADDR);
    if ((flag & 0x80) && ((flag & 0x0F) < GT911_MAX_CONTACTS))
    {
      write(GT911_REG_COORD_ADDR, 0);
      return flag & 0x0F;
    }
    delay(1);
  } while (millis() < timeout);

  return 0;
}

bool initGT911::readTouchPoints()
{
  bool result = readBytes(GT911_REG_COORD_ADDR + 1, (uint8_t *)_points, sizeof(GTPoint) * GT911_MAX_CONTACTS);

  if (result && _rotation != Rotate::_0)
  {
    for (uint8_t i = 0; i < GT911_MAX_CONTACTS; i++)
    {
      if (_rotation == Rotate::_180)
      {
        _points[i].x = _info.xResolution - _points[i].x;
        _points[i].y = _info.yResolution - _points[i].y;
      }
    }
  }

  return result;
}

bool initGT911::begin(int8_t intPin, int8_t rstPin, uint32_t clk)
{
  _intPin = intPin;
  _rstPin = rstPin;

  if (_rstPin > 0)
  {
    delay(300);
    reset();
    delay(200);
  }

  if (intPin > 0)
  {
    pinMode(_intPin, INPUT);
    attachInterrupt(_intPin, _gt911_irq_handler, FALLING);
  }

  _wire->begin();
  _wire->setClock(clk);
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() == 0)
  {
    readInfo(); // Need to get resolution to use rotation
    return true;
  }
  return false;
}

bool initGT911::productID(uint8_t *buf, uint8_t len)
{
  if (len < 4)
  {
    return false;
  }

  memset(buf, 0, 4);
  return readBytes(GT911_REG_ID, buf, 4);
}

GTConfig *initGT911::readConfig()
{
  readBytes(GT911_REG_CFG, (uint8_t *)&_config, sizeof(_config));

  if (readChecksum() == calcChecksum((uint8_t *)&_config, sizeof(_config)))
  {
    _configLoaded = true;
    return &_config;
  }
  return nullptr;
}

bool initGT911::updateConfig()
{
  uint8_t checksum = calcChecksum((uint8_t *)&_config, sizeof(_config));

  if (_configLoaded && readChecksum() != checksum)
  { // Config is different
    writeBytes(GT911_REG_CFG, (uint8_t *)&_config, sizeof(_config));

    uint8_t buf[2] = {checksum, 1};
    writeBytes(GT911_REG_CHECKSUM, buf, sizeof(buf));
    delay(10); // Wait for config to be applied
    return true;
  }
  return false;
}

GTInfo *initGT911::readInfo()
{
  readBytes(GT911_REG_DATA, (uint8_t *)&_info, sizeof(_info));
  return &_info;
}

uint8_t initGT911::touched(uint8_t mode)
{
  bool irq = false;
  if (mode == GT911_MODE_INTERRUPT)
  {
    noInterrupts();
    irq = gt911IRQ;
    gt911IRQ = false;
    interrupts();
  }
  else if (mode == GT911_MODE_POLLING)
  {
    irq = true;
  }

  uint8_t contacts = 0;
  if (irq)
  {
    contacts = readTouches();

    if (contacts > 0)
    {
      readTouchPoints();
    }
  }

  return contacts;
}

GTPoint initGT911::getPoint(uint8_t num)
{
  return _points[num];
}

GTPoint *initGT911::getPoints()
{
  return _points;
}

void initGT911::setupDisplay(uint16_t xRes, uint16_t yRes, Rotate rotation)
{
  _rotation = rotation;

  GTConfig *cfg = readConfig();
  if (cfg == nullptr)
  {
    GT911_Log("Config Read Error");
    return;
  }

  cfg->hSpace = (5 | (5 << 4)); // Set horizontal space
  cfg->vSpace = (5 | (5 << 4)); // Set vertical space

  cfg->xResolution = xRes; // Set X resolution
  cfg->yResolution = yRes; // Set Y resolution

  //cfg->moduleSwitch1 &= ~(1 << 7); // Set bit 7 to 0 to invert X-axis
  //cfg->moduleSwitch1 &= ~(1 << 6); // Set bit 6 to 0 to invert Y-axis

  cfg->moduleSwitch1 |= (1 << 7); // Set bit 7 to 1 to invert X-axis if not in 0 rotation
  cfg->moduleSwitch1 |= (1 << 6); // Set bit 6 to 1 to invert Y-axis if not in 0 rotation

  updateConfig();
}
