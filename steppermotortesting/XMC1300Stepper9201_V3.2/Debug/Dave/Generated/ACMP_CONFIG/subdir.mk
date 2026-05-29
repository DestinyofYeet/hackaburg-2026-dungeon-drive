################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dave/Generated/ACMP_CONFIG/acmp_config.c \
../Dave/Generated/ACMP_CONFIG/acmp_config_conf.c 

OBJS += \
./Dave/Generated/ACMP_CONFIG/acmp_config.o \
./Dave/Generated/ACMP_CONFIG/acmp_config_conf.o 

C_DEPS += \
./Dave/Generated/ACMP_CONFIG/acmp_config.d \
./Dave/Generated/ACMP_CONFIG/acmp_config_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Dave/Generated/ACMP_CONFIG/%.o: ../Dave/Generated/ACMP_CONFIG/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC1302_T038x0200 -I"$(PROJECT_LOC)/Libraries/XMCLib/inc" -I"$(PROJECT_LOC)/ProbeScope" -I"$(PROJECT_LOC)/Libraries/CMSIS/Include" -I"$(PROJECT_LOC)/Libraries/CMSIS/Infineon/XMC1300_series/Include" -I"$(PROJECT_LOC)" -I"$(PROJECT_LOC)/Dave/Generated" -I"$(PROJECT_LOC)/Libraries" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m0 -mthumb -g -gdwarf-2 -o "$@" "$<" 
	@echo 'Finished building: $<'
	@echo.

