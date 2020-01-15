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

#include "SerialMessage.hpp"

SerialMessage::SerialMessage()
{
    setDeviceId( "" );
    setTimestamp( 0 );
    setType( "" );
    setContent( "" );
}

SerialMessage::SerialMessage( std::string deviceId, unsigned long long timestamp, std::string type, std::string content )
{
    setDeviceId( deviceId );
    setTimestamp( timestamp );
    setType( type );
    setContent( content );
}

SerialMessage::SerialMessage( std::string deviceId, std::string type, std::string content )
{
    setDeviceId( deviceId );
    setTimestamp( getCurrentTimestamp() );
    setType( type );
    setContent( content );
}

SerialMessage::SerialMessage( std::string type, std::string content )
{
    setDeviceId( "" );
    setTimestamp( getCurrentTimestamp() );
    setType( type );
    setContent( content );
}

SerialMessage::~SerialMessage()
{

}

void SerialMessage::setDeviceId( std::string deviceId )
{
    this->deviceId = deviceId;
}

std::string SerialMessage::getDeviceId()
{
    return this->deviceId;
}

void SerialMessage::setTimestamp( unsigned long long timestamp )
{
    this->timestamp = timestamp;
}

unsigned long long SerialMessage::getTimestamp()
{
    return this->timestamp;
}

unsigned long long SerialMessage::getCurrentTimestamp()
{
    std::chrono::system_clock::duration durationSinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    unsigned long long currentTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch).count();

    return currentTimestamp;
}

void SerialMessage::setType( std::string type )
{
    this->type = type;
}

std::string SerialMessage::getType()
{
    return this->type;
}

void SerialMessage::setContent( std::string content )
{
    this->content = content;
}

std::string SerialMessage::getContent()
{
    return this->content;
}
