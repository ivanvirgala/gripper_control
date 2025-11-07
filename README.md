# Drive Control – MightyZap RS-485 Gripper

Example for controlling a **MightyZap 12Lf-55F-27** servo via **RS-485** from **Arduino Mega** or **Raspberry Pi 4**.

⚙️ **Files**  
```text

gripper_control/
├── code/
│ ├── arduino_mega.ino # Arduino Mega sketch (RS-485 via Serial1)
│ ├── finger_spi.py    # Raspberry Pi control script (SPI)
│ └── raspberry_pi_4_model_B.py # Raspberry Pi control script (UART0 + GPIO17)
├── LICENSE
└── README.md
```

🔌 **Wiring**  
**Arduino Mega → RS-485 → MightyZap**  
TX1 → DI, RX1 → RO, D2 → DE/RE, 5V/GND → VCC/GND  

**Raspberry Pi → RS-485 → MightyZap**  
TXD0 (GPIO14) → DI, RXD0 (GPIO15) → RO, GPIO17 → DE/RE, 3V3/GND → VCC/GND  

Baud: `57600 8N1` · Device ID: `0x00` · Header: `0xFF 0xFF 0xFF`

```text
┌────────────────┐       ┌──────────────────┐       ┌────────────────────┐
│  Arduino Mega  │──────▶│ RS-485 Module    │────▶  │ MightyZap Gripper  │
│ (TX1/RX1, D2)  │◀──────│ (DI/RO, DE/RE)   │◀────  │ (A/B Differential) │
└────────────────┘       └──────────────────┘       └────────────────────┘

┌────────────────┐       ┌──────────────────┐       ┌────────────────────┐
│ Raspberry Pi 4 │──────▶│ RS-485 Module    │────▶  │ MightyZap Gripper  │
│ (GPIO14/15/17) │◀──────│ (DI/RO, DE/RE)   │◀────  │ (A/B Differential) │
└────────────────┘       └──────────────────┘       └────────────────────┘
```
🔌 **Wiring** 
**SPI ADC → Raspberry Pi 4**

| ADC Pin | Raspberry Pi 4 Pin | Function |
|----------|--------------------|-----------|
| VIN (3V3) | 1 / 17 | Power 3.3 V |
| GND | 6 | Ground (shared) |
| SCK | 23 (GPIO 11) | SPI Clock |
| DIN | 19 (GPIO 10) | MOSI |
| DOUT | 21 (GPIO 9) | MISO |
| CS | 24 (GPIO 8 / CE0) | Chip Select |

Both analog and logic VIN must be 3.3 V. All grounds must be common with the Raspberry Pi.

🚀 **Quick Start**  
**Raspberry Pi:**  
```bash
sudo apt install python3-pip
pip3 install pyserial RPi.GPIO
python3 raspberry_pi_4_model_B.py true   # open
python3 raspberry_pi_4_model_B.py false  # close
```

Arduino Mega: Upload arduino_mega.ino (Board: Mega 2560, Port: Serial1)

📡 Packet Format
[FF FF FF] [ID] [SIZE] [CMD] [PAYLOAD] [CHK]
CHK = 0xFF - ((ID + SIZE + CMD + sum(PAYLOAD)) & 0xFF)

Torque ON: CMD = 0xF3, [0x80, 0x01]
Goal pos: CMD = 0xF3, [0x86, POS_L, POS_H]

🧩 Notes
DE/RE HIGH = TX, LOW = RX (GPIO17 / D2)
Keep short delays to avoid bus contention.
Ensure shared ground and correct RS-485 polarity.
Use a 3V3-compatible transceiver with Raspberry Pi.
Disable the serial console before using /dev/ttyAMA0.

🧾 License

MIT License
