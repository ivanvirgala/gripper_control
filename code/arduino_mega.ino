// MightyZap 12Lf-55F-27 (RS-485, IR protocol) – Arduino Mega
// HW: RS-485 transceiver, DE+RE con. on pin 2, Serial1 (TX1=18, RX1=19), 57600 8N1

#include <Arduino.h>

static const uint8_t RS485_DIR_PIN = 2;   // HIGH=TX, LOW=RX
static const uint32_t BAUD = 57600;

// IR protocol
static const uint8_t CMD_LOAD_DATA  = 0xF2; // for reading
static const uint8_t CMD_STORE_DATA = 0xF3;

static const uint8_t ID = 0; 

// Addresses (Force-Control)
static const uint8_t ADR_FORCE_ONOFF        = 0x80; // 1 byte  (0=OFF, 1=ON)
static const uint8_t ADR_GOAL_POSITION_L    = 0x86; // 2 bytes (L,H)
static const uint8_t ADR_GOAL_POSITION_H    = 0x87;
static const uint8_t ADR_PRESENT_POSITION_L = 0x8C; // 2 bytes (L,H)
static const uint8_t ADR_PRESENT_POSITION_H = 0x8D;

inline void rs485Tx(bool tx){ 
  digitalWrite(RS485_DIR_PIN, tx ? HIGH : LOW); delayMicroseconds(40); 
}

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

// Writing 1 byte to address
void write1(uint8_t id, uint8_t addr, uint8_t val) {
  uint8_t f[2] = { addr, val };
  irSendPacket(id, CMD_STORE_DATA, f, sizeof(f));
}

// Write 16-bit value (Little Endian) from "L" address
void write2(uint8_t id, uint8_t addrL, uint16_t val) {
  uint8_t f[3] = { addrL, (uint8_t)(val & 0xFF), (uint8_t)(val >> 8) };
  irSendPacket(id, CMD_STORE_DATA, f, sizeof(f));
}

// Target position setting (0..4095; for 27 mm use approx. 0..~3686)
void setGoalPosition(uint16_t counts) {
  write2(ID, ADR_GOAL_POSITION_L, counts);
}

// (Optional) turn on "force/torque" if it doesn't move
void forceOn(bool on) {
  write1(ID, ADR_FORCE_ONOFF, on ? 0x01 : 0x00);
}

void setup() {
  pinMode(RS485_DIR_PIN, OUTPUT);
  rs485Tx(false);
  Serial1.begin(BAUD, SERIAL_8N1);

  // If the actuator does not respond, first turn on "force"
  forceOn(true);
}

void loop() {
  setGoalPosition(1000); // Position 1.
  delay(4000);
  setGoalPosition(3500); // Position 2.
  delay(4000);
}
