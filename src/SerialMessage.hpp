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

#ifndef SERIALMESSAGE_HPP
#define SERIALMESSAGE_HPP

// C++ Standard Libraries
#include <string>
#include <chrono>
#include <iostream>

/**
 * SerialMessage class
 * File: SerialMessage.hpp
 * Purpose: Defines a container class which stores all information correlated to serial messages, and also generates timestamps if required.
 *
 * @author Jan-Eric Schober
 * @version 1.0, 20.01.2019
*/
class SerialMessage
{
private:
    // Variables
    std::string deviceId;
    unsigned long long timestamp;
    std::string type;
    std::string content;

    // Methods
    /**
     * Gets the current system's timestamp.
     *
     * @return Current timestamp.
    */
    static unsigned long long getCurrentTimestamp();

public:
    // Constructors
    /**
     * Default constructor.
    */
    SerialMessage();

    /**
     * Basic constructor.
     *
     * @param deviceId Device ID to be set.
     * @param timestamp Timestamp to be set.
     * @param type Message type to be set.
     * @param content Message content to be set.
    */
    SerialMessage(
        std::string deviceId,
        unsigned long long timestamp,
        std::string type,
        std::string content
    );

    /**
     * Constructor - timestamp gets retrieved from the system.
     *
     * @param deviceId Device ID to be set.
     * @param type Message type to be set.
     * @param content Message content to be set.
    */
    SerialMessage(
        std::string deviceId,
        std::string type,
        std::string content
    );

    /**
     * Constructor - timestamp gets retrieved from the system and deviceId is left empty.
     *
     * @param type Message type to be set.
     * @param content Message content to be set.
    */
    SerialMessage(
        std::string type,
        std::string content
    );

    // Destructors
    /**
     * Destructor.
    */
    ~SerialMessage();

    // Methods
    /**
     * Sets the device ID from which the message is.
     *
     * @param deviceId Device ID to be set.
    */
    void setDeviceId( std::string deviceId );

    /**
     * Gets the device ID currently set.
     *
     * @return Current device ID.
    */
    std::string getDeviceId();

    /**
     * Sets the timestamp on which the message appeared.
     *
     * @param timestamp Timestamp to be set.
    */
    void setTimestamp( unsigned long long timestamp );

    /**
     * Gets the timestamp currently set.
     *
     * @return Current timestamp.
    */
    unsigned long long getTimestamp();

    /**
     * Sets the message's type.
     *
     * @param type Message type to be set.
    */
    void setType( std::string type );

    /**
     * Gets the message type currently set.
     *
     * @return Current type.
    */
    std::string getType();

    /**
     * Sets the content of the message.
     *
     * @param content Message content to be set.
    */
    void setContent( std::string content );

    /**
     * Gets the message's current content.
     *
     * @return Current content.
    */
    std::string getContent();
};

#endif // SERIALMESSAGE_HPP
