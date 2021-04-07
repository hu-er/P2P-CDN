################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Compress.cpp \
../FileDataReceiver.cpp \
../FileManager.cpp \
../MySqlManager.cpp \
../ReSent.cpp \
../TCPManager.cpp \
../UDPManager.cpp \
../GetIPAddress.cpp \
../main.cpp 

OBJS += \
./Compress.o \
./FileDataReceiver.o \
./FileManager.o \
./MySqlManager.o \
./ReSent.o \
./TCPManager.o \
./UDPManager.o \
./GetIPAddress.o \
./main.o 

CPP_DEPS += \
./Compress.d \
./FileDataReceiver.d \
./FileManager.d \
./MySqlManager.d \
./ReSent.d \
./TCPManager.d \
./UDPManager.d \
./GetIPAddress.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include -O2 -g -lpthread -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


