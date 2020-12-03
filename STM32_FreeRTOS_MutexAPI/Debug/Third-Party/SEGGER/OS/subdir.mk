################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third-Party/SEGGER/OS/SEGGER_SYSVIEW_FreeRTOS.c 

OBJS += \
./Third-Party/SEGGER/OS/SEGGER_SYSVIEW_FreeRTOS.o 

C_DEPS += \
./Third-Party/SEGGER/OS/SEGGER_SYSVIEW_FreeRTOS.d 


# Each subdirectory must supply rules for building sources it contributes
Third-Party/SEGGER/OS/%.o: ../Third-Party/SEGGER/OS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DDEBUG -DSTM32F446xx -DUSE_STDPERIPH_DRIVER -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/StdPeriph_Driver/inc" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Third-Party/SEGGER/SEGGER" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Third-Party/SEGGER/OS" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Third-Party/SEGGER/Config" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Third-Party/FreeRTOS/org/Source/include" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/Config" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/inc" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/CMSIS/device" -I"/home/py/workspace/RTOS_workspace/STM32_FreeRTOS_MutexAPI/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


