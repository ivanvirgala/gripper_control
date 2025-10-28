# Drive Control for MightyZap RS‑485 Gripper

Minimal example for controlling a **MightyZap 12Lf-55F-27** gripper via **RS-485** from **Arduino Mega** or **Raspberry Pi 4**.

---

## ⚙️ Files

---

## 🔌 Wiring

**Arduino Mega → RS-485 → MightyZap**  
TX1 → DI, RX1 → RO, D2 → DE/RE, 5V/GND → VCC/GND  

**Raspberry Pi → RS-485 → MightyZap**  
TXD0 (GPIO14) → DI, RXD0 (GPIO15) → RO, GPIO17 → DE/RE, 3V3/GND → VCC/GND  

Baud: `57600 8N1` · Device ID: `0x00` · Header: `0xFF 0xFF 0xFF`

---

## 🚀 Quick Start

**Raspberry Pi:**
```bash
sudo apt install python3-pip
pip3 install pyserial RPi.GPIO
python3 raspberry_pi_4_model_B.py true    # open
python3 raspberry_pi_4_model_B.py false   # close

[FF FF FF] [ID] [SIZE] [CMD] [PAYLOAD] [CHK]
CHK = 0xFF - ((ID + SIZE + CMD + sum(PAYLOAD)) & 0xFF)
