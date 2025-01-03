# Viatom ESP32 Companion

Firmware for ESP32 to act as a companion device for the Viatom wearable ring oximeter.  
The companion device reads the values approximately every 4 seconds and sends them to an [InfluxDB](https://www.influxdata.com/products/influxdb/).  
They can then be visualized e.g. with [Grafana](https://grafana.com/).  
Furthermore, the companion device can also set the time and trigger the vibration as an alarm.

The goal of this firmware is to provide an open-source solution for operating the ring oximeter and to not need the closed-source smartphone app anymore.  
Additionally, the functionality is extended in comparison to the smartphone apps (e.g. the alarm functionality).

## Hardware

The following hardware was used for development and is known to work:
- [PO2 device](https://www.viatomtech.com/po2)
- [ESP32-C3](https://www.espressif.com/en/products/socs/esp32-c3)

## Software

The following software was used for development:
- [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/)

## Acknowledgments

The [viatom-ble](https://github.com/ecostech/viatom-ble/) project was the trigger to start the development of this companion device.  
Some parts are also transferred from that project.

## Usage

1. Rename the file `lib/secrets/secrets.h.template` to `lib/secrets/secrets.h` and fill the variables.
The MAC address of the PO2 device can be determined through scanning.
The [viatom-ble](https://github.com/ecostech/viatom-ble/) project can also be used to determine the MAC address.

2. Upload the firmware with PlatformIO to your ESP32-C3.

## Reverse Engineering

The [Adafruit Bluefruit LE Sniffer](https://www.adafruit.com/product/2269) was used to sniff the BLE traffic between the Android App and the PO2 device.  
[Wireshark](https://www.wireshark.org/) was then used to analyze the traffic and the protocol.