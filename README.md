Occupancy Meter ESP32 Camera Software
=====

Build:

```shell
$ git submodule init
$ git submodule update
$ make menuconfig
$ make flash monitor
```

Doe de volgende dingen in menuconfig:
- Zet PSRAM aan (navigeer naar submenu's "Component Config" --> "ESP32-specific". En dan heet de optie "Support for external, SPI-connected RAM")
- Voer je wifi access point SSID en wachtwoord in (Occupancy Meter Configuratie submenu)

-----

ESP-IDF
-----

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
