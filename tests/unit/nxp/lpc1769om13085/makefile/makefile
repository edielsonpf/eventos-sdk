-include ../makefile.init

############################
# Command for removing files
RM := rm -rf

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

############################
# Compiler
CC := arm-none-eabi-gcc

############################
# Linker
LL := arm-none-eabi-gcc

############################
# Binary/exectable to build
EXE := EventOS_unit_tests.axf
 
############################
# Directory definition
PROJ_BASE_DIR = ../../../../..
TEST_BASE_DIR = $(PROJ_BASE_DIR)/tests/unit
LIB_BASE_DIR = $(PROJ_BASE_DIR)/lib
DRIVERS_BASE_DIR = $(PROJ_BASE_DIR)/drivers
THIRD_PARTY_LIB_BASE_DIR = $(LIB_BASE_DIR)/third_party

############################
# Include directory definition
TEST_INCLUDE_DIRS += -I $(TEST_BASE_DIR)/common/include
TEST_INCLUDE_DIRS += -I $(TEST_BASE_DIR)/nxp/lpc1769om13085/common/config_files
TEST_INCLUDE_DIRS += -I $(TEST_BASE_DIR)/nxp/lpc1769om13085/common/application_code

LIB_INCLUDE_DIRS += -I $(LIB_BASE_DIR)/include 
LIB_INCLUDE_DIRS += -I $(LIB_BASE_DIR)/include/private
LIB_INCLUDE_DIRS += -I $(LIB_BASE_DIR)/EventOS/portable/device/NXP/LPC17xx/

#DRIVERS_INCLUDE_DIRS += -I $(DRIVERS_BASE_DIR)/LPC17xx_HAL_Drivers/inc 
DRIVERS_INCLUDE_DIRS += -I $(DRIVERS_BASE_DIR)/cmsis/include
DRIVERS_INCLUDE_DIRS += -I $(DRIVERS_BASE_DIR)/cmsis/device/NXP/LPC17xx/include 

THIRD_PARTY_LIB_INCLUDE_DIRS += -I $(THIRD_PARTY_LIB_BASE_DIR)/unity/src 
THIRD_PARTY_LIB_INCLUDE_DIRS += -I $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src 

#Aggregate all include directories
INCLUDE_ALL_DIRS += $(TEST_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(LIB_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(DRIVERS_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(THIRD_PARTY_LIB_INCLUDE_DIRS)

############################
#Source files definition definition
TEST_SRC_FILES += $(shell find $(TEST_BASE_DIR)/common/source/ -name '*.c')
TEST_SRC_FILES += $(shell find $(TEST_BASE_DIR)/nxp/lpc1769om13085/common/application_code/ -name '*.c')

LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/EventOS/ -name '*.c')
LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/EventOS/portable/device/NXP/LPC17xx/ -name '*.c')
LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/log/ -name '*.c')

#DRIVERS_SRC_FILES += $(shell find $(DRIVERS_BASE_DIR)/LPC17xx_HAL_Drivers/src -name '*.c')
DRIVERS_SRC_FILES += $(shell find $(DRIVERS_BASE_DIR)/cmsis/source/ -name '*.c')
DRIVERS_SRC_FILES += $(shell find $(DRIVERS_BASE_DIR)/cmsis/device/NXP/LPC17xx/source/ -name '*.c')

THIRD_PARTY_SRC_FILES += $(shell find $(THIRD_PARTY_LIB_BASE_DIR)/unity/src/ -name '*.c')
THIRD_PARTY_SRC_FILES += $(shell find $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src/ -name '*.c')

#Aggregate all include and src directories
C_SRCS += $(TEST_SRC_FILES)
C_SRCS += $(LIB_SRC_FILES)
C_SRCS += $(DRIVERS_SRC_FILES)
C_SRCS += $(THIRD_PARTY_SRC_FILES)

OBJS += \
	main.o \
	board.o \
	core_cm3.o \
	system_LPC17xx.o \
	cr_startup_lpc17.o \
	eventos_test_event.o \
	eventos_test_framework.o \
	eventos_test_runner.o \
	event.o \
	list.o \
	port.o \
	eventos_log.o \
	unity.o \
	unity_fixture.o 
	
C_DEPS = $(OBJS:.o=.d)

-include ../makefile.defs

############################
# Compiler options
CC_OPTIONS := -D__REDLIB__ -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -DLPCXPRESSO_LPC17xx -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs $(INCLUDE_ALL_DIRS)

############################
# Linker Options
LL_OPTIONS := -nostdlib -Xlinker -Map="linker/EventOS.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "linker/EventOS.ld" -o $(EXE)


############################
# Main (all) target
# All Target

all: $(EXE)
	@echo "*** finished building ***"
	
# Tool invocations
$(EXE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	$(CC) $(LL_OPTIONS) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) $(EXE)
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size $(EXE); # arm-none-eabi-objcopy -v -O binary $(EXE) "EventOS.bin" ; # checksum -p Generic_M3 -d "EventOS.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

############################
# Rule to build the files in the source folder
%.o: $(TEST_BASE_DIR)/nxp/lpc1769om13085/common/application_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
############################
# Rule to build the files in the source folder
%.o: $(TEST_BASE_DIR)/common/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/EventOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/EventOS/portable/device/NXP/LPC17xx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	
	
############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/log/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(DRIVERS_BASE_DIR)/LPC17xx_HAL_Drivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(DRIVERS_BASE_DIR)/cmsis/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	
	
############################
# Rule to build the files in the lib folder
%.o: $(DRIVERS_BASE_DIR)/cmsis/device/NXP/LPC17xx/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(THIRD_PARTY_LIB_BASE_DIR)/unity/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CC_OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	


	
-include ../makefile.targets
