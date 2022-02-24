################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application/audio/adpcm.c \
../application/audio/tl_audio.c 

OBJS += \
./application/audio/adpcm.o \
./application/audio/tl_audio.o 


# Each subdirectory must supply rules for building sources it contributes
application/audio/%.o: ../application/audio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk" -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk\drivers\8258" -D__PROJECT_8258_MASTER_PLUG__=1 -DCHIP_TYPE=CHIP_TYPE_825x -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


