################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Compress.cpp \
../FileDataManager.cpp \
../FileManager.cpp \
../GetIPAddress.cpp \
../MySqlManager.cpp \
../SocketManager.cpp \
../TCPManager.cpp \
../UDPManager.cpp \
../main.cpp 

OBJS += \
./Compress.o \
./FileDataManager.o \
./FileManager.o \
./GetIPAddress.o \
./MySqlManager.o \
./SocketManager.o \
./TCPManager.o \
./UDPManager.o \
./main.o 

CPP_DEPS += \
./Compress.d \
./FileDataManager.d \
./FileManager.d \
./GetIPAddress.d \
./MySqlManager.d \
./SocketManager.d \
./TCPManager.d \
./UDPManager.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


