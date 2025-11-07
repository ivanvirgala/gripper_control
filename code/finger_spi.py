import spidev, time

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 200_000
spi.mode = 0b01

CFG = 0xC78B   # AIN0 vs GND
FS = 1.024

def read_once():
    spi.xfer2([(CFG>>8)&0xFF, CFG&0xFF, 0x00, 0x00])    # start conversion
    time.sleep(0.01)                                   
    rx = spi.xfer2([0x00,0x00,0x00,0x00])               # DATA + echo
    raw = (rx[0] << 8) | rx[1]
    if raw & 0x8000: raw -= 0x10000
    volts = raw * (FS / 32768.0)
    return rx, raw, volts

try:
    print("Measuring started... Ctrl+C exit")
    while True:
        rx, raw, v = read_once()
        print(f"RX:{rx}  RAW:{raw:6d}  V:{v:+.6f}")
        time.sleep(0.1)
finally:
    spi.close()
