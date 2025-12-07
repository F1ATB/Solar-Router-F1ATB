#include <Arduino.h>
#include <AsyncUDP.h>
#include <WiFi.h>

#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson
#include <FastCRC32.h>    // https://github.com/RobTillaart/CRC

#define MYCILA_UDP_MSG_TYPE_JSY_DATA 0x02 // supports all JSY models
#define MYCILA_UDP_PORT              53964

AsyncUDP udp;

float _Tension_M, _Intensite_M, _PowerFactor_M, _PuissanceS_M_inst, _PuissanceI_M_inst, _Energie_M_Soutiree, _Energie_M_Injectee, _Frequence;
int _PVAS_M_inst, _PVAI_M_inst;

void Setup_MycilaJsyApp() {

	udp.onPacket([](AsyncUDPPacket packet) {
    // buffer[0] == MYCILA_UDP_MSG_TYPE_JSY_DATA (1)
    // buffer[1] == size_t (4)
    // buffer[5] == MsgPack (?)
    // buffer[5 + size] == CRC32 (4)

    size_t len = packet.length();
    uint8_t* buffer = packet.data();

    if (len < 5 || buffer[0] != MYCILA_UDP_MSG_TYPE_JSY_DATA)
      return;

    uint32_t size;
    memcpy(&size, buffer + 1, 4);

    if (len != size + 9)
      return;

    // crc32
    FastCRC32 crc32;
    crc32.add(buffer, size + 5);
    uint32_t crc = crc32.calc();

    if (memcmp(&crc, buffer + size + 5, 4) != 0)
      return;
	 
    JsonDocument doc;
    deserializeMsgPack(doc, buffer + 5, size);

    // Données générale de la Maison
    _Tension_M = doc["channel2"]["voltage"];
    _Intensite_M = doc["channel2"]["current"];
    _Energie_M_Soutiree = doc["channel2"]["active_energy_imported"];
    _Energie_M_Injectee = doc["channel2"]["active_energy_returned"];
    _Frequence = doc["channel2"]["frequency"];
    float pf2 = doc["channel2"]["power_factor"];
    pf2 = abs(pf2);
    if (pf2 > 1) pf2 = 1;
    _PowerFactor_M = pf2;
    float pw2 = doc["channel2"]["active_power"];
    if (pw2 >= 0) {
      _PuissanceS_M_inst = pw2;
      _PuissanceI_M_inst = 0;
      if (pf2 > 0.01)
      {
        _PVAS_M_inst = PfloatMax(pw2 / pf2);
      }
      else
      {
        _PVAS_M_inst = 0;
      }
      _PVAI_M_inst = 0;
    }
    else
    {
      _PuissanceS_M_inst = 0;
      _PuissanceI_M_inst = -pw2;
      if (pf2 > 0.01)
      {
        _PVAI_M_inst = PfloatMax(-pw2 / pf2);
      }
      else
      {
        _PVAI_M_inst = 0;
      }
      _PVAS_M_inst = 0;
    }

    /* // canal secondaire
    if (doc.containsKey("channel1")) {
      // à voir sur demande 
    }*/
    
    filtre_puissance();
    PuissanceRecue = true; // Reset du Watchdog à chaque trame reçue
    EnergieActiveValide = true;
    Pva_valide = true;
    if (cptLEDyellow > 30) {
      cptLEDyellow = 4;
    }

    serializeJson(doc, Mycila_dataBrute);

  });

	 
	udp.listen(MYCILA_UDP_PORT);
}

void LectureMycila() {
  Tension_M = _Tension_M;
  Intensite_M = _Intensite_M;
  Energie_M_Soutiree = _Energie_M_Soutiree;
  Energie_M_Injectee = _Energie_M_Injectee;
  PowerFactor_M = _PowerFactor_M;
  PuissanceS_M_inst = _PuissanceS_M_inst;
  PuissanceI_M_inst = _PuissanceI_M_inst;
  PVAS_M_inst = _PVAS_M_inst;
  PVAI_M_inst = _PVAI_M_inst;
  Frequence = _Frequence;
}
