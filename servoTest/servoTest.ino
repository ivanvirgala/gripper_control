// MightyZap 12Lf-55F-27 (RS-485, IR protocol) – Arduino Mega
// HW: RS-485 transceiver, DE+RE spoj na pin 2, Serial1 (TX1=18, RX1=19), 57600 8N1

#include <Arduino.h>

static const uint8_t RS485_DIR_PIN = 2;   // HIGH=TX, LOW=RX
static const uint32_t BAUD = 57600;

// IR protocol
static const uint8_t CMD_LOAD_DATA  = 0xF2; // nepovinné (ak chceš čítať)
static const uint8_t CMD_STORE_DATA = 0xF3;

static const uint8_t ID = 0; // ponecháme ID=0 (z výroby)

// Adresy (Force-Control rada)
static const uint8_t ADR_FORCE_ONOFF        = 0x80; // 1 bajt (0=OFF, 1=ON)
static const uint8_t ADR_GOAL_POSITION_L    = 0x86; // 2 bajty (L,H)
static const uint8_t ADR_GOAL_POSITION_H    = 0x87;
static const uint8_t ADR_PRESENT_POSITION_L = 0x8C; // 2 bajty (L,H)
static const uint8_t ADR_PRESENT_POSITION_H = 0x8D;

inline void rs485Tx(bool tx) { digitalWrite(RS485_DIR_PIN, tx ? HIGH : LOW); delayMicroseconds(40); }

uint8_t irChecksum(uint8_t id, uint8_t size, uint8_t cmd, const uint8_t* f, size_t n) {
  uint16_t s = id + size + cmd;
  for (size_t i = 0; i < n; ++i) s += f[i];
  return 0xFF - (uint8_t)(s & 0xFF);
}

void irSendPacket(uint8_t id, uint8_t cmd, const uint8_t* f, size_t n) {
  const uint8_t hdr[3] = {0xFF, 0xFF, 0xFF};
  const uint8_t size   = (uint8_t)(2 + n); // COMMAND..CHECKSUM
  const uint8_t csum   = irChecksum(id, size, cmd, f, n);

  rs485Tx(true);
  Serial1.write(hdr, 3);
  Serial1.write(id);
  Serial1.write(size);
  Serial1.write(cmd);
  if (n) Serial1.write(f, n);
  Serial1.write(csum);
  Serial1.flush();
  rs485Tx(false);
}

// Zápis 1 bajtu na adresu
void write1(uint8_t id, uint8_t addr, uint8_t val) {
  uint8_t f[2] = { addr, val };
  irSendPacket(id, CMD_STORE_DATA, f, sizeof(f));
}

// Zápis 16-bit hodnoty (Little Endian) od "L" adresy
void write2(uint8_t id, uint8_t addrL, uint16_t val) {
  uint8_t f[3] = { addrL, (uint8_t)(val & 0xFF), (uint8_t)(val >> 8) };
  irSendPacket(id, CMD_STORE_DATA, f, sizeof(f));
}

// Nastavenie cieľovej polohy (0..4095; pre 27 mm používaj cca 0..~3686)
void setGoalPosition(uint16_t counts) {
  write2(ID, ADR_GOAL_POSITION_L, counts);
}

// (Voliteľné) zapni „force/torque“, ak by sa nehýbalo
void forceOn(bool on) {
  write1(ID, ADR_FORCE_ONOFF, on ? 0x01 : 0x00);
}

void setup() {
  pinMode(RS485_DIR_PIN, OUTPUT);
  rs485Tx(false);
  Serial1.begin(BAUD, SERIAL_8N1);

  // Ak aktuátor nereaguje, najprv zapni "force"
  forceOn(true);

}

void loop() {
  /*
  setGoalPosition(1000); // stred
  delay(4000);
  setGoalPosition(3500); // viac vysunuté (bezpečný rozsah pre 27 mm drž ~0..3686)
  delay(4000);
  */
}
