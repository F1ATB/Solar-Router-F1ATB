#define LGFX_USE_V1
#include <LovyanGFX.hpp>

//******************************************
// Equivalence ESP32_Type et ScreenType
//******************************************
enum ScreenType {
  S028_SCREEN_ILI9341_BL21 = 4,
  S028_SCREEN_ST7789_BL21 = 5,
  S024_SCREEN_ILI9341_BL27 = 6,
  S024_SCREEN_ST7789_BL27 = 7,
  S024C_SCREEN_ILI9341_BL27 = 8,
  S028C_JC2432W328_ST7789_BL27 = 9
};

#define LCD_MOSI 13
#define LCD_MISO 12
#define LCD_SCK 14
#define LCD_CS 15
#define LCD_RST -1
#define LCD_DC 2
#define LCD_BL_27 27
#define LCD_BL_21 21


//Resistive Touch
#define TOUCH_CS 33
#define TOUCH_IRQ 36
//pour 024
#define S024_TOUCH_MOSI 13
#define S024_TOUCH_MISO 12
#define S024_TOUCH_SCK 14
//pour 028
#define S028_TOUCH_MOSI 32
#define S028_TOUCH_MISO 39
#define S028_TOUCH_SCK 25

//Capacitive touch
#define I2C_SDA 33  // Touch screen SDA pin 
#define I2C_SCL 32  // Touch screen SCL pin
#define TP_RST 25   // Touch screen reset pin
#define TP_INT 21   // Touch screen interrupt pin


class LGFX : public lgfx::LGFX_Device {
private:
  lgfx::Bus_SPI _bus;
  lgfx::Panel_Device* _panel = nullptr;
  lgfx::Light_PWM _light;
  lgfx::Touch_XPT2046 _touch;  //Touch résistif

public:
  LGFX(byte ESP32type) {
    ScreenType type = (ScreenType)ESP32type;
    initBus();
    initPanel(type);
    initBacklight(type);
    initTouch(type);
  }

private:
  void initBus() {
    auto cfg = _bus.config();
    cfg.spi_host = HSPI_HOST;
    cfg.spi_mode = 3;  // 3 pour ST7789, 0 pour ILI9341(? à tester)
    cfg.freq_write = 40000000;
    cfg.freq_read = 16000000;
    cfg.spi_3wire = true;
    cfg.use_lock = true;
    cfg.dma_channel = 1;
    cfg.pin_sclk = LCD_SCK;
    cfg.pin_mosi = LCD_MOSI;
    cfg.pin_miso = LCD_MISO;
    cfg.pin_dc = LCD_DC;
    _bus.config(cfg);
  }

  void initPanel(ScreenType type) {
    if (type == S028_SCREEN_ST7789_BL21 || type == S024_SCREEN_ST7789_BL27 || type == S028C_JC2432W328_ST7789_BL27) {
      auto* p = new lgfx::Panel_ST7789();
      auto cfg = p->config();
      cfg.pin_cs = LCD_CS;
      cfg.pin_rst = LCD_RST;
      cfg.pin_busy = -1;
      cfg.memory_width = 240;
      cfg.memory_height = 320;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      p->config(cfg);
      p->setBus(&_bus);
      _panel = p;
    } else if (type == S028_SCREEN_ILI9341_BL21 || type == S024_SCREEN_ILI9341_BL27 || type == S024C_SCREEN_ILI9341_BL27) {
      auto* p = new lgfx::Panel_ILI9341();
      auto cfg = p->config();
      cfg.pin_cs = LCD_CS;
      cfg.pin_rst = LCD_RST;
      cfg.pin_busy = -1;
      cfg.memory_width = 240;
      cfg.memory_height = 320;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      p->config(cfg);
      p->setBus(&_bus);
      _panel = p;
    }

    setPanel(_panel);
  }
  void initBacklight(ScreenType type) {
    auto cfg = _light.config();
    if (type == S024_SCREEN_ILI9341_BL27 || type == S024_SCREEN_ST7789_BL27 || type == S024C_SCREEN_ILI9341_BL27 || type== S028C_JC2432W328_ST7789_BL27) {
      cfg.pin_bl = LCD_BL_27;
    } else {
      cfg.pin_bl = LCD_BL_21;
    }
    cfg.invert = false;
    cfg.freq = 44100;
    cfg.pwm_channel = 7;
    _light.config(cfg);
    _panel->setLight(&_light);
  }


  void initTouch(ScreenType type) {
    if (type != S024C_SCREEN_ILI9341_BL27  && type !=S028C_JC2432W328_ST7789_BL27) { //Touch resistifs seulement
      auto cfg = _touch.config();
      cfg.x_min = 0;
      cfg.x_max = 239;
      cfg.y_min = 0;
      cfg.y_max = 319;
      cfg.pin_int = TOUCH_IRQ;
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;


      cfg.freq = 1000000;
      if (type == S028_SCREEN_ILI9341_BL21 || type == S028_SCREEN_ST7789_BL21) {
        cfg.spi_host = VSPI_HOST;
        cfg.pin_sclk = S028_TOUCH_SCK;
        cfg.pin_mosi = S028_TOUCH_MOSI;
        cfg.pin_miso = S028_TOUCH_MISO;
      } else {
        cfg.spi_host = HSPI_HOST;
        cfg.pin_sclk = S024_TOUCH_SCK;
        cfg.pin_mosi = S024_TOUCH_MOSI;
        cfg.pin_miso = S024_TOUCH_MISO;
      }
      cfg.pin_cs = TOUCH_CS;

      _touch.config(cfg);
      _panel->setTouch(&_touch);
    }
  }
};
