# Simple-FOTA

Quick run guide for the Simple-FOTA system (STM32 + ESP32 gateway + CoreIoT).

## Quick start

From project root:

1. Configure [esp32_gateway/src/global.cpp](esp32_gateway/src/global.cpp) (**required**).

	You must configure these variables before running:

	```cpp
	const char* WIFI_SSID = "<your_wifi_ssid>";
	const char* WIFI_PASSWORD = "<your_wifi_password>";
	const char* DEVICE_TOKEN = "<your_device_token>";
	```

	Without correct values here, the gateway cannot connect properly or perform OTA as expected.

2. Build STM32 app firmware (for OTA):

```bash
cd firmware
make clean && make
```

3. Build + flash STM32 bootloader:

```bash
cd ../bootloader
make clean && make
st-flash write ./build/bootloader.bin 0x08000000
```

4. Build + upload ESP32 gateway:

```bash
cd ../esp32_gateway
pio run
pio run -t upload
```

5. Monitor gateway logs:

```bash
pio device monitor
```

6. OTA on CoreIoT:
- (Optional) Import the prebuilt dashboard file [simple_fota_dashboard.json](simple_fota_dashboard.json).
- Upload firmware `.bin` to **OTA Firmware**.
- You can use prebuilt files [sample_firmware_1.bin](sample_firmware_1.bin) or [sample_firmware_2.bin](sample_firmware_2.bin), or your own build output.
- Assign firmware to target device.
- ESP32 downloads and flashes STM32 automatically.

## Note

⚠️ **Important:**
- Before ESP32 starts flashing firmware, make sure STM32 is reset and in the expected boot state.
- UART wiring must be correct: STM32 `PA9` (USART TX) -> ESP32 `RX2`, and STM32 `PA10` (USART RX) -> ESP32 `TX2`.
- I2C wiring for LCD1602 module must be correct: STM32 `PB7` (`I2C1_SDA`) -> LCD1602 I2C module `SDA`, and STM32 `PB6` (`I2C1_SCL`) -> LCD1602 I2C module `SCL`.

| STM32 pin | ESP32 pin |
| --- | --- |
| `PA9` (USART TX) | `RX2` |
| `PA10` (USART RX) | `TX2` |

| STM32 pin | LCD1602 I2C module pin |
| --- | --- |
| `PB7` (`I2C1_SDA`) | `SDA` |
| `PB6` (`I2C1_SCL`) | `SCL` |

