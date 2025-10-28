# Drive Control for MightyZap RS‑485 Gripper

Modern, minimal reference implementation to control a MightyZap 12Lf‑55F‑27 actuator over RS‑485 from either Arduino Mega (.ino) or Raspberry Pi 4 Model B (.py).

✱ Purpose: provide a clean, reproducible baseline for gripper open/close motion and arbitrary position commands, with the packet format and checksum spelled out for quick porting.

📦 Repository Structure
.
├── arduino_mega.ino            # Arduino Mega sketch (RS‑485 on Serial1)
└── raspberry_pi_4_model_B.py    # Raspberry Pi control script (UART0 + GPIO17 for DE/RE)
🧰 Hardware & Interfaces

Actuator: MightyZap 12Lf‑55F‑27

Bus: RS‑485 half‑duplex

Baud: 57600 8N1

Packet header: 0xFF 0xFF 0xFF

Device ID: 0x00 (default)

Key registers/commands:

CMD_STORE = 0xF3

ADR_FORCE_ONOFF = 0x80

ADR_GOAL_POS_L = 0x86

The Raspberry Pi script uses UART0 (/dev/ttyAMA0) and GPIO17 to drive the RS‑485 DE/RE direction pin; positions POS_OPEN=3000, POS_CLOSE=1000 are provided as examples. The packet checksum is 0xFF - (sum & 0xFF). fileciteturn0file0

🔌 Wiring
Arduino Mega → RS‑485 Transceiver → MightyZap
Arduino Mega	RS‑485 Transceiver	Notes
TX1 (D18)	DI	Serial1 TX
RX1 (D19)	RO	Serial1 RX
D2	DE + /RE (tied)	Direction control (HIGH=TX, LOW=RX)
5V / GND	VCC / GND	Match transceiver voltage
A/B	A/B	Differential pair to actuator

The Arduino sketch targets Serial1 at 57600 8N1 and toggles DE/RE via D2. (See comments in the .ino file.)

Raspberry Pi 4 → RS‑485 Transceiver → MightyZap
Raspberry Pi 4	RS‑485 Transceiver	Notes
GPIO14 (TXD0)	DI	UART0 TX (/dev/ttyAMA0)
GPIO15 (RXD0)	RO	UART0 RX
GPIO17	DE + /RE (tied)	Direction control
3V3 / GND	VCC / GND	Use 3V3‑compatible transceiver
A/B	A/B	To actuator

Ensure you disable the Linux serial console and enable UART (raspi‑config → Interface Options → Serial). Use a 3V3 RS‑485 module when connecting to the Pi.

🛠️ Setup
Raspberry Pi (Python)

Enable UART on the Pi and wire as above.

Install deps:

sudo apt update && sudo apt install -y python3-pip
pip3 install pyserial RPi.GPIO

Verify the port exists:

ls -l /dev/ttyAMA0
Arduino Mega

Wire as above.

Open arduino_mega.ino in Arduino IDE.

Board: Arduino Mega 2560; Port: your Mega’s port.

Upload.

🚀 Quick Start
Raspberry Pi script

Run the helper to open or close the gripper:

python3 raspberry_pi_4_model_B.py true   # open
python3 raspberry_pi_4_model_B.py false  # close

Direction pin: GPIO17; serial: /dev/ttyAMA0 @ 57600. fileciteturn0file0

To change target positions, edit POS_OPEN / POS_CLOSE near the top of the script. fileciteturn0file0

Arduino sketch

The sketch mirrors the same packet format and direction‑control logic on Serial1 (TX1/RX1), with D2 driving DE/RE. Adjust constants in the sketch as needed.

🧪 Protocol Reference (TL;DR)

Frame:

[0xFF 0xFF 0xFF] [ID] [SIZE] [CMD] [PAYLOAD ...] [CHECKSUM]

SIZE = 2 + len(PAYLOAD)

CHECKSUM = 0xFF - ((ID + SIZE + CMD + sum(PAYLOAD)) & 0xFF)

Common ops:

Torque enable:

CMD=0xF3, PAYLOAD=[0x80, 0x01] → store FORCE_ONOFF = 1 (enable). fileciteturn0file0

Set goal position (little‑endian):

CMD=0xF3, PAYLOAD=[0x86, POS_L, POS_H] with POS = POS_L + (POS_H<<8). fileciteturn0file0

Direction control:

Drive DE/RE HIGH during TX; return LOW immediately after ser.flush() to listen. (GPIO17 on Pi; D2 on Mega). fileciteturn0file0

🧷 Design Notes

Minimal round‑trip timings (≈50–100 µs) are used around direction switching to avoid bus contention. fileciteturn0file0

Python script keeps a short serial timeout = 50 ms to prevent blocking. fileciteturn0file0

All constants are grouped at the top of each program for easy porting.

🧯 Safety

Power the actuator per vendor specs; RS‑485 A/B polarity matters.

Ensure a shared ground between controller, transceiver, and actuator.

When testing, start with a low torque and safe positions; be prepared to cut power.

🔍 Troubleshooting
Symptom	Likely Cause	Fix
No motion	Wrong UART device or console still enabled	Use /dev/ttyAMA0; disable serial console
Garbled/ignored	Baud or parity mismatch	Confirm 57600 8N1
One‑way comms	DE/RE not toggled	Tie DE and /RE; drive the pin HIGH for TX, LOW for RX
Jittery moves	Bus contention	Add small delays before/after TX; ensure single master
📚 Extending

Add read‑back of status registers and position.

Wrap Python into a tiny CLI: grip open|close|pos <N>.

Publish as a Python package / Arduino library for reuse.

🧾 License

MIT (see LICENSE).

🙌 Acknowledgements

Based on the in‑repo Arduino/Python sources and comments.

If you use this in a paper or project, please cite the repository and the actuator vendor docs.
