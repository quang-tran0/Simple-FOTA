# Simple-FOTA Manual

This repository contains a simple end-to-end FOTA (Firmware Over-The-Air) setup for STM32 + ESP32 gateway + CoreIoT dashboard.

## 1) Project Overview

- [bootloader](bootloader): STM32 bootloader project.
- [firmware](firmware): STM32 application firmware project.
- [esp32_gateway](esp32_gateway): ESP32 gateway that downloads OTA firmware and flashes STM32.
- [simple_fota_dashboard.json](simple_fota_dashboard.json): CoreIoT dashboard configuration.

## 2) Typical OTA Flow

1. Build STM32 application firmware from [firmware](firmware).
2. Export the generated firmware binary (`.bin`).
3. Upload that firmware file to CoreIoT as OTA firmware.
4. Assign the OTA firmware to the target CoreIoT device.
5. ESP32 gateway downloads the assigned firmware and flashes it to STM32.

## 3) Important Note (CoreIoT OTA Assignment)

Firmware built from [firmware](firmware) can be uploaded into **OTA Firmware** on the CoreIoT platform, then assigned to a device so it is flashed into STM32.

In short:

- Source firmware project: [firmware](firmware)
- Upload destination: CoreIoT **OTA Firmware**
- Deployment target: Assigned CoreIoT device
- Final action: Gateway flashes firmware to STM32

## 4) Build STM32 Projects

Both [bootloader](bootloader) and [firmware](firmware) include a `Makefile`.

From workspace root:

```bash
cd firmware
make
```

```bash
cd bootloader
make
```

The build outputs are generated in each project's `build/` directory.

## 5) ESP32 Gateway

The ESP32 gateway project is in [esp32_gateway](esp32_gateway) and uses PlatformIO.

Typical commands:

```bash
cd esp32_gateway
pio run
pio run -t upload
```

Optional serial monitor:

```bash
pio device monitor
```

## 6) Suggested End-to-End Bring-Up

1. Flash STM32 bootloader from [bootloader](bootloader).
2. Flash ESP32 gateway from [esp32_gateway](esp32_gateway).
3. Build STM32 app in [firmware](firmware) and upload its `.bin` to CoreIoT OTA Firmware.
4. Assign that OTA firmware to the device in CoreIoT.
5. Verify ESP32 logs for download + flash success.
6. Confirm STM32 boots into new application firmware.

