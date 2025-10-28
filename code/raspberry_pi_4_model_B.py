# MightyZap 12Lf-55F-27 (RS-485, IR protocol) – Arduino Mega
# HW: RS-485 transceiver, DE+RE spoj na pin 2, Serial1 (TX1=18, RX1=19), 57600 8N1

import sys, serial, time, RPi.GPIO as GPIO

# === HARDWARE ===
SERIAL_PORT = "/dev/ttyAMA0"  # UART0 (PL011)
BAUD = 57600
DIR_PIN = 17                  # GPIO17 -> RS-485 DE + /RE (connected)

# === PROTOCOL ===
ID = 0x00
CMD_STORE = 0xF3
ADR_FORCE_ONOFF = 0x80
ADR_GOAL_POS_L  = 0x86

def checksum(id, size, cmd, factors):
    s = id + size + cmd + sum(factors)
    return (0xFF - (s & 0xFF)) & 0xFF

def send_packet(ser, cmd, factors):
    size = 2 + len(factors)
    pkt = bytearray([0xFF, 0xFF, 0xFF, ID, size, cmd]) + bytearray(factors)
    pkt.append(checksum(ID, size, cmd, factors))
    GPIO.output(DIR_PIN, GPIO.HIGH)   # TX
    time.sleep(0.00005)
    ser.write(pkt)
    ser.flush()
    GPIO.output(DIR_PIN, GPIO.LOW)    # back to RX

def set_position(ser, position):
    send_packet(ser, CMD_STORE, [ADR_GOAL_POS_L, position & 0xFF, (position >> 8) & 0xFF])

# === MAIN ===
if len(sys.argv) < 2:
    print("Use: python3 servoTest.py [true|false]")
    sys.exit(1)

arg = sys.argv[1].lower()
if arg not in ["true", "false"]:
    print("Argument must be true or false")
    sys.exit(1)

move_open = arg == "true" # True = gripper open, False = gripper closed

# Constant positions
POS_OPEN = 3000
POS_CLOSE = 1000

GPIO.setmode(GPIO.BCM)
GPIO.setup(DIR_PIN, GPIO.OUT, initial=GPIO.LOW)

ser = serial.Serial(SERIAL_PORT, BAUD, timeout=0.05)
time.sleep(0.1)

# Turn on torque / force
send_packet(ser, CMD_STORE, [ADR_FORCE_ONOFF, 0x01])
time.sleep(0.05)

if move_open:
    print("Gripper opening...")
    set_position(ser, POS_OPEN)
else:
    print("Gripper closing...")
    set_position(ser, POS_CLOSE)

# Set position 
# goal = 3000
# send_packet(ser, CMD_STORE, [ADR_GOAL_POS_L, goal & 0xFF, (goal >> 8) & 0xFF])

ser.close()
GPIO.cleanup()