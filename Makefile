VERSION                     =       1.0
DOCKER_IMAGE_NAME           =       serialportgateway
DOCKER_IMAGE_TAG            =       latest
CXX                         =       /usr/bin/g++-6
CFLAGS                      =       -std=c++1y -Wall
LIBS                        =       -L/tmp/usr/local/lib -lpthread -lserial
INCLUDES                    =       -I/repos/serial/include/
SRC_DIR                     =       ./src
LOCAL_DEPENDENCIES_DIR      =       ./dependencies
OBJS                        =       $(LOCAL_DEPENDENCIES_DIR)/Logger/src/Logger.o \
                                    $(LOCAL_DEPENDENCIES_DIR)/Logger/src/LogType.o \
                                    $(LOCAL_DEPENDENCIES_DIR)/Exception/src/Exception.o \
                                    $(LOCAL_DEPENDENCIES_DIR)/Config/src/ConfigExceptions.o \
                                    $(LOCAL_DEPENDENCIES_DIR)/Config/src/Config.o \
                                    $(SRC_DIR)/SerialDevice.o \
                                    $(SRC_DIR)/SerialMessage.o \
                                    $(SRC_DIR)/SerialPortGateway.o
SRC_NAME_MAIN               =       serial2console-gateway.cpp
BIN_DIR                     =       ./bin
BIN_NAME                    =       serial2console-gateway

.PHONY: all
all: makeDirs buildMsg build

.PHONY: makeDirs
makeDirs:
	@if [ ! -d $(BIN_DIR) ]; then \
		mkdir $(BIN_DIR); \
	fi

.PHONY: buildMsg
buildMsg:
	@echo "\e[92m---- Building \"$(BIN_NAME)\"...\e[0m"

build: $(OBJS)
	@$(CXX) $(CFLAGS) -o $(BIN_DIR)/$(BIN_NAME) $(SRC_DIR)/$(SRC_NAME_MAIN) $(LIBS) $(INCLUDES) $(OBJS)
	@echo "\e[92m---- DONE.\e[0m"

%.o: %.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@ $(LIBS) $(INCLUDES)

.PHONY: buildDockerImage
buildDockerImage:
	@echo "\e[92m--- Building Docker-Image $(DOCKER_IMAGE_NAME)\e[0m"
	docker build -t $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_TAG) ./

.PHONY: clean
clean:
	@echo "\e[92m---- Cleaning up...\e[0m"
	@rm -r $(OBJS)
	@echo "\e[92m---- DONE.\e[0m"
