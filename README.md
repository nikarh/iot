# What is this?

This repository contains firmware sources for my custom IOT devices, which are almost exclusively driven by various ESP32 microcontrollers.

The sources are either yaml configurations for [esphome](https://esphome.io/) or [platformio](https://platformio.org/) projects.


For all of the projects esphome is preferable, bacause:

- Almost no programming is necessary
- It checks constraints of microcontroller pins (some can only be used for input, some for output, some have pull-up and pull-down resistors and some don't, etc)
- It has a simple to use home assistant integration which allows some pretty sophisticated configurations
- Supports OTA updates natively