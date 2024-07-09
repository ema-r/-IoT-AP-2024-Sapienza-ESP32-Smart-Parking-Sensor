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
The "heart" of the application, for the ESP32 Detector system the main concern was providing reliable detection of parked cars while minimizing energy consumption. Ideally, we wanted to find a solution we could use a wakeup source and, ideally, as a source of energy harvesting. While the energy harvesting proved to be not worth the effort compared to buying already made solar harvesters and similiar solutions, we did obtain satisfactory results for wakeup sources.
#### Detection
Three different detection systems were considered: a soundwave distance detector, a solenoid element, similiar to the ones that control traffic lights, and piezolectric devices.
- The soundwave distance detector worked out of the box with no modifications needed for our use case, with a maximum range of 4 meters and reliability as long as it was pointed towards a flat surface. The idea ended up being abandoned early as, other than being slightly uninteresting, it would prove to be somewhat unreliable with a car underneath, vulnerable to tampering, and, critically, the fact that unlike the chosen solution, it required to be fed power to work.
  
- The solenoid element was the second route we considered. By placing a solenoid under a moving car, we could've (ideally) obtained an induced current on the solenoid, which we could then use as a wakeup source for the device. This idea was discarded during testing for multiple reasons: it required a suitable solenoid to be built, as no ready units exist, which proved to be a challenge, both due to the amount of required wire and general difficulties in the construction; it required the "activator" car to move over the solenoid at a decent speed, which is hardly ideal in a parking solution; and finally, real traffic units work by being fed AC current at a certain frequency. They then sense cars (or bycicles, sometimes) by noticing shifts in the frequency caused by induction from the moving metal over them. We couldn't easily feed it constant AC current, and, together with other solutions proving to be more appealing, it was scrapped.
  
- The piezoelectric element was the chosen solution. The small sensor chosen is a ready-made device that utilizes a piezoelectric element paired to a protection board that reduces to voltage to a safe range. It provides analog and tuneable digital outputs, the latter requiring external power to work. Once the piezoelectric element is struck by any kind of force that's capable of inducing a vibration, AC current is produced on the analog output, with no external power source required. We connect the AO to the ESP32 device via GPIO pins, which we then setup as EXT1, wake-up-when-any-high GPIO pins. We can pinpoint which one of multiple elements woke up the device, allowing us to connect multiple lot sensors to a single ESP32 for cost effectiveness. To prove the concept, we ended up pairing two sensors to the ESP32.

A wrong sensor, a pressure sensitive resistence, was also bought, but ended up unused due to requiring, again, extra power.
#### Energy consumption
#### Components overview
#### LoRa and Encryption
#### Wakeup system
### Gateway
### Server

## Required Devices

## Members
- [Emanuele Roncioni](https://www.linkedin.com/in/emanuele-roncioni-4b516a303/)
- [Francesco Saverio Sconocchia Pisoni](https://it.linkedin.com/in/francesco-saverio-sconocchia-pisoni-0a0050303)

Click [here](https://docs.google.com/presentation/d/1uYoUDjAFOGUmK91i_s4SOkWCZ_YNvJvgRWBcW6wO5Q0/edit#slide=id.p) for our short presentation.
