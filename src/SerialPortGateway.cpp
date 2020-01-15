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

#include "SerialPortGateway.hpp"

const std::string SerialPortGateway::CHAR_SPACE = " ";
const std::string SerialPortGateway::CHAR_NEWLINE = "\n";
const std::string SerialPortGateway::CHAR_CARRIAGE_RETURN = "\r";
const std::string SerialPortGateway::LIST_SEPARATOR = ",";

SerialPortGateway::SerialPortGateway(
    std::string configFile,
    std::string hardwareWhitelistFile,
    std::string serialPortBlacklistFile,
    std::string logPath
)
{
    setConfigFile( configFile );
    setHardwareWhitelistFile( hardwareWhitelistFile );
    setSerialPortBlacklistFile( serialPortBlacklistFile );
    setLogPath( logPath );
    setStarted( false );

    initConfig();
    initLogger();
    loadHardwareWhitelist();
    loadSerialPortBlacklist();
}

SerialPortGateway::~SerialPortGateway()
{
    stop();
    deleteLoggerInstance();
    deleteConfigInstance();
}

void SerialPortGateway::setConfigFile( std::string configFile )
{
    if ( configFile.empty() )
    {
        throw Exception( "Path to config file must not be empty." );
    }

    this->configFile = configFile;
}

std::string SerialPortGateway::getConfigFile()
{
    return this->configFile;
}

void SerialPortGateway::setHardwareWhitelistFile( std::string hardwareWhitelistFile )
{
    this->hardwareWhitelistFile = hardwareWhitelistFile;
}

std::string SerialPortGateway::getHardwareWhitelistFile()
{
    return this->hardwareWhitelistFile;
}

void SerialPortGateway::setSerialPortBlacklistFile( std::string serialPortBlacklistFile )
{
    this->serialPortBlacklistFile = serialPortBlacklistFile;
}

std::string SerialPortGateway::getSerialPortBlacklistFile()
{
    return this->serialPortBlacklistFile;
}

void SerialPortGateway::setLogPath( std::string logPath )
{
    if ( logPath.empty() )
    {
        throw Exception( "Log path must not be empty." );
    }

    this->logPath = logPath;
}

std::string SerialPortGateway::getLogPath()
{
    return this->logPath;
}

void SerialPortGateway::setLoggingActive( bool loggingActive )
{
    this->loggingActive = loggingActive;
}

bool SerialPortGateway::isLoggingActive()
{
    return this->loggingActive;
}

void SerialPortGateway::setScanInterval( unsigned int scanInterval )
{
    this->scanInterval = scanInterval;
}

unsigned int SerialPortGateway::getScanInterval()
{
    return this->scanInterval;
}

void SerialPortGateway::setWaitBeforeCommunication( unsigned int waitBeforeCommunication )
{
    this->waitBeforeCommunication = waitBeforeCommunication;
}

unsigned int SerialPortGateway::getWaitBeforeCommunication()
{
    return this->waitBeforeCommunication;
}

void SerialPortGateway::setBaudRate( unsigned int baudRate )
{
    if ( baudRate <= 0 )
    {
        throw Exception( "Baud Rate must be > 0." );
    }

    this->baudRate = baudRate;
}

unsigned int SerialPortGateway::getBaudRate()
{
    return this->baudRate;
}

void SerialPortGateway::setMessageDelimiter( std::string messageDelimiter )
{
    if ( messageDelimiter.empty() )
    {
        throw Exception( "Message delimiter must not be empty." );
    }

    this->messageDelimiter = messageDelimiter;
}

std::string SerialPortGateway::getMessageDelimiter()
{
    return this->messageDelimiter;
}

void SerialPortGateway::setCommandToGetDeviceId( std::string commandToGetDeviceId )
{
    if ( commandToGetDeviceId.empty() )
    {
        throw Exception( "Command for getting the Device ID must not be empty." );
    }

    this->commandToGetDeviceId = commandToGetDeviceId;
}

std::string SerialPortGateway::getCommandToGetDeviceId()
{
    return this->commandToGetDeviceId;
}

void SerialPortGateway::setMessageTypeForIds( std::string messageTypeForIds )
{
    if ( messageTypeForIds.empty() )
    {
        throw Exception( "Message type for IDs must not be empty." );
    }

    this->messageTypeForIds = messageTypeForIds;
}

std::string SerialPortGateway::getMessageTypeForIds()
{
    return this->messageTypeForIds;
}

void SerialPortGateway::setConfigInstance( Config * configInstance )
{
    if ( configInstance == nullptr )
    {
        throw Exception( "Config instance must not be null." );
    }

    this->configInstance = configInstance;
}

Config * SerialPortGateway::getConfigInstance()
{
    return this->configInstance;
}

void SerialPortGateway::initConfig()
{
    Config * config = new Config( getConfigFile() );
    setConfigInstance( config );

    bool loggingActive = config->getBool( "LOGGING_ACTIVE" );
    unsigned int scanInterval = config->getUnsignedInteger( "SCAN_INTERVAL" );
    unsigned int waitBeforeCommunication = config->getUnsignedInteger( "WAIT_BEFORE_COMMUNICATION" );
    unsigned int baudRate = config->getUnsignedInteger( "BAUD_RATE" );
    std::string messageDelimiter = config->getString( "MESSAGE_DELIMITER" );
    std::string commandToGetDeviceId = config->getString( "COMMAND_GETID" );
    std::string messageTypeForIds = config->getString( "MESSAGE_TYPE_ID" );

    setLoggingActive( loggingActive );
    setScanInterval( scanInterval );
    setWaitBeforeCommunication( waitBeforeCommunication );
    setBaudRate( baudRate );
    setMessageDelimiter( messageDelimiter );
    setCommandToGetDeviceId( commandToGetDeviceId );
    setMessageTypeForIds( messageTypeForIds );
}

void SerialPortGateway::deleteConfigInstance()
{
    delete getConfigInstance();
}

void SerialPortGateway::setLoggerInstance( Logger * loggerInstance )
{
    if ( loggerInstance == nullptr )
    {
        throw Exception( "Logger instance must not be null." );
    }

    this->loggerInstance = loggerInstance;
}

Logger * SerialPortGateway::getLoggerInstance()
{
    return this->loggerInstance;
}

void SerialPortGateway::initLogger()
{
    Logger * logger = new Logger( "SerialPortGateway", getLogPath(), true, isLoggingActive(), true );
    logger->writeInfo( "Logger initialized." );

    setLoggerInstance( logger );
}

void SerialPortGateway::deleteLoggerInstance()
{
    delete getLoggerInstance();
}

void SerialPortGateway::loadHardwareWhitelist()
{
    std::string fileName = getHardwareWhitelistFile();

    if ( fileName.empty() )
    {
        throw Exception( "Path to Hardware-ID Whitelist must not be empty." );
    }

    std::ifstream fileStream( fileName, std::ifstream::out | std::ifstream::app );

    if ( !fileStream.is_open() )
    {
        throw Exception( "Couldn't open Hardware-ID Whitelist. (Path: \"" + fileName + "\")" );
    }

    std::string line;

    while ( std::getline( fileStream, line ) )
    {
        getHardwareWhitelist()->insert( line );

        getLoggerInstance()->writeInfo( std::string( "Whitelisted Hardware-ID: \"" + line + "\"." ) );
    }

    fileStream.close();

    if ( isHardwareWhitelistEmpty() )
    {
        getLoggerInstance()->writeInfo( "Hardware Whitelist is empty, which means that not whitelist-checks will be performed." );
    }
}

SerialPortGateway::StringSet * SerialPortGateway::getHardwareWhitelist()
{
    return &hardwareWhitelist;
}

bool SerialPortGateway::isHardwareWhitelistEmpty()
{
    return getHardwareWhitelist()->empty();
}

bool SerialPortGateway::hasHardwareWhitelistEntry( std::string hardwareId )
{
    StringSet * hardwareWhitelist = getHardwareWhitelist();

    if ( hardwareWhitelist->find( hardwareId ) != hardwareWhitelist->end() )
    {
        return true;
    }

    return false;
}

void SerialPortGateway::loadSerialPortBlacklist()
{
    std::string fileName = getSerialPortBlacklistFile();

    if ( fileName.empty() )
    {
        getLoggerInstance()->writeInfo( "No Serial Port Blacklist given. (No ports will be ignored while scanning.)" );

        return;
    }

    std::ifstream fileStream( fileName, std::ifstream::out | std::ifstream::app );

    if ( !fileStream.is_open() )
    {
        throw Exception( "Couldn't open Serial Port Blacklist. (Path: \"" + fileName + "\")" );
    }

    std::string line;

    while ( std::getline( fileStream, line ) )
    {
        getSerialPortBlacklist()->insert( line );

        getLoggerInstance()->writeInfo( std::string( "Blacklisted Serial Port: \"" + line + "\"" ) );
    }

    fileStream.close();
}

SerialPortGateway::StringSet * SerialPortGateway::getSerialPortBlacklist()
{
    return &serialPortBlacklist;
}

bool SerialPortGateway::isSerialPortBlacklistEmpty()
{
    return getSerialPortBlacklist()->empty();
}

bool SerialPortGateway::hasSerialPortBlacklistEntry( std::string serialPort )
{
    StringSet * serialPortBlacklist = getSerialPortBlacklist();

    if ( serialPortBlacklist->find( serialPort ) != serialPortBlacklist->end() )
    {
        return true;
    }

    return false;
}

SerialPortGateway::SerialDeviceMap * SerialPortGateway::getSerialDevices()
{
    return &serialDevices;
}

SerialPortGateway::AtomicBoolPairMap * SerialPortGateway::getReadLoopStates()
{
    return &readLoopStates;
}

bool SerialPortGateway::addSerialDevice( std::string serialPort, bool suppressLogs )
{
    if ( !std::ifstream( serialPort ) )
    {
        getLoggerInstance()->writeWarn( std::string( "Couldn't add serial device on port \"" + serialPort + "\", because it doesn't exist or can not be accessed." ) );

        return false;
    }

    if ( hasSerialPortBlacklistEntry( serialPort ) )
    {
        if ( !suppressLogs )
        {
            getLoggerInstance()->writeWarn( std::string( "Didn't add serial device on port \"" + serialPort + "\", because the port is blacklisted." ) );
        }

        return false;
    }

    if ( SerialDevicePointer tempSerialDevice = getSerialDeviceByPort( serialPort ) )
    {
        if ( !suppressLogs )
        {
            getLoggerInstance()->writeWarn( std::string( "Couldn't add serial device on port \"" + serialPort + "\", because it has already been registered with ID \"" + tempSerialDevice->getId() + "\"." ) );
        }

        return false;
    }

    if ( !isHardwareWhitelistEmpty() )
    {
        // The sysfs information returns a string which contains the hardwareId
        // Inside this string, the hardwareId is formatted as followed (if it exists): "[...] VID:PID=<4 character long VID>:<4 character long PID> [...]"
        // Examples: "[...] USB VID:PID=1a86:7523 " or "[...] USB VID:PID=2341:0042 SNR=85438333935351F01180 [...]"
        // -> We only need the characters after the key ("VID:PID="), until the next blank space (" ")

        std::string hardwareId;

        // VERY VERY VEEEEERY ugly workaround for getting the hardwareId, until we have another RELIABLE solution for this (e.g. a new, better [proably self-written] serial library)
        // Unfortunately the serial libraries' "get_sysfs_info" function wasn't exposed, with which we could've done this directly. Without this ugly loop-workaround..
        for ( serial::PortInfo const & serialPortInfo : serial::list_ports() )
        {
            if ( serialPortInfo.port == serialPort )
            {
                hardwareId = serialPortInfo.hardware_id;

                break;
            }
        }

        std::string key = "VID:PID=";
        std::size_t keyPosition = hardwareId.find( key );
        std::size_t valuePosition = keyPosition + key.length();
        std::size_t hardwareIdLength = hardwareId.find_first_of( " ", valuePosition ) - valuePosition;

        if ( keyPosition != std::string::npos && hardwareIdLength != std::string::npos )
        {
            hardwareId = hardwareId.substr( valuePosition, hardwareIdLength );
        }
        else
        {
            getLoggerInstance()->writeWarn( std::string( "Couldn't add device on port \"" + serialPort + "\", because the Hardware ID could not be retrieved." ) );

            return false;
        }

        if ( !hasHardwareWhitelistEntry( hardwareId ) )
        {
            getLoggerInstance()->writeWarn( std::string( "Didn't add device on port \"" + serialPort + "\", because Hardware ID \"" + hardwareId + "\" is not whitelisted." ) );

            return false;
        }
    }

    SerialDevicePointer serialDevice = std::make_shared<SerialDevice>( serialPort, getBaudRate(), serial::Timeout::simpleTimeout( 250 ) );
    bool idRetrieved = false;

    try
    {
        serialDevice->init();
        std::this_thread::sleep_for( std::chrono::milliseconds( getWaitBeforeCommunication() ) );
        idRetrieved = retrieveDeviceId( serialDevice );
        serialDevice->getInstance()->flush();
    }
    catch( const serial::IOException & e )
    {
        getLoggerInstance()->writeError( std::string( "Couldn't add serial device on port \"" + serialPort + "\" due to an IOException: " + std::string( e.what() ) ) );

        return false;
    }
    catch( const serial::SerialException & e )
    {
        getLoggerInstance()->writeError( std::string( "Couldn't add serial device on port \"" + serialPort + "\" due to an SerialException: " + std::string( e.what() ) ) );

        return false;
    }
    catch ( const serial::PortNotOpenedException & e )
    {
        getLoggerInstance()->writeError( std::string( "Couldn't add serial device on port \"" + serialPort + "\" due to an PortNotOpenedException: " + std::string( e.what() ) ) );

        return false;
    }

    if ( !idRetrieved )
    {
        getLoggerInstance()->writeError( std::string( "Couldn't add serial device on port \"" + serialPort + "\", because the device didn't respond with a valid message containing the ID, or the ID was empty." ) );

        return false;
    }

    std::string deviceId = serialDevice->getId();
    SerialDeviceMap * serialDevices = getSerialDevices();
    SerialDeviceMap::iterator it = serialDevices->find( deviceId );

    if ( it == serialDevices->end() )
    {
        (*serialDevices)[deviceId] = serialDevice;

        getLoggerInstance()->writeInfo( std::string( "Added Serial Device with ID \"" + deviceId + "\" on port \"" + serialPort + "\"." ) );

        std::thread( &SerialPortGateway::serialDeviceAddedCallback, this, deviceId, serialPort ).detach();
        startReadLoop( deviceId );

        return true;
    }
    else
    {
        getLoggerInstance()->writeError( std::string( "Serial Device with ID \"" + deviceId + "\" already exists on port \"" + it->second->getPort() + "\". Can't add device with the same ID on port \"" + serialPort + "\"." ) );

        return false;
    }
}

unsigned int SerialPortGateway::addNewSerialPorts( bool suppressLogs )
{
    if ( !suppressLogs )
    {
        getLoggerInstance()->writeInfo( "Searching for new serial ports..." );
    }

    unsigned int numDevicesAdded = 0;
    std::vector<serial::PortInfo> serialPorts = serial::list_ports();

    for ( serial::PortInfo const & serialPortInfo : serialPorts )
    {
        // Set suppressLogs to true, so our logs don't get spammed with obvious "Errors" while using addNewSerialPorts repeatedly.
        // (This is to surpress messages about the currently iterated serialPort being blacklisted, and the current port already being added/registered.)
        if ( addSerialDevice( serialPortInfo.port, true ) )
        {
            numDevicesAdded++;
        }
    }

    if ( !suppressLogs )
    {
        getLoggerInstance()->writeInfo( "Finished searching for new serial ports. Added " + std::to_string( numDevicesAdded ) + " devices." );
    }

    return numDevicesAdded;
}

void SerialPortGateway::addNewSerialPortsLoop()
{
    unsigned int scanInterval = getScanInterval();

    while ( isStarted() )
    {
        addNewSerialPorts( true );

        if ( scanInterval > 0 )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( scanInterval ) );
        }
        else // If scanInterval is 0, quit the loop after the first iteration
        {
            break;
        }
    }
}

SerialPortGateway::SerialDevicePointer SerialPortGateway::getSerialDeviceById( std::string deviceId )
{
    SerialDeviceMap * serialDevices = getSerialDevices();
    SerialDeviceMap::iterator it = serialDevices->find( deviceId );
    SerialDevicePointer serialDevice = nullptr;

    if ( it != serialDevices->end() )
    {
        serialDevice = it->second;
    }

    return serialDevice;
}

SerialPortGateway::SerialDevicePointer SerialPortGateway::getSerialDeviceByPort( std::string serialPort )
{
    SerialDevicePointer serialDevice = nullptr;

    for ( std::pair<std::string, SerialDevicePointer> const & entry : *getSerialDevices() )
    {
        if ( entry.second->getPort() == serialPort )
        {
            serialDevice = entry.second;
        }
    }

    return serialDevice;
}

bool SerialPortGateway::deleteSerialDevice( std::string deviceId )
{
    SerialDeviceMap * serialDevices = getSerialDevices();
    SerialDeviceMap::iterator it = serialDevices->find( deviceId );
    std::string serialPort;
    bool properlyClosed;

    if ( it != serialDevices->end() )
    {
        stopReadLoop( deviceId );

        serialPort = it->second->getPort();
        SerialDevice::SerialInstance serialInstance = it->second->getInstance();

        if ( serialInstance != nullptr )
        {
            try
            {
                serialInstance->flush();
                serialInstance->close();

                properlyClosed = true;
            }
            catch( const serial::IOException & e )
            {
                getLoggerInstance()->writeError( std::string( "Could not properly delete Serial Device with ID \"" + deviceId + "\" on port \"" + serialPort + "\" due to an IOException: " + std::string( e.what() ) ) );

                properlyClosed = false;
            }
            catch ( const serial::PortNotOpenedException & e )
            {
                getLoggerInstance()->writeError( std::string( "Could not properly delete Serial Device with ID \"" + deviceId + "\" on port \"" + serialPort + "\" due to an PortNotOpenedException: " + std::string( e.what() ) ) );

                properlyClosed = false;
            }
        }
    }
    else
    {
        getLoggerInstance()->writeWarn( std::string( "Serial Device with ID \"" + deviceId + "\" was not found and could therefore not be deleted." ) );

        return false;
    }

    SerialDeviceMap::size_type numDeleted = serialDevices->erase( deviceId );

    if ( properlyClosed && numDeleted > 0 )
    {
        getLoggerInstance()->writeInfo( std::string( "Deleted Serial Device with ID \"" + deviceId + "\" on port \"" + serialPort + "\"." ) );

        std::thread( &SerialPortGateway::serialDeviceDeletedCallback, this, deviceId, serialPort ).detach();

        return true;
    }
    else
    {
        getLoggerInstance()->writeError( std::string( "Could not properly delete Serial Device with ID \"" + deviceId + "\" on port \"" + serialPort + "\"." ) );

        return false;
    }
}

unsigned int SerialPortGateway::deleteAllSerialDevices( bool suppressLogs )
{
    if ( !suppressLogs )
    {
        getLoggerInstance()->writeInfo( std::string( "Trying to delete all Serial Devices." ) );
    }

    unsigned int numDevicesDeleted = 0;

    for ( std::pair<std::string, SerialDevicePointer> const & entry : *getSerialDevices() )
    {
        if ( deleteSerialDevice( entry.first ) )
        {
            numDevicesDeleted++;
        }
    }

    if ( !suppressLogs )
    {
        getLoggerInstance()->writeInfo( "Finished deleting serial devices. Deleted " + std::to_string( numDevicesDeleted ) + " devices." );
    }

    return numDevicesDeleted;
}

void SerialPortGateway::readLoop( std::string deviceId )
{
    getLoggerInstance()->writeInfo( std::string( "Read loop started for Serial Device with ID \"" + deviceId + "\"." ) );

    setReadLoopQuitted( deviceId, false );

    while ( isReadLoopStarted( deviceId ) )
    {
        try
        {
            std::string line = getSerialDeviceById( deviceId )->getInstance()->readline();

            if ( !line.empty() )
            {
                std::thread( &SerialPortGateway::processMessage, this, deviceId, line ).detach();
            }
        }
        catch( const serial::SerialException & e )
        {
            getLoggerInstance()->writeError( std::string( "Serial Port Error: " + std::string( e.what() ) ) );
            getLoggerInstance()->writeInfo( std::string( "Deleting Serial Device with ID \"" + deviceId + "\" due to an read error." ) );

            deleteSerialDevice( deviceId );
        }
    }

    getLoggerInstance()->writeInfo( std::string( "Read loop stopped for Serial Device with ID \"" + deviceId + "\"." ) );

    setReadLoopQuitted( deviceId, true );
}

void SerialPortGateway::startReadLoop( std::string deviceId )
{
    setReadLoopStarted( deviceId, true );
    std::thread( &SerialPortGateway::readLoop, this, deviceId ).detach();
}

void SerialPortGateway::stopReadLoop( std::string deviceId )
{
    setReadLoopStarted( deviceId, false );
}

void SerialPortGateway::stopAllReadLoops()
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();
    AtomicBoolPairMap::iterator it;

    for ( it = readLoopStates->begin(); it != readLoopStates->end(); it++ )
    {
        stopReadLoop( it->first );
    }
}

void SerialPortGateway::setReadLoopStarted( std::string deviceId, bool started )
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();

    (*readLoopStates)[deviceId].first = started;
}

bool SerialPortGateway::isReadLoopStarted( std::string deviceId )
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();
    AtomicBoolPairMap::iterator it = readLoopStates->find( deviceId );

    if ( it != readLoopStates->end() )
    {
        return it->second.first;
    }

    return false;
}

void SerialPortGateway::setReadLoopQuitted( std::string deviceId, bool quitted )
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();

    (*readLoopStates)[deviceId].second = quitted;
}

bool SerialPortGateway::isReadLoopQuitted( std::string deviceId )
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();
    AtomicBoolPairMap::iterator it = readLoopStates->find( deviceId );

    if ( it != readLoopStates->end() )
    {
        return it->second.second;
    }

    return true;
}

bool SerialPortGateway::isEveryReadLoopQuitted()
{
    AtomicBoolPairMap * readLoopStates = getReadLoopStates();
    AtomicBoolPairMap::iterator it;

    for ( it = readLoopStates->begin(); it != readLoopStates->end(); it++ )
    {
        if ( isReadLoopQuitted( it->first ) )
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool SerialPortGateway::retrieveDeviceId( SerialDevicePointer serialDevice )
{
    std::string commandToGetDeviceId = getCommandToGetDeviceId() + CHAR_NEWLINE;

    SerialDevice::SerialInstance serialInstance = serialDevice->getInstance();
    serialInstance->flush();
    serialInstance->write( commandToGetDeviceId );
    //std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
    std::string message = serialInstance->readline();

    StringPair parsedMessage = parseMessage( message, getMessageDelimiter() );
    std::string type = parsedMessage.first;
    std::string deviceId = parsedMessage.second;

    if ( type == getMessageTypeForIds() && !deviceId.empty() )
    {
        serialDevice->setId( deviceId );

        return true;
    }
    else
    {
        return false;
    }
}

SerialPortGateway::StringPair SerialPortGateway::parseMessage( std::string message, std::string delimiter )
{
    std::size_t delimiterPos = message.find_first_of( delimiter );
    std::size_t messageEnd = message.find_first_of( CHAR_NEWLINE + CHAR_CARRIAGE_RETURN );
    std::string type = "";
    std::string content = "";

    // Parses the message only when there is a correct ending and a delimiter.
    // Allows to send messages with empty types, as long as there's a delimiter AND content after that
    if ( messageEnd != std::string::npos && delimiterPos != std::string::npos )
    {
        type = message.substr( 0, delimiterPos );

        if (  delimiterPos < messageEnd )
        {
            messageEnd--; // Exclude the newline character
            content = message.substr( delimiterPos + 1, messageEnd - delimiterPos );
        }
    }

    return std::make_pair( type, content );
}

void SerialPortGateway::processMessage( std::string deviceId, std::string message )
{
    StringPair parsedMessage = parseMessage( message, getMessageDelimiter() );
    std::string type = parsedMessage.first;
    std::string content = parsedMessage.second;
    SerialMessage serialMessage = SerialMessage( deviceId, type, content );

    std::thread( &SerialPortGateway::messageCallback, this, serialMessage ).detach();
}

void SerialPortGateway::sendMessageToSerialDeviceAsync( std::string deviceId, std::string message )
{
    try
    {
        SerialDevicePointer device = getSerialDeviceById( deviceId );

        if ( device != nullptr )
        {
            SerialDevice::SerialInstance serialInstance = device->getInstance();
            std::size_t bytesWritten = serialInstance->write( message + CHAR_NEWLINE ); // Append a newline character to mark the end of the message

            if ( bytesWritten == message.length() + 1 ) // + 1 for the appended newline character
            {
                getLoggerInstance()->writeInfo( std::string( "Delivered message \"" + message + "\" to device with ID \"" + deviceId + "\" (Bytes written: " + std::to_string( bytesWritten ) + "/" + std::to_string( message.length() + 1 ) + ")." ) );
            }
            else
            {
                getLoggerInstance()->writeError( std::string( "Could not deliver message \"" + message + "\" properly to device with ID \"" + deviceId + "\" (Bytes written: " + std::to_string( bytesWritten ) + "/" + std::to_string( message.length() + 1 ) + ")." ) );
            }
        }
        else
        {
            getLoggerInstance()->writeInfo( std::string( "Device with ID \"" + deviceId + "\" not found. Message \"" + message + "\" can not be delivered." ) );
        }
    }
    catch( const serial::SerialException & e )
    {
        getLoggerInstance()->writeError( std::string( "Serial Port Error: " + std::string( e.what() ) ) );
        getLoggerInstance()->writeInfo( std::string( "Deleting Serial Device with ID \"" + deviceId + "\" due to an write error." ) );

        deleteSerialDevice( deviceId );
    }
}

void SerialPortGateway::start()
{
    if ( isStarted() )
    {
        getLoggerInstance()->writeWarn( "SerialPortGateway already started, not starting again." );

        return;
    }

    getLoggerInstance()->writeInfo( "Starting SerialPortGateway." );

    setStarted( true );

    std::thread( &SerialPortGateway::addNewSerialPortsLoop, this ).detach();
}

void SerialPortGateway::stop()
{
    if ( !isStarted() )
    {
        getLoggerInstance()->writeInfo( "SerialPortGateway not started. Nothing to be stopped." );

        return;
    }

    getLoggerInstance()->writeInfo( "Stopping SerialPortGateway." );

    setStarted( false );
    deleteAllSerialDevices();
}

void SerialPortGateway::setStarted( bool started )
{
    this->started = started;
}

bool SerialPortGateway::isStarted()
{
    return this->started;
}

std::vector<std::string> SerialPortGateway::getDeviceIds()
{
    std::vector<std::string> deviceIds;

    for ( std::pair<std::string, SerialDevicePointer> const & entry : *getSerialDevices() )
    {
        deviceIds.insert( deviceIds.begin(), entry.first );
    }

    return deviceIds;
}

std::string SerialPortGateway::getDeviceIdList()
{
    std::stringstream deviceIdList;

    for ( std::string deviceId : getDeviceIds() )
    {
        deviceIdList << deviceId << LIST_SEPARATOR;
    }

    std::string result = deviceIdList.str();

    if ( !result.empty() )
    {
        std::size_t separatorLength = LIST_SEPARATOR.length();
        result.erase( result.length() - separatorLength, separatorLength ); // Remove the last separator
    }

    return result;
}

std::vector<std::string> SerialPortGateway::getSerialPorts()
{
    std::vector<std::string> serialPorts;

    for ( serial::PortInfo const & serialPortInfo : serial::list_ports() )
    {
        serialPorts.insert( serialPorts.begin(), serialPortInfo.port );
    }

    return serialPorts;
}

std::string SerialPortGateway::getSerialPortList()
{
    std::stringstream serialPortsList;

    for ( std::string serialPort : getSerialPorts() )
    {
        serialPortsList << serialPort << LIST_SEPARATOR;
    }

    std::string result = serialPortsList.str();

    if ( !result.empty() )
    {
        std::size_t separatorLength = LIST_SEPARATOR.length();
        result.erase( result.length() - separatorLength, separatorLength ); // Remove the last separator
    }

    return result;
}

std::map<std::string, std::string> SerialPortGateway::getDeviceIdToSerialPortMappings()
{
    std::map<std::string, std::string> deviceIdToSerialPortMappings;

    for ( std::pair<std::string, SerialDevicePointer> const & entry : *getSerialDevices() )
    {
        deviceIdToSerialPortMappings[entry.first] = entry.second->getPort();
    }

    return deviceIdToSerialPortMappings;
}

std::string SerialPortGateway::getDeviceIdToSerialPortMappingList()
{
    std::stringstream deviceIdToSerialPortMappingList;

    for ( std::pair<std::string, std::string> deviceIdToSerialPortMapping : getDeviceIdToSerialPortMappings() )
    {
        deviceIdToSerialPortMappingList << deviceIdToSerialPortMapping.first << " -> " << deviceIdToSerialPortMapping.second << LIST_SEPARATOR;
    }

    std::string result = deviceIdToSerialPortMappingList.str();

    if ( !result.empty() )
    {
        std::size_t separatorLength = LIST_SEPARATOR.length();
        result.erase( result.length() - separatorLength, separatorLength ); // Remove the last separator
    }

    return result;
}

void SerialPortGateway::sendMessageToSerialDevice( std::string deviceId, std::string message )
{
    std::thread( &SerialPortGateway::sendMessageToSerialDeviceAsync, this, deviceId, message ).detach();
}

void SerialPortGateway::broadcastMessageToSerialDevices( std::string message )
{
    for ( std::string deviceId : getDeviceIds() )
    {
        std::thread( &SerialPortGateway::sendMessageToSerialDeviceAsync, this, deviceId, message ).detach();
    }
}

void SerialPortGateway::serialDeviceAddedCallback( std::string deviceId, std::string serialPort )
{

}

void SerialPortGateway::serialDeviceDeletedCallback( std::string deviceId, std::string serialPort )
{

}

void SerialPortGateway::messageCallback( SerialMessage serialMessage )
{
    std::stringstream message;
    message
        << "New message from \"" << serialMessage.getDeviceId()
        << "\": timestamp=\"" << serialMessage.getTimestamp()
        << "\", type=\"" << serialMessage.getType()
        << "\", content=\"" << serialMessage.getContent()
        << "\".";

    getLoggerInstance()->writeInfo( message.str() );
}
