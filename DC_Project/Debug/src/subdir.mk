################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/buttons.c \
../src/helloworld.c \
../src/leds.c \
../src/lock.c \
../src/mode.c \
../src/model.c \
../src/params.c \
../src/pi.c \
../src/platform.c \
../src/uart_cmd.c 

OBJS += \
./src/buttons.o \
./src/helloworld.o \
./src/leds.o \
./src/lock.o \
./src/mode.o \
./src/model.o \
./src/params.o \
./src/pi.o \
./src/platform.o \
./src/uart_cmd.o 

C_DEPS += \
./src/buttons.d \
./src/helloworld.d \
./src/leds.d \
./src/lock.d \
./src/mode.d \
./src/model.d \
./src/params.d \
./src/pi.d \
./src/platform.d \
./src/uart_cmd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../DC_Project_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


