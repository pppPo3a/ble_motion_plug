################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/b85m_ble_pir/advdata_mac.c \
../vendor/b85m_ble_pir/app.c \
../vendor/b85m_ble_pir/app_att.c \
../vendor/b85m_ble_pir/app_button.c \
../vendor/b85m_ble_pir/batt_read.c \
../vendor/b85m_ble_pir/button.c \
../vendor/b85m_ble_pir/main.c \
../vendor/b85m_ble_pir/sensor.c 

OBJS += \
./vendor/b85m_ble_pir/advdata_mac.o \
./vendor/b85m_ble_pir/app.o \
./vendor/b85m_ble_pir/app_att.o \
./vendor/b85m_ble_pir/app_button.o \
./vendor/b85m_ble_pir/batt_read.o \
./vendor/b85m_ble_pir/button.o \
./vendor/b85m_ble_pir/main.o \
./vendor/b85m_ble_pir/sensor.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/b85m_ble_pir/%.o: ../vendor/b85m_ble_pir/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk" -I"E:\pir\B85M_SINGLE_BLE_SDK\b85_ble_sdk\drivers\8258" -D__PROJECT_8258_BLE_PIR__=1 -DCHIP_TYPE=CHIP_TYPE_825x -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


