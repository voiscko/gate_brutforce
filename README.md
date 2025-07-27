# Gate Brute - Flipper Zero Security Testing Tools

A collection of brute-force applications for the Flipper Zero device, designed for security testing and research purposes.

## ⚠️ DISCLAIMER

**FOR EDUCATIONAL AND AUTHORIZED TESTING PURPOSES ONLY**

This software is provided for educational purposes and authorized security testing only. Users are solely responsible for ensuring they have proper authorization before using these tools. The authors and contributors are not responsible for any misuse, damage, or illegal activities performed with this software.

**USE AT YOUR OWN RISK AND RESPONSIBILITY**

## Features

### NFC Brute Force (`nfc/`)
- Brute-force NFC/MIFARE Classic keys
- Systematic key enumeration
- Progress tracking
- Automatic key saving upon success

### RFID Brute Force (`rfid/`)
- RFID EM4100 protocol brute-forcing
- Sequential ID enumeration
- Real-time emulation

### Sub-GHz (`sub_ghz/`)
- Sub-GHz frequency brute-force capabilities (placeholder)

## Installation

1. Clone this repository
2. Build using the Flipper Zero build system (ufbt)
3. Install the `.fap` files to your Flipper Zero

### Building with ufbt

```bash
# For NFC module
cd nfc/
ufbt

# For RFID module  
cd rfid/
ufbt
```

## Usage

### NFC Brute Force
1. Launch the NFC brute force app on your Flipper Zero
2. Place the target NFC card near the device
3. Press OK to start brute-forcing
4. The app will systematically try different keys
5. Successful keys are automatically saved

### RFID Brute Force
1. Launch the RFID brute force app
2. The app will start emulating sequential RFID IDs
3. Press OK to stop

## Legal Notice

This tool is intended for:
- Security research
- Authorized penetration testing
- Educational purposes
- Testing your own devices

**You must have explicit permission to test any device or system that you do not own.**

## Contributing

Contributions are welcome! Please ensure all contributions maintain the educational and research focus of this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

This is a community project. For issues and questions, please use the GitHub issue tracker.

---

**Remember: With great power comes great responsibility. Use these tools ethically and legally.**