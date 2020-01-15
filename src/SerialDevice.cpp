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

#include "SerialDevice.hpp"

SerialDevice::SerialDevice(
    std::string port,
    unsigned int baudRate,
    TimeoutInfo timeout,
    ByteSizeEnum byteSize,
    ParityEnum parity,
    StopBitsEnum stopBits,
    FlowControlEnum flowControl
)
{
    setPort( port );
    setBaudRate( baudRate );
    setTimeout( timeout );
    setByteSize( byteSize );
    setParity( parity );
    setStopBits( stopBits );
    setFlowControl( flowControl );
}

SerialDevice::~SerialDevice()
{
    // In the future -with another library- this will be used to flush & close the communication buffers.
}

void SerialDevice::setPort( std::string port )
{
    if ( port.empty() )
    {
        throw std::invalid_argument( "Port must not be empty." );
    }

    this->port = port;
}

std::string SerialDevice::getPort()
{
    return this->port;
}

void SerialDevice::setBaudRate( unsigned int baudRate )
{
    this->baudRate = baudRate;
}

unsigned int SerialDevice::getBaudRate()
{
    return this->baudRate;
}

void SerialDevice::setTimeout( TimeoutInfo timeout )
{
    this->timeout = timeout;
}

SerialDevice::TimeoutInfo SerialDevice::getTimeout()
{
    return this->timeout;
}

void SerialDevice::setByteSize( ByteSizeEnum byteSize )
{
    this->byteSize = byteSize;
}

SerialDevice::ByteSizeEnum SerialDevice::getByteSize()
{
    return this->byteSize;
}

void SerialDevice::setParity( ParityEnum parity )
{
    this->parity = parity;
}

SerialDevice::ParityEnum SerialDevice::getParity()
{
    return this->parity;
}

void SerialDevice::setStopBits( StopBitsEnum stopBits )
{
    this->stopBits = stopBits;
}

SerialDevice::StopBitsEnum SerialDevice::getStopBits()
{
    return this->stopBits;
}

void SerialDevice::setFlowControl( FlowControlEnum flowControl )
{
    this->flowControl = flowControl;
}

SerialDevice::FlowControlEnum SerialDevice::getFlowControl()
{
    return this->flowControl;
}

void SerialDevice::setId( std::string id )
{
    // if ( id.empty() )
    // {
    //     throw std::invalid_argument( "Serial-ID must be at least 1 character long (Port: " + getPort() + ")." );
    // }

    this->id = id;
}

std::string SerialDevice::getId()
{
    return this->id;
}

void SerialDevice::init()
{
    // Only initialize if there is no instance present yet.
    if ( getInstance() != nullptr )
    {
        return;
    }

    // Create a new Serial instance in conjunction with a smart pointer, and share the ownership with the SerialDevice afterwards.
    SerialInstance instance = std::make_shared<Serial>( getPort(), getBaudRate(), getTimeout(), getByteSize(), getParity(), getStopBits(), getFlowControl() );
    setInstance( instance );
}

void SerialDevice::setInstance( SerialInstance instance )
{
    this->instance = instance;
}

SerialDevice::SerialInstance SerialDevice::getInstance()
{
    return this->instance;
}
