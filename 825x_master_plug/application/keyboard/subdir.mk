################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application/keyboard/keyboard.c 

OBJS += \
./application/keyboard/keyboard.o 


# Each subdirectory must supply rules for building sources it contributes
application/keyboard/%.o: ../application/keyboard/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk" -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk\drivers\8258" -D__PROJECT_8258_MASTER_PLUG__=1 -DCHIP_TYPE=CHIP_TYPE_825x -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


