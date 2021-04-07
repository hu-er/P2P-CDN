################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MySqlManager.cpp \
../TCPManager.cpp \
../main.cpp 

OBJS += \
./MySqlManager.o \
./TCPManager.o \
./main.o 

CPP_DEPS += \
./MySqlManager.d \
./TCPManager.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/x86_64-linux-gnu/ -I/usr/lib64/mysql -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


