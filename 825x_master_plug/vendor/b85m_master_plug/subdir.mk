################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/b85m_master_plug/app.c \
../vendor/b85m_master_plug/app_audio.c \
../vendor/b85m_master_plug/blm_att.c \
../vendor/b85m_master_plug/blm_host.c \
../vendor/b85m_master_plug/blm_ota.c \
../vendor/b85m_master_plug/blm_pair.c \
../vendor/b85m_master_plug/button.c \
../vendor/b85m_master_plug/main.c 

OBJS += \
./vendor/b85m_master_plug/app.o \
./vendor/b85m_master_plug/app_audio.o \
./vendor/b85m_master_plug/blm_att.o \
./vendor/b85m_master_plug/blm_host.o \
./vendor/b85m_master_plug/blm_ota.o \
./vendor/b85m_master_plug/blm_pair.o \
./vendor/b85m_master_plug/button.o \
./vendor/b85m_master_plug/main.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/b85m_master_plug/%.o: ../vendor/b85m_master_plug/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk" -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk\drivers\8258" -D__PROJECT_8258_MASTER_PLUG__=1 -DCHIP_TYPE=CHIP_TYPE_825x -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


