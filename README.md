# SerialPortGateway
The SerialPortGateway functions as a gateway to serial devices, which can each be adressed by a unique device ID.

Features:
* Auto discovery of new serial devices
* Processing of incoming messages from the devices
    * Easy to use message format for communicating with serial devices
* Sending messages/commands to specific devices or all devices at once
* Automatic handling of IO Errors
* Logging of all actions
* Designed as a base class for other applications to be build upon
    * Callbacks for easily handling different events (New messages, devices added/deleted )
    * Easy to use interface for interfering with the gateway
* Parallelization of all time critical or potentially blocking tasks
* Quick configuration of the gateway via easy config files
* Can be used either dockerized or standalone

# Table of Contens
1. [Files and Folder structure](#files-and-folder-structure)
2. [Dependencies](#dependencies)
3. [Installation](#installation)
4. [Including and compiling SerialPortGateway in a project](#including-and-compiling-serialportgateway-in-a-project)
5. [Code Documentation](#code-documentation)
6. [Requirements for communicating between the SerialPortGateway and serial devices](#requirements-for-communicating-between-the-serialportgateway-and-serial-devices)
7. [Usage](#usage)
    1. [Configuration files](#configuration-files)
        1. [Main Config file](#main-config-file)
        2. [Hardware ID Whitelist](#hardware-id-whitelist)
        3. [Serial Port Blacklist](#serial-port-blacklist)
    2. [Starting the application in a Docker container](#starting-the-application-in-a-docker-container)
    3. [Basic usage](#basic-usage)
    4. [Inherit and extend SerialPortGateway](#inherit-and-extend-serialportgateway)
8. [To Do list](#to-do-list)
9. [Notes](#notes)
10. [License](#license)

# Files and Folder structure
* `config` contains the configuration files
    * Sample Config file
    * Sample hardware ID whitelist
    * Sample serial port blacklist
* `dependencies` is the place where all dependencies get downloaded to (See [Installation](#Installation) for further details)
* `src` contains the source code
    * `SerialDevice` class
    * `SerialMessage` class
    * `SerialPortGateway` class
    * `serial2console-gateway` application (Demo & debugging tool)
* `.env` is an environment file for Docker
* `.gitmodules` contains references to the dependencies
* `build.sh` is a script for building the application
* `docker-compose.yml` is an exemplary docker-compose for easy deployment
* `Dockerfile` is an exemplary Dockerfile which can be used for building an image with all needed dependencies
* `Makefile` is an exemplary Makefile for compiling the application

# Dependencies
If you follow the [Installation](#Installation) instructions, some dependencies will be cloned directly into the dependencies folder:
* [Exception](https://github.com/je-s/Exception)
* [Logger](https://github.com/je-s/Logger)
* [Config](https://github.com/je-s/Config)

In case you're going to use the Docker image, you're good and don't require anything other than Docker in order to deploy/compile/test the SerialPortGateway or any enhancements done by yourself.

In case you want to use the SerialPortGateway without Docker, you need the following few things installed:
* gcc, g++, libc-dev, make, curl, wget, git.
    * g++ must be version 5 or higher.
    * git must be version 2 or higher.
* [wjwwood's serial library](https://github.com/wjwwood/serial). This also requires:
    * cmake, python, python-pip, catkin
If there's anything not working, try to sync your configuration with that from the Dockerfile (Or drop me an issue if nothing helps).

# Installation
1. Clone the repo
2. Change the directory into the repo
3. Execute `git submodule update --init --recursive --remote` to download all dependencies needed for using/compiling/executing the gateway
4. Building:
    1. Build the Docker image: `make buildDockerImage` or `docker-compose build`
    2. Build the application without Docker: `./build.sh`

# Including and compiling SerialPortGateway in a project
C++14 is required for compilation.

In order to use SerialPortGateway, `<path>/SerialPortGateway/src/SerialPortGateway.hpp` needs to be included.\
In order to compile SerialPortGateway, following files need to be compiled and linked:
* `<path>/SerialPortGateway/dependencies/Logger/src/Logger.cpp`
* `<path>/SerialPortGateway/dependencies/Logger/src/LogType.cpp`
* `<path>/SerialPortGateway/dependencies/Exception/src/Exception.cpp`
* `<path>/SerialPortGateway/dependencies/Config/src/ConfigExceptions.cpp`
* `<path>/SerialPortGateway/dependencies/Config/src/Config.cpp`
* `<path>/SerialPortGateway/src/SerialDevice.cpp`
* `<path>/SerialPortGateway/src/SerialMessage.cpp`
* `<path>/SerialPortGateway/src/SerialPortGateway.cpp`

(Take a look at the Makefile.)

# Code Documentation
Besides this document, you can find detailed documentation in the headers in [src](src/). The sources are also documented where necessary/useful.\
Have a look at the [serial2console-gateway](src/serial2console-gateway.cpp) reference application and the [Serial2MqttGateway](https://github.com/je-s/Serial2MqttGateway) for some pragmatic examples.

# Requirements for communicating between the SerialPortGateway and serial devices
* Messages from devices need to follow the [SerialMessageFormat](https://github.com/je-s/SerialMessageFormat).
* The command declared via the config key `COMMAND_GETID` / via the function `setCommandToGetDeviceId` needs to be understood by every device connected and result in a validly formatted message from the device to the gateway directly after. This message needs to correspond to the message type declared with the config key `MESSAGE_TYPE_ID` / via the function `setMessageTypeForIds` and needs to contain the device's ID.
* The message delimiter declared with the config key `MESSAGE_DELIMITER` / via the function `setMessageDelimiter` needs to be in sync with the message delimiter the connected devices are using.
* The baud rate declared with the config key `BAUD_RATE` / via the function `setBaudRate` needs also to be used by every device connected for sending and receiving data.

# Usage
## Configuration files
The SerialPortGateway can be easily configured with three different config files.

### Main Config file
This is the basic config of the gateway. It uses the [Config](https://github.com/je-s/Config) class.
The internal instance can be accessed with `Config * SerialPortGateway::getConfigInstance()`, in case you want to make use of it urself (as in the [Serial2MqttGateway](https://github.com/je-s/Serial2MqttGateway)).

The config file needs to have following keys defined:

| Key | Purpose | Value Description | Default |
| --- | ------- | ----------------- | ------- |
| LOGGING_ACTIVE | Whether logging to a file is active or not | 0 or 1 | 0 |
| SCAN_INTERVAL | Interval in which should be scanned for new devices | Time between scans in ms<br>0 means no automatic scans | 0 |
| WAIT_BEFORE_COMMUNICATION | Waiting time before an inital communication attempt to a device | Waiting time in ms<br> Must be > 0 | 500 |
| BAUD_RATE | Baud rate to be used for communication from and to the serial device | Number > 0 | 9600 |
| MESSAGE_DELIMITER | Message delimiter to be used for interpreting messages from serial devices | String | : |
| COMMAND_GETID | Command which is used to request/retrieve the device ID for each of the connected devices | String | getid<br>As used with the [ArduinoStreamCommander](https://github.com/je-s/ArduinoStreamCommander) |
| MESSAGE_TYPE_ID | Message type for messages which are intended to contain a device ID | String | id<br>As used with the [ArduinoStreamCommander](https://github.com/je-s/ArduinoStreamCommander) |

### Hardware ID Whitelist
The hardware ID whitelist lists all allowed hardware IDs;
* **If the list is empty, no whitelist-checks will be performed.**
* If the list has one or more entries, only devices matching one of those hardware IDs can/will be added to the gateway.

The hardware IDs are formatted in the following way: `<VendorID>:<ProductID>`\
The hardware IDs are separated from each other by a line break. A hardware ID whitelist can look like the following:
```
1a86:7523
0403:6001
2341:0042
```
These IDs can be retrieved with Linux tools such as "lsusb" or "udevadm".

### Serial Port Blacklist
The serial port blacklist lists all forbidden serial port paths that should be ignored when scanning for or adding new devices. The port paths are separated from each other by a line break. A port blacklist can look like the following:
```
/dev/ttyS0
/dev/ttyS1
/dev/ttyS2
/dev/ttyS3
```

## Starting the application in a Docker container
* The container needs to run in priviledged mode in order to gain access to the serial ports.
* In case you want to use the container just as your development environment, do one of the following things:
    * `docker-compose up -d devenv; docker attach serialportgateway_daemon_<number>;`
    * `docker run --privileged -it serialportgateway /bin/bash`
* For starting the application in a Docker container, simply use the `docker-compose.yml`-File included;\
Run `docker-compose up -d daemon` if you want to have the `serial2console-gateway` run in the background.\
You can interfere with the application by attaching to the container. i.e.: `docker attach serialportgateway_daemon_1`.
* In case you didn't build the image already, docker-compose will do this automatically for you.

Take a look at the [Notes](#Notes) for some restrictions when using Docker as container runtime in conjunction with serial ports.

## Basic usage
1. Instantiate a new `SerialPortGateway`-Object:
```
    SerialPortGateway * gateway = nullptr;
    gateway = new SerialPortGateway( "<configFile>", "<hardwareWhitelistFile>", "<serialPortBlacklistFile>", "<logPath>" );
```
2. Start the gateway: `gateway->start();`
3. Interfere with the gateway:
    * `gateway->sendMessageToSerialDevice( "SerialKiller", "Kill 'Em All" );`
    * `gateway->broadcastMessageToSerialDevices( "selfdestruct" );`
    * ...
4. Stop the gateway:
```
    gateway->stop();

    // This loop waits for the all read loops being gracefully quit, before exiting the application
    while( !gateway->isEveryReadLoopQuitted() )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }
```

## Inherit and extend SerialPortGateway
If you need an example on how to enhance SerialPortGateway's functionality, you can have a look at the [Serial2MqttGateway](https://github.com/je-s/Serial2MqttGateway).

Very basically you can to do this:
1. Inherit: `class EnhancedGateway : public SerialPortGateway`
2. Enhance with your functionality:
    * If you implement your own constructor, make sure to also call the base constructor: `EnhancedGateway::EnhancedGateway( ... ) : SerialPortGateway( ... ) { ... }`
    * Optionally, overwrite the `start` and `stop` functions
    * (Re-)Implement the callbacks: `serialDeviceAddedCallback`, `serialDeviceDeletedCallback`, `messageCallback`
3. Done.

# To Do list
1. Replace the currently used serial library, because of some design problems and functionalities/implementations which I don't like
2. Use some kind of serial utilities library to make things like retrieving the hardware ID of serial devices more easy & consistent.
3. Probably take over writing the libraries mentioned in 1. and 2. myself. :D

# Notes
* In case you're using Arduinos with the SerialPortGateway, you can use the [ArduinoStreamCommander](https://github.com/je-s/ArduinoStreamCommander).\
But this doesn't mean you have to, as long as you follow the [requirements for communication](#requirements-for-communicating-between-the-serialportgateway-and-serial-devices).
* Docker can -without doing anything else- only provide devices or ports to the container which are already connected when the container is started. In this case this means that all serial devices to be connected must be initially available at least at the start of the container, so that the port numbers are registered in the container. Dis- and reconnects that happen afterwards -and thus the exchange of devices to other ports- are unproblematic, but to register additional devices a reboot is required.\
Meaning: If more ports should be used than when the container was initially started, the container must be restarted.

# License
```
   Copyright 2019 Jan-Eric Schober

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
```
