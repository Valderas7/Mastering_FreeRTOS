################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_Idle_Hook_Power_Saving/StdPeriph_Driver/inc" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_Idle_Hook_Power_Saving/inc" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_Idle_Hook_Power_Saving/CMSIS/device" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_Idle_Hook_Power_Saving/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


