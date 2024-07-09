# i(L)ot - IoT Algorithms and Services Sapienza 23/24

The following repository contains the final group project for the 23/34 edition
of the IoT Algorithms and Services course held in Sapienza University of Rome.

The repository contains code for a smart IoT parking system that relies on embedded devices to collect data on parking spots availability.
The code is separated between the core esp32 component (iLot), that can provide information about the occupancy of two different parking spots via two connected piezoelectric devices;
a gateway component that relies on a Cubecell IoT device to act as a "LoRa antenna" for a beefier MQTT gateway device,
and a containerized server component that receives MQTT data to be processed in a easily accessible and persistent format.

## Architecture
![architecture](./architecture.png)
The system relies on a tiered architecture, with different protocols employed at each level. From the bottom up:
- The lowermost layer of the architecture is the ESP32 - Detector component. This communicates with the upper level via the LoRa protocol, chosen since the working of the ESP32 component would require constant, sleep-heavy duty cycling; LoRa allows the devices to communicate without having to setup and negotiate a connection every wakeup, lowering energy costs significantly. Attention was given to avoiding MitM attacks at this level. Each device at this level accounts for 2 parking spots in a lot, sending a message every time it's woken up by a detection in either of the spots.
- The middle layer acts as a bridge between the server and the devices, as communicating over LoRa directly to the server wouldn't work. The devices in this layer consist of regular machines, connected to the internet, with a prototyping board (we used a cubecell, but any LoRa and serial capable device can work) connected to act as a LoRa antenna. The system receives the messages and resends them via MQTT to a broker present on the next layer. MQTT was chosen to allow, if needed, to send messages back to the lower layers of the system, but this capability went unused. Every device on this layer accounts for a multitude of parking spots, intended for potentially handling the messages of the entire parking lot building.
- The uppermost layer is a dockerized system composed by an MQTT broker and a webserver. The broker sends data to the webserver, that subscribes to the required MQTT topic. Every received message updates the webserver state, which can be accessed via a web interface. Every element on this layer is meant to take care of 1..n parking lots.
## How to setup
Clone the repo, then:
### ESP32
The ESP component requires the Espressif SDK. For the automated wakeup to work correctly,
two GPIO pins must be selected (defaults to GPIO1 and GPIO2) as wakeup sources, either via menuconfig or by editing sdkconfig directly,
and the piezoelectric devices analog output connected to these same pins. To get the code running on the device:
```
idf.py set-target <esp32 platform> && idf.py -p /dev/ttyUSB<port_num> flash monitor
```
### Gateway
Flash the .ino file to the Cubecell device; Connect the device via USB, and, after adding the MQTT broker password and username pair, run the provided .py file.

### Server
The "Server" component relies on Docker Compose to quickly setup an MQTT "listener" Mosquitto container and a companion Python Flask webserver that
presents the MQTT data in a more convenient format. To first start the containers, simply run:
```
docker-compose up --build
```
while the Docker daemon is running. Depending on the configuration of the Docker instance, administrator priviledges might be required.
The Mosquitto container requires some additional setup, as it needs a non-default password and username pair. To add one, get the Mosquitto container id via
```
docker ps
```
then access the container via sh:
```
docker exec <mosquitto-container-id> -it sh
```
and add a new password with the following command:
```
mosquitto_passwd /mosquitto/config/passwd <username>
```
which will prompt for the new password to be added. Configure these credentials on the Flask server and restart the containers to complete the configuration process.

## Objectives and Rationale
### ESP32
#### Components overview
#### LoRa and Encryption
#### Wakeup system
### Gateway
### Server

## Energy consumption

## Required Devices

## Members
- [Emanuele Roncioni](https://www.linkedin.com/in/emanuele-roncioni-4b516a303/)
- [Francesco Saverio Sconocchia Pisoni](https://it.linkedin.com/in/francesco-saverio-sconocchia-pisoni-0a0050303)

Click [here](https://docs.google.com/presentation/d/1uYoUDjAFOGUmK91i_s4SOkWCZ_YNvJvgRWBcW6wO5Q0/edit#slide=id.p) for our short presentation.
