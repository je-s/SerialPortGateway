/*
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
*/

// C Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <thread>

// SerialPortGateway Class
#include "SerialPortGateway.hpp"

/**
 * serial2console-gateway application
 * File: serial2console-gateway.cpp
 * Purpose: Defines a CLI for interacting with a SerialPortGateway
 *
 * @author Jan-Eric Schober
 * @version 1.0 20.01.2019
*/

// Constants
const std::string COMMAND_USAGE = "u";
const std::string COMMAND_LISTDEVICES = "ld";
const std::string COMMAND_LISTSERIALPORTS = "lp";
const std::string COMMAND_LISTMAPPINGS = "lm";
const std::string COMMAND_SEND = "s";
const std::string COMMAND_BROADCAST = "b";
const std::string COMMAND_ADDDEVICE = "a";
const std::string COMMAND_ADDNEWDEVICES = "an";
const std::string COMMAND_DELETEDEVICE = "d";
const std::string COMMAND_DELETEALLDEVICES = "da";
const std::string COMMAND_QUIT = "q";

// Variables
SerialPortGateway * gateway = nullptr;
bool gatewayStarted = true;

/**
 * Prints the usage of the CLI.
*/
void printUsage()
{
    std::cout
        << "Usage:" << std::endl
        << "\t" << COMMAND_LISTDEVICES << ": List all registered devices." << std::endl
        << "\t" << COMMAND_LISTSERIALPORTS << ": List all of the system's serial ports." << std::endl
        << "\t" << COMMAND_LISTMAPPINGS << ": Lists all mappings from deviceIds to serial ports." << std::endl
        << "\t" << COMMAND_SEND << ": Send a message to a single device." << std::endl 
        << "\t" << COMMAND_BROADCAST << ": Broadcast a message to all registered devices." << std::endl
        << "\t" << COMMAND_ADDDEVICE << ": Adds a device." << std::endl
        << "\t" << COMMAND_ADDNEWDEVICES << ": Adds all new devices." << std::endl
        << "\t" << COMMAND_DELETEDEVICE << ": Deletes a device." << std::endl
        << "\t" << COMMAND_DELETEALLDEVICES << ": Deletes all devices." << std::endl
        << "\t" << COMMAND_QUIT << ": Quit the gateway." << std::endl;
}

/**
 * Lists all registered/connected devices of the SerialPortGateway by their device ID.
*/
void listDevices()
{
    std::cout << gateway->getDeviceIdList() << std::endl;
}

/**
 * Lists all of the system's available serial ports which can be connected (and possibly are already connected).
*/
void listSerialPorts()
{
    std::cout << gateway->getSerialPortList() << std::endl;
}

/**
 * Lists all mappings between connected devices by their ID & their corresponding serial port.
*/
void listDeviceIdToSerialPortMappings()
{
    std::cout << gateway->getDeviceIdToSerialPortMappingList() << std::endl;
}

/**
 * Sends a message to a specific device.
*/
void sendMessage()
{
    std::string deviceId, message;
    std::cout << "-> Enter Device ID: " << std::endl;
    std::getline( std::cin, deviceId );
    std::cout << "-> Enter Message: " << std::endl;
    std::getline( std::cin, message );

    gateway->sendMessageToSerialDevice( deviceId, message );
}

/**
 * Sends a message to all devices connected.
*/
void broadcastMessage()
{
    std::string message;
    std::cout << "-> Enter Message: " << std::endl;
    std::getline( std::cin, message );

    gateway->broadcastMessageToSerialDevices( message );
}

/**
 * Tries to add a device on a specific serial port.
*/
void addDevice()
{
    std::string serialPort;
    std::cout << "-> Enter serial port: " << std::endl;
    std::getline( std::cin, serialPort);

    gateway->addSerialDevice( serialPort );
}

/**
 * Tries to add all new devices which are not currently registered.
 * This includes devices disconnected (either by user or because of and error) and reconnected to the system.
*/
void addNewDevices()
{
    gateway->addNewSerialPorts();
}

/**
 * Tries to delete a specific device by ID.
*/
void deleteDevice()
{
    std::string deviceId;
    std::cout << "-> Enter Device ID: " << std::endl;
    std::getline( std::cin, deviceId );

    gateway->deleteSerialDevice( deviceId );
}

/**
 * Tries to delete all devices currently registered.
*/
void deleteAllDevices()
{
    gateway->deleteAllSerialDevices();
}

/**
 * Stops the SerialPortGateway gracefully.
 *
 * @param param Param passed by the invoked signal. Does nothing.
*/
void stopGateway( int param )
{
    std::cout << std::endl << "Stopping Gateway..." << std::endl;

    gatewayStarted = false;
    gateway->stop();

    // This loop waits for the all read loops being gracefully quit, before exiting the application
    while( !gateway->isEveryReadLoopQuitted() )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }

    std::cout << "Gateway stopped." << std::endl;
    exit( 0 );
}

/**
 * Command loop which takes input form the user and decides which function needs to be called.
*/
void fetchCommandsLoop()
{
    printUsage();
    
    while ( gatewayStarted )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        std::string command;
        std::cout << "Enter Command: " << std::endl;
        std::getline( std::cin, command );

        if ( command == COMMAND_USAGE )
        {
            printUsage();
        }
        else if ( command == COMMAND_LISTDEVICES )
        {
            listDevices();
        }
        else if ( command == COMMAND_LISTSERIALPORTS )
        {
            listSerialPorts();
        }
        else if ( command == COMMAND_LISTMAPPINGS )
        {
            listDeviceIdToSerialPortMappings();
        }
        else if ( command == COMMAND_SEND )
        {
            sendMessage();
        }
        else if ( command == COMMAND_BROADCAST )
        {
            broadcastMessage();
        }
        else if ( command == COMMAND_ADDDEVICE )
        {
            addDevice();
        }
        else if ( command == COMMAND_ADDNEWDEVICES )
        {
            addNewDevices();
        }
        else if ( command == COMMAND_DELETEDEVICE )
        {
            deleteDevice();
        }
        else if ( command == COMMAND_DELETEALLDEVICES )
        {
            deleteAllDevices();
        }
        else if ( command == COMMAND_QUIT )
        {
            stopGateway( 0 );
        }
        else
        {
            std::cout << std::string( "Unknown command \"" + command + "\". Enter \"" + COMMAND_USAGE + "\" for usage." ) << std::endl;
        }
    }
}

/**
 * Main function.
 *
 * @return Returns the exitcode. 0 means success, 1 means there was an error while executing.
*/
int main( int argc, char* argv[] )
{
    signal( SIGINT, stopGateway );

    try
    {
        if ( argc < 5 )
        {
            throw std::invalid_argument( "Not all start parameters given. Format: \"" + std::string( argv[0] ) + " <configFile> <hardwareWhitelistFile> <serialPortBlacklistFile> <logPath>\"." );
        }

        std::string configFile = argv[1];
        std::string hardwareWhitelistFile = argv[2];
        std::string serialPortBlacklistFile = argv[3];
        std::string logPath = argv[4];

        gateway = new SerialPortGateway( configFile, hardwareWhitelistFile, serialPortBlacklistFile, logPath );

        std::cout << "Starting Gateway.." << std::endl;
        gateway->start();
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        std::thread( fetchCommandsLoop ).detach();
        std::cout << "Gateway started." << std::endl;
	    std::cout << "CTRL+C to exit." << std::endl;

        // Infinite Loop
        while ( gatewayStarted )
        {
            std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
        }
    }
        catch ( ConfigMalformedException & e )
    {
        std::cerr << "ConfigMalformedException: " << e.what() << std::endl;

        return 1;
    }
    catch ( ConfigMissingException & e )
    {
        std::cerr << "ConfigMissingException: " << e.what() << std::endl;

        return 1;
    }
    catch ( ConfigKeyNotFoundException & e )
    {
        std::cerr << "ConfigKeyNotFoundException: " << e.what() << std::endl;

        return 1;
    }
    catch ( NumericConfigValueMalformedException & e )
    {
        std::cerr << "NumericConfigValueMalformedException: " << e.what() << std::endl;

        return 1;
    }
    catch ( serial::IOException & e )
    {
        std::cerr << "IOException: " << e.what() << std::endl;

        return 1;
    }
    catch ( serial::SerialException & e )
    {
        std::cerr << "SerialException: " << e.what() << std::endl;

        return 1;
    }
    catch ( serial::PortNotOpenedException & e )
    {
        std::cerr << "PortNotOpenedException: " << e.what() << std::endl;

        return 1;
    }
    catch ( Exception & e )
    {
        std::cerr << "Exception: " << e.what() << std::endl;

        return 1;
    }
    catch ( std::exception & e )
    {
        std::cerr << "exception: " << e.what() << std::endl;

        return 1;
    }

    return 0;
}
