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

#ifndef SERIALDEVICE_HPP
#define SERIALDEVICE_HPP

// C++ Standard Libraries
#include <string>
#include <memory>
#include <exception>

// wjwwood's serial Library (https://github.com/wjwwood/serial)
#include "serial/serial.h"

/**
 * SerialDevice class
 * File: SerialDevice.hpp
 * Purpose: Defines a container class which makes interacting between wjwwood's serial Library and the SerialPortGateway more easy and consistent.
 *          Also serves as a wrapper/interface, because the serial implementation is planned to be replaced in the near future by an own implementation.
 *
 * @author Jan-Eric Schober
 * @version 1.0, 20.01.2019
*/
class SerialDevice
{
public:
    // Types
    typedef serial::Timeout TimeoutInfo;
    typedef serial::bytesize_t ByteSizeEnum;
    typedef serial::parity_t ParityEnum;
    typedef serial::stopbits_t StopBitsEnum;
    typedef serial::flowcontrol_t FlowControlEnum;
    typedef serial::Serial Serial;
    typedef std::shared_ptr<Serial> SerialInstance;

private:
    // Constants
    static const unsigned int BAUDRATE = 9600;
    static const TimeoutInfo TIMEOUT;

    // Variables
    std::string port;
    unsigned int baudRate;
    TimeoutInfo timeout;
    ByteSizeEnum byteSize;
    ParityEnum parity;
    StopBitsEnum stopBits;
    FlowControlEnum flowControl;
    std::string id;
    SerialInstance instance;

    // Methods
    /**
     * Sets the SerialInstance to be used by/with the SerialDevice.
     *
     * @param instance Instance to be set as the SerialDevice's serial communication interface of type SerialInstance (std::shared_ptr<Serial>).
     * @return void
    */
    void setInstance( SerialInstance instance );

public:
    // Constructors
    /**
     * Default constructor.
     *
     * @param port Path to the serial port to connect to.
     * @param baudRate Baud rate to be used.
     * @param timeout Timeout information to be set.
     * @param byteSize Byte size to be used for communicating.
     * @param parity Parity option to be used for communicating.
     * @param stopBits Stop bit configuration for the connection.
     * @param flowControl Flow control configuration for the connection.
    */
    SerialDevice(
        std::string port,
        unsigned int baudRate = BAUDRATE,
        TimeoutInfo timeout = TimeoutInfo(),
        ByteSizeEnum byteSize = ByteSizeEnum::eightbits,
        ParityEnum parity = ParityEnum::parity_none,
        StopBitsEnum stopBits = StopBitsEnum::stopbits_one,
        FlowControlEnum flowControl = FlowControlEnum::flowcontrol_none
    );

    // Destructors
    /**
     * Destructor.
    */
    ~SerialDevice();

    // Methods
    /**
     * Sets the serial port to be used.
     *
     * @param port Path to the serial port to connect to.
    */
    void setPort( std::string port );

    /**
     * Gets the serial port currently set.
     *
     * @return Current path to the serial port.
    */
    std::string getPort();

    /**
     * Sets the baud rate to be used for communication.
     *
     * @param baudRate Baud rate to be set.
    */
    void setBaudRate( unsigned int baudRate );

    /**
     * Gets the baud rate currently set.
     *
     * @return Current baud rate.
    */
    unsigned int getBaudRate();

    /**
     * Sets the timeout to be used while communicating.
     *
     * @param timeout Timeout to be set.
    */
    void setTimeout( TimeoutInfo timeout );

    /**
     * Gets the timeout currently set.
     *
     * @return Current timeout.
    */
    TimeoutInfo getTimeout();

    /**
     * Sets the byte size to be used for communication.
     *
     * @param byteSize Byte size to be set.
    */
    void setByteSize( ByteSizeEnum byteSize );

    /**
     * Gets the byteSize currently set.
     *
     * @return Current byte size.
    */
    ByteSizeEnum getByteSize();

    /**
     * Sets the parity option to be used for communication.
     *
     * @param parity Parity option to be set.
    */
    void setParity( ParityEnum parity );

    /**
     * Gets the parity option currently set.
     *
     * @return Current parity option.
    */
    ParityEnum getParity();

    /**
     * Sets the stop bit configuration to be used for the connection.
     *
     * @param stopBits Stop bit configuration to be set.
    */
    void setStopBits( StopBitsEnum stopBits );

    /**
     * Gets the parity configuration currently set.
     *
     * @return Current stop bit configuration.
    */
    StopBitsEnum getStopBits();

    /**
     * Sets the flow control configuration to be used for the connection.
     *
     * @param flowControl Flow control configuration to be set.
    */
    void setFlowControl( FlowControlEnum flowControl );

    /**
     * Gets the flow control configuration currently set.
     *
     * @return Current flow control configuration.
    */
    FlowControlEnum getFlowControl();

    /**
     * Sets the ID of the serial device.
     *
     * @param id ID to be set.
    */
    void setId( std::string id );

    /**
     * Gets the ID currently set.
     *
     * @return Current ID.
    */
    std::string getId();

    /**
     * Initializes a serial instance if getInstance() == nullptr.
    */
    void init();

    /**
     * Gets the SerialInstance currently set.
     *
     * @return Current SerialInstance.
    */
    SerialInstance getInstance();
};

#endif // SERIALDEVICE_HPP
