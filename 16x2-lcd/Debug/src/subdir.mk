################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gpio.c \
../src/hd44780.c \
../src/main.c 

C_DEPS += \
./src/gpio.d \
./src/hd44780.d \
./src/main.d 

OBJS += \
./src/gpio.o \
./src/hd44780.o \
./src/main.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/gpio.d ./src/gpio.o ./src/hd44780.d ./src/hd44780.o ./src/main.d ./src/main.o

.PHONY: clean-src

