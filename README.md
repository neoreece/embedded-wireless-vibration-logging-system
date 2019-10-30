# Embedded Wireless Vibration Logging System

All code, documentation, and diagrams produced by Reece Younger

EE437: Intro to Embedded Systems
Problem: Design a multi-channel wireless vibration logging system.

* Release 1.0 - Verified by Instructor - 2019 10 30
  * Arduino Nano establishes wireless (bluetooth) connection with Arduino Uno
  * Arduino Nano reads and transmits accelerometer data via bluetooth
  * Arudino Uno recieves data from Nano
  * Arduino Uno formats data to plot to serial monitor
  * Arduino Uno writes data to SD card

## Requirements

1. Communication between microcontrollers must be wireless.
2. A device must be solely responsible for collecting data
   1. This device must not store data locally
3. A device must be solely responsible for recieving, proccessing, and saving
   data.

### Design

Data recorded by the Arduino Nano and accelerometer must be saved to
external storage.

This design decision increasing cohesion within modules by limiting its
responsibility to either collecting and transmitting data (in the NanoDriver)
or recieving, formatting, and storing data (in the UnoDriver).

Coupling between devices is reduced by limiting module-module communication to
a single packet of data. In this case, the X, Y, and Z accelerometer data
transmitted as a single packet.

By separating these responsibilities, data collection and data proccesing and
storage, the system efficiency is increased depending on the number of
collection devices. If each collection device was required to also process and
store data, hardware with greater resources would be required for every
collection device.

Instead, the processing capabilities and, therefore, cost of the collection
devices, may be restricted. A single device handling this responsibility
operates with one-to-many relationship with respect to the collection devices.

#### NanoDriver

* Responsibilites
  * Configure accelerometer on startup
  * Read accelerometer X, Y, and Z registers
  * Transmit accelerometer data to central storage

#### UnoDriver

* Responsibilites
  * Configure SD card on startup
  * Recieve accelerometer data from collection devices
  * Format collected data for output
  * Write collected data to SD card
