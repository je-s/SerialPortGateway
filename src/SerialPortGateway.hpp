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

#ifndef SERIALPORTGATEWAY_HPP
#define SERIALPORTGATEWAY_HPP

// C++ Standard Libraries
#include <atomic> // std::atomic_bool
#include <string> // std::string, std::getline, std::string::npos
#include <exception>
#include <map> // std::map
#include <set> // std::set
#include <sstream> // std::stringstream
#include <fstream>  // std::ifstream
#include <thread> // std::thread, std::this_thread::sleep_for

// wjwwood's serial Library (https://github.com/wjwwood/serial)
#include "serial/serial.h"

#include "SerialDevice.hpp"
#include "SerialMessage.hpp"
#include "../dependencies/Exception/src/Exception.hpp"
#include "../dependencies/Config/src/Config.hpp"
#include "../dependencies/Logger/src/Logger.hpp"

/**
 * SerialPortGateway class
 * File: SerialPortGateway.hpp
 * Purpose: Defines a class which manages the communication of serial devices by device IDs, and acts as a gateway to those.
 *          It's (e.g.) possible to receive and process messages, and also to deliver specific messages to specific devices.
 *          The class can be configured via a config file, can make use of a hardware ID whitelist and a serial port blacklist.
 *          Errors (IO Errors for example) are handled automatically, and every action or failure is logged either (optionally to a file).
 *
 * @author Jan-Eric Schober
 * @version 1.0, 20.01.2019
*/
class SerialPortGateway
{
private:
    // Types
    typedef std::shared_ptr<SerialDevice> SerialDevicePointer; // Shared Pointers are used, so a detached thread can finish it's operation even when the SerialDevice has been deleted from our SerialDevice list.
    typedef std::map<std::string, SerialDevicePointer> SerialDeviceMap; // first value: deviceId, second value: SerialDevicePointer
    typedef std::set<std::string> StringSet;
    typedef std::pair<std::string, std::string> StringPair;
    typedef std::pair<std::atomic<bool>, std::atomic<bool>> AtomicBoolPair;
    typedef std::map<std::string, AtomicBoolPair> AtomicBoolPairMap;

    // Constants
    static const std::string CHAR_SPACE;
    static const std::string CHAR_NEWLINE;
    static const std::string CHAR_CARRIAGE_RETURN;
    static const std::string LIST_SEPARATOR;

    // Variables
    std::string configFile;
    std::string hardwareWhitelistFile;
    std::string serialPortBlacklistFile;
    std::string logPath;
    bool loggingActive;
    unsigned int scanInterval;
    unsigned int waitBeforeCommunication;
    unsigned int baudRate;
    std::string messageDelimiter;
    std::string commandToGetDeviceId;
    std::string messageTypeForIds;
    Config * configInstance;
    Logger * loggerInstance;
    std::atomic_bool started;
    StringSet hardwareWhitelist; // Contains all whitelisted hardwareIds
    StringSet serialPortBlacklist; // Contains all blacklisted serialPorts
    SerialDeviceMap serialDevices; // Contains a mapping between all registered deviceIds and SerialDevicePointers. ( deviceId -> SerialDevicePointer )
    AtomicBoolPairMap readLoopStates; // Contains a mapping between all registered deviceIds, and whether the loop is started, respectively quitted. ( deviceId -> <started, quitted> )

    // Methods
    /**
     * Sets the path to the config file which shall be loaded.
     *
     * @param configFile Path to the config file to be used.
    */
    void setConfigFile( std::string configFile );

    /**
     * Gets the currently set path to the config file.
     *
     * @return Current path to the config file.
    */
    std::string getConfigFile();

    /**
     * Sets the path to the hardware whitelist file which shall be loaded.
     *
     * @param hardwareWhitelistFile Path to the hardware whitelist file to be used.
    */
    void setHardwareWhitelistFile( std::string hardwareWhitelistFile );

    /**
     * Gets the currently set path to hardware whitelist file.
     *
     * @return Current path to the hardware whitelist file.
    */
    std::string getHardwareWhitelistFile();

    /**
     * Sets the path to the serial port blacklist file which shall be loaded.
     *
     * @param serialPortBlacklistFile Path to the serial port blacklist file to be used.
    */
    void setSerialPortBlacklistFile( std::string serialPortBlacklistFile );

    /**
     * Gets the currently set path to serial port blacklist file.
     *
     * @return Current path to the serial port blacklist file.
    */
    std::string getSerialPortBlacklistFile();

    /**
     * Sets the file path to where the logs shall be written.
     *
     * @param logPath File path to write the logs to.
    */
    void setLogPath( std::string logPath );

    /**
     * Gets the currently set file path to where the logs get written to.
     *
     * @return Current path to where the logs should be written.
    */
    std::string getLogPath();

    /**
     * Sets whether logging to a file is active or not.
     *
     * @param loggingActive Logging to a file true/false.
    */
    void setLoggingActive( bool loggingActive );

    /**
     * Gets whether logging to a file is active or not.
     *
     * @return Logging active?
    */
    bool isLoggingActive();

    /**
     * Sets the interval (in ms) in which should be searched for new serial devices.
     * Zero (0) means that there will only be one initial scan, but no further automatic, periodical scans.
     *
     * @param scanInterval Scan interval in ms.
    */
    void setScanInterval( unsigned int scanInterval );

    /**
     * Gets the currently set scan interval.
     *
     * @return Scan interval in ms.
    */
    unsigned int getScanInterval();

    /**
     * Sets a waiting time before an inital communication attempt to a device.
     *
     * @param waitBeforeCommunication Waiting time in ms.
    */
    void setWaitBeforeCommunication( unsigned int waitBeforeCommunication );

    /**
     * Gets the currently set waiting time before a first communicaiton attempt to a device.
     *
     * @return Waiting time in ms.
    */
    unsigned int getWaitBeforeCommunication();

    /**
     * Sets the baud rate to be used for communication from and to the serial device.
     *
     * @param baudRate Current baud rate.
    */
    void setBaudRate( unsigned int baudRate );

    /**
     * Gets the currently set baud rate.
     *
     * @return Current baud rate.
    */
    unsigned int getBaudRate();

    /**
     * Sets the message delimiter to be used for interpreting messages from serial devices.
     * The message delimiter separates the message type from the message content.
     *
     * @param messageDelimiter The message delimiter to be used. Can be a single or multiple characters.
    */
    void setMessageDelimiter( std::string messageDelimiter );

    /**
     * Gets the currently set message delimiter.
     *
     * @return Current message delimiter.
    */
    std::string getMessageDelimiter();

    /**
     * Sets the command which is used to request/retrieve the device ID for each of the connected devices.
     * This command needs to be understood by every device connected and result in a validly formatted message (see ReadMe) directly after,
     * which corresponds to the message type declared with the "setMessageTypeForIds" function and contains the device's ID.
     *
     * @param commandToGetDeviceId Command which gets send to each device to request/retrieve the device ID.
    */
    void setCommandToGetDeviceId( std::string commandToGetDeviceId );

    /**
     * Gets the currently set command to request/retrieve the device ID from devices.
     *
     * @return Currently set command to get device ID.
    */
    std::string getCommandToGetDeviceId();

    /**
     * Sets the message type for messages which are intended to contain a device ID.
     * This message type needs to be used whenever a device is answereing the command set via "setCommandToGetDeviceId" with a
     * validly formatted message (see ReadMe), in order to allow the gateway to identify the correct response and extract the contained device ID.
     *
     * @param messageTypeForIds Message type of a message containing the device ID.
     * @return
    */
    void setMessageTypeForIds( std::string messageTypeForIds );

    /**
     * Gets the currently set message type for messages containing the device ID.
     *
     * @return Currently set message type for device IDs.
    */
    std::string getMessageTypeForIds();

    /**
     * Sets whether the gateway is started or not.
     *
     * @param started Started true/false.
    */
    void setStarted( bool started );

    /**
     * Sets the config instance to be used.
     *
     * @param configInstance Pointer to config instance.
    */
    void setConfigInstance( Config * configInstance );

    /**
     * Initializes the config instance, and loads the config file.
    */
    void initConfig();

    /**
     * Deletes the config instance.
    */
    void deleteConfigInstance();

    /**
     * Sets the logger instance to be used.
     *
     * @param loggerInstance Pointer to logger instance.
    */
    void setLoggerInstance( Logger * loggerInstance );

    /**
     * Initializes the logger instance.
    */
    void initLogger();

    /**
     * Deletes the logger instance.
    */
    void deleteLoggerInstance();

    /**
     * Loads the hardware whitelist.
    */
    void loadHardwareWhitelist();

    /**
     * Gets the hardware whitelist currently loaded.
     *
     * @return Pointer to hardware whitelist.
    */
    StringSet * getHardwareWhitelist();

    /**
     * Gets whether the hardware whitelist is empty or not.
     *
     * @return Hardware whitelist empty?
    */
    bool isHardwareWhitelistEmpty();

    /**
     * Checks whether the hardware whitelist has a specific entry or not.
     *
     * @param hardwareId Hardware ID to search for.
     * @return Hardware ID contained or not?
    */
    bool hasHardwareWhitelistEntry( std::string hardwareId );

    /**
     * Retrieves the serial port's hardware ID.
     *
     * @param serialPort Serial port to get the hardware ID from.
     * @return Serial port's hardware ID. Returns an empty string in case no ID could be retrieved.
    */
    std::string getHardwareId( std::string serialPort );

    /**
     * Loads the serial port blacklist.
    */
    void loadSerialPortBlacklist();

    /**
     * Gets the serial port blacklist currently loaded.
     *
     * @return Pointer to serial port blacklist.
    */
    StringSet * getSerialPortBlacklist();

    /**
     * Gets whether the serial port blacklist is empty or not.
     *
     * @return Serial port blacklist empty?
    */
    bool isSerialPortBlacklistEmpty();

    /**
     * Checks whether the serial port blacklist has a specific entry or not.
     *
     * @param serialPort Serial port to search for.
     * @return Serial port contained or not?
    */
    bool hasSerialPortBlacklistEntry( std::string serialPort );

    /**
     * Loop for periodically adding new serial ports as serial devices to the gateway.
     * This loop is at least executed once. If "getScanInterval" returns 0, it stops executing after the first run.
     * This loop gets solely called in a detached thread by the "start" function.
    */
    void addNewSerialPortsLoop();

    /**
     * Gets a mapping of all currently registered serial devices.
     * The container maps between deviceIds and SerialDevicePointers.
     * ( deviceId -> SerialDevicePointer )
     *
     * @return Pointer to a mapping.
    */
    SerialDeviceMap * getSerialDevices();

    /**
     * Gets a mapping of all read loop states.
     * The container maps between deviceIds and a pair of atomic bools, which contain whether a loop is started/quitted.
     * ( deviceId -> <started, quitted> )
     *
     * @return Pointer to a mapping.
    */
    AtomicBoolPairMap * getReadLoopStates();

    /**
     * Gets a serial device by its device ID.
     *
     * @param deviceId Device ID to search a device for.
     * @return SerialDevicePointer, or null if nothing found.
    */
    SerialDevicePointer getSerialDeviceById( std::string deviceId );

    /**
     * Gets a serial device by its serial port.
     *
     * @param serialPort Serial port to search a device for.
     * @return SerialDevicePointer, or null if nothing found.
    */
    SerialDevicePointer getSerialDeviceByPort( std::string serialPort );

    /**
     * Initialises the serial device (and retrieves the deviceId).
     *
     * @param serialDevice Serial device pointer -with all it's parameters- to initialise.
     * @return Whether the serial device was successfully initialised or not.
    */
    bool initSerialDevice( SerialDevicePointer serialDevice );

    /**
     * Loop for reading data from a serial device.
     * In case there's a new line from the serial device, a detached thread gets spawned where "processMessage" gets executed.
     * In case there's an error occuring while reading from the device, a corresponding message gets logged and the device gets deleted.
     * This loop gets solely called in a detached thread by the "startReadLoop" function.
     *
     * @param deviceId Device ID we're running this readLoop for.
    */
    void readLoop( std::string deviceId );

    /**
     * Starts a read loop for a specific deviceId.
     *
     * @param deviceId Device ID we're starting a read loop for.
    */
    void startReadLoop( std::string deviceId );

    /**
     * Stops a read loop for a specific deviceId.
     *
     * @param deviceId Device ID of which we want to stop the read loop.
    */
    void stopReadLoop( std::string deviceId );

    /**
     * Stops the read loops of all devices.
    */
    void stopAllReadLoops();

    /**
     * Sets the state of whether a specific device IDs' read loop is started.
     *
     * @param deviceId Device ID to set the read loop state for.
     * @param started Whether the read loop is started or not.
    */
    void setReadLoopStarted( std::string deviceId, bool started );

    /**
     * Checks whether a specific read loop is started or not.
     *
     * @param deviceId Device ID to check for.
     * @return Whether the read loop is started or not. Also returns false if the device ID was not found.
    */
    bool isReadLoopStarted( std::string deviceId );

    /**
     * Sets the state of whether a specific device IDs' read loop is quitted.
     *
     * @param deviceId Device ID to set the read loop state for.
     * @param started Whether the read loop is quitted or not. Also returns false if the device ID was not found.
    */
    void setReadLoopQuitted( std::string deviceId, bool quitted );

    /**
     * Checks whether a specific read loop is quitted or not.
     *
     * @param deviceId Device ID to check for.
     * @return Whether the read loop is quitted or not. Also returns false fi the device ID was not found.
    */
    bool isReadLoopQuitted( std::string deviceId );

    /**
     * Retrieves a device ID for a device on a specific serial port.
     * If the device ID could be retrieved successfully, it gets set in the passed SerialDevice object's deviceId attribute.
     *
     * @param serialDevice SerialDevicePointer to a SerialDevice object, which has no ID yet.
     * @return Whether the ID could be retrieved or not.
    */
    bool retrieveDeviceId( SerialDevicePointer serialDevice );

    /**
     * Parses a message from a serial device into a StringPair containing the type and content.
     * ( type, content )
     *
     * @param message Message to be parsed.
     * @param delimiter Delimiter to use.
     * @return StringPair containing the type and content of the message.
    */
    StringPair parseMessage( std::string message, std::string delimiter );

    /**
     * Processes a message from a serial device.
     * After parsing, the function calls the "messageCallback" in a detached thread, which can be redefined by inheriting classes.
     *
     * @param deviceId The device ID the message is coming from.
     * @param message The message to process.
    */
    void processMessage( std::string deviceId, std::string message );

    /**
     * Unlike "sendMessageToSerialDevice", this function takes over the sending of a message to a device.
     * This function gets solely called in a detached thread by the "sendMessageToSerialDevice" function.
     *
     * @param deviceId Device ID to send the message to.
     * @param message Message to send to the device.
    */
    void sendMessageToSerialDeviceBlocking( std::string deviceId, std::string message );

protected:
    // Methods
    /**
     * Gets the config instance.
     *
     * @return Pointer to the current config instance.
    */
    Config * getConfigInstance();

    /**
     * Gets the logger instance.
     *
     * @return Pointer to the current config instance.
    */
    Logger * getLoggerInstance();

public:
    // Constructors
    /**
     * Default constructor.
     *
     * @param configFile Path to the config file which shall be loaded.
     * @param hardwareWhitelistFile Path to the hardware whitelist file which shall be loaded. Must not be empty.
     * @param serialPortBlacklistFile Path to the serial port blacklist file which shall be loaded.
     * @param logPath Path to which the log file should be written.
    */
    SerialPortGateway( std::string configFile, std::string hardwareWhitelistFile, std::string serialPortBlacklistFile, std::string logPath = "./logs" );

    // Destructors
    /**
     * Destructor.
    */
    ~SerialPortGateway();

    // Methods
    /**
     * Tries to add/register a new serial device to the gateway.
     * In case there's an error occuring while trying to add the device, a corresponding message gets logged.
     *
     * @param serialPort Serial port on which the device is mapped.
     * @param suppressLogs Whether to suppress log messages about the serial port being blacklisted, and the serial port already being added/registered.
     *                     This is to suppress log spam if this function gets called periodically/more often by, for instance, "addNewSerialPorts".
     *                     This could be done in the loop already (before calling "addSerialDevice"), but this is the more central approach without
     *                     doing some checks twice (If the device has not been added yet/is not blacklisted).
     * @return Whether the device has been successfully added or not.
    */
    bool addSerialDevice( std::string serialPort, bool suppressLogs = false );

    /**
     * Adds newly available serial ports, which habe not yet been added to the gateway.
     *
     * @param suppressLogs Whether to suppress log messages about the process of adding new serial ports.
     *                     This is to suppress log spam if this function gets called in a loop, for instance in "addNewSerialPortsLoop".
     * @return Number of new devices added.
    */
    unsigned int addNewSerialPorts( bool suppressLogs = false );

    /**
     * Tries to delete a serial device form the gateway.
     *
     * @param deviceId Device ID to delete.
     * @return Whether the device has been successfully deleted or not.
    */
    bool deleteSerialDevice( std::string deviceId );

    /**
     * Tries to delete all serial devices currently registered.
     * If "stop" is not used, this can be used to gracefully shut down all serial connections and remove all serial devices from the gateway.
     *
     * @param suppressLogs Whether to suppress log messages about the process of deleting serial devices.
     * @return Number of devices deleted.
    */
    unsigned int deleteAllSerialDevices( bool suppressLogs = false );

    /**
     * Starts the gateway; initializes the automatic scanning for new devices.
     * If you don't want to use the automatic scan function at all, and don't want to have "addNewSerialPorts" executed even once, just don't call this method.
    */
    void start();

    /**
     * Stops the gateway; stops the automatic scanning of new devices, and deletes all devices currently registered.
    */
    void stop();

    /**
     * Gets whether the gateway is started.
     *
     * @return Whether the gateway is started or not.
    */
    bool isStarted();

    /**
     * Gets whether every read loop is quitted.
     * This only returns true, if every registered read loop has been stopped AND quitted; means all loops have successfully exited and come to an end.
     * This can be used to check whether it's safe to shutdown the gateway, or if there are any read operations still ongoing.
     *
     * @return Whether every read loop has been quitted or not.
    */
    bool isEveryReadLoopQuitted();

    /**
     * Gets all currently registered device IDs.
     *
     * @return A vector of strings containing all device IDs.
    */
    std::vector<std::string> getDeviceIds();

    /**
     * Gets a list of all currently registered device IDs, separated by LIST_SEPARATOR.
     *
     * @return String list containing all device IDs.
    */
    std::string getDeviceIdList();

    /**
     * Gets all of the system's available serial ports.
     *
     * @return A vector of strings containing all serial ports.
    */
    std::vector<std::string> getSerialPorts();

    /**
     * Gets a list of the system's available serial ports, separated by LIST_SEPARATOR.
     *
     * @return String list containing all serial ports.
    */
    std::string getSerialPortList();

    /**
     * Gets all mappings between connected devices by their ID & their corresponding serial port.
     *
     * @return A map of strings containing all device ID to serial port mappings.
    */
    std::map<std::string, std::string> getDeviceIdToSerialPortMappings();

    /**
     * Gets a list of all mappings between connected devices by their ID & their corresponding serial port, separated by LIST_SEPARATOR.
     *
     * @return String list containing all mappings.
    */
    std::string getDeviceIdToSerialPortMappingList();

    /**
     * Sends a message to a specific device ID.
     * This function calls "sendMessageToSerialDeviceBlocking" in a detached thread (making it async), which handles the main process of delivering messages to a device.
     *
     * @param deviceId Device ID to send the message to.
     * @param message Message to send to the device.
    */
    void sendMessageToSerialDevice( std::string deviceId, std::string message );

    /**
     * Broadcasts a message to all registered serial devices.
     *
     * @param message Message to send to all devices.
    */
    void broadcastMessageToSerialDevices( std::string message );

    /**
     * Callback which gets called when a new device got added.
     * This function can be redefined by inheriting classes.
     *
     * @param deviceId Device ID which has been added.
     * @param serialPort Serial port on which the device has been added.
    */
    virtual void serialDeviceAddedCallback( std::string deviceId, std::string serialPort );

    /**
     * Callback which gets called when a device got deleted.
     * This function can be redefined by inheriting classes.
     *
     * @param deviceId Device ID which has been deleted.
     * @param serialPort Serial port which the device had which has been deleted.
    */
    virtual void serialDeviceDeletedCallback( std::string deviceId, std::string serialPort );

    /**
     * Callback which gets called when a new message arrives.
     * This function can be redefined by inheriting classes.
     *
     * @param serialMessage Serial message instance containing all data necessary for further processing.
    */
    virtual void messageCallback( SerialMessage serialMessage );
};

#endif // SERIALPORTGATEWAY_HPP
