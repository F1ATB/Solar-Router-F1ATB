/*
   initGT911 library V1.0.0
   Created by Milad Nikpendar
   Date: 2025-08-18
*/
#ifndef INIT_GT911_H
#define INIT_GT911_H

#include <Arduino.h>
#include <Wire.h>
#include "initGT911_Structs.h"

//#define GT911_Debug_Serial

#ifdef GT911_Debug_Serial
#define GT911_Log(a) Serial.println("[GT911] " + String(a))
#define GT911_Logf(a, ...) Serial.printf(String("[GT911] " + String(a) + "\n").c_str(), ##__VA_ARGS__)
#else
#define GT911_Log(a)
#define GT911_Logf(a, ...)
#endif // GT911_Debug_Serial

// 0x28/0x29 (0x14 7bit)
#define GT911_I2C_ADDR_28 0x14
// 0xBA/0xBB (0x5D 7bit)
#define GT911_I2C_ADDR_BA 0x5D

#define GT911_MAX_CONTACTS 5

#define GT911_REG_CFG 0x8047
#define GT911_REG_CHECKSUM 0x80FF
#define GT911_REG_DATA 0x8140
#define GT911_REG_ID 0x8140
#define GT911_REG_COORD_ADDR 0x814E

enum : uint8_t
{
  GT911_MODE_INTERRUPT,
  GT911_MODE_POLLING
};

class initGT911
{
public:
  enum class Rotate
  {
    _0,
    _90,
    _180,
    _270,
  };

private:
  TwoWire *_wire;
  int8_t _intPin;
  int8_t _rstPin;
  uint8_t _addr;

  bool _configLoaded = false;
  GTConfig _config;
  GTInfo _info;
  GTPoint _points[GT911_MAX_CONTACTS];

  Rotate _rotation = Rotate::_0;

  void reset();
  void i2cStart(uint16_t reg);
  bool write(uint16_t reg, uint8_t data);
  uint8_t read(uint16_t reg);
  bool writeBytes(uint16_t reg, uint8_t *data, uint16_t size);
  bool readBytes(uint16_t reg, uint8_t *data, uint16_t size);
  uint8_t calcChecksum(uint8_t *buf, uint8_t len);
  uint8_t readChecksum();
  int8_t readTouches();
  bool readTouchPoints();

public:
  initGT911(TwoWire *twi = &Wire, uint8_t addr = GT911_I2C_ADDR_BA);
  bool begin(int8_t intPin = -1, int8_t rstPin = -1, uint32_t clk = 400000);
  bool productID(uint8_t *buf, uint8_t len);
  GTConfig *readConfig();
  bool updateConfig();
  GTInfo *readInfo();

  uint8_t touched(uint8_t mode = GT911_MODE_INTERRUPT);
  GTPoint getPoint(uint8_t num);
  GTPoint *getPoints();

  void setupDisplay(uint16_t xRes, uint16_t yRes, Rotate rotation);
};

#endif // INIT_GT911_H
