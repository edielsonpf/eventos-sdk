TARGET_EXEC ?= EventOS_unit_tests.out

############################
# Directory definition
PROJ_BASE_DIR = ../../../..
TEST_BASE_DIR = $(PROJ_BASE_DIR)/tests/unit
LIB_BASE_DIR = $(PROJ_BASE_DIR)/lib
DRIVERS_BASE_DIR = $(PROJ_BASE_DIR)/drivers
THIRD_PARTY_LIB_BASE_DIR = $(LIB_BASE_DIR)/third_party

############################
#Source files definition definition
TEST_SRC_FILES += $(shell find $(TEST_BASE_DIR)/common/source/ -name '*.c')
TEST_SRC_FILES += $(shell find $(TEST_BASE_DIR)/pc/linux/common/application_code/ -name '*.c')

LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/EventOS/ -name '*.c')
LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/EventOS/portable/GCC/linux_simulator/ -name '*.c')
LIB_SRC_FILES += $(shell find $(LIB_BASE_DIR)/log/ -name '*.c')

THIRD_PARTY_SRC_FILES += $(shell find $(THIRD_PARTY_LIB_BASE_DIR)/unity/src/ -name '*.c')
THIRD_PARTY_SRC_FILES += $(shell find $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src/ -name '*.c')

#Aggregate all include and src directories
SRCS += $(TEST_SRC_FILES)
SRCS += $(LIB_SRC_FILES)
SRCS += $(DRIVERS_SRC_FILES)
SRCS += $(THIRD_PARTY_SRC_FILES)


OBJS += \
	main.o \
	eventos_test_event.o \
	eventos_test_framework.o \
	eventos_test_runner.o \
	event.o \
	list.o \
	port.o \
	eventos_log.o \
	unity.o \
	unity_fixture.o 

#OBJS := $(addsuffix .o,$(basename $(SRCS)))

DEPS := $(OBJS:.o=.d)



############################
# Include directory definition
TEST_INCLUDE_DIRS += $(TEST_BASE_DIR)/common/include
TEST_INCLUDE_DIRS += $(TEST_BASE_DIR)/pc/linux/common/config_files
TEST_INCLUDE_DIRS += $(TEST_BASE_DIR)/pc/linux/common/application_code

LIB_INCLUDE_DIRS += $(LIB_BASE_DIR)/include 
LIB_INCLUDE_DIRS += $(LIB_BASE_DIR)/include/private
LIB_INCLUDE_DIRS += $(LIB_BASE_DIR)/EventOS/portable/GCC/linux_simulator/

DRIVERS_INCLUDE_DIRS += $(DRIVERS_BASE_DIR)/cmsis/include
DRIVERS_INCLUDE_DIRS += $(DRIVERS_BASE_DIR)/cmsis/device/NXP/LPC17xx/include 

THIRD_PARTY_LIB_INCLUDE_DIRS += -I $(THIRD_PARTY_LIB_BASE_DIR)/unity/src 
THIRD_PARTY_LIB_INCLUDE_DIRS += -I $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src 

#Aggregate all include directories
INC_DIRS += $(TEST_INCLUDE_DIRS)
INC_DIRS += $(LIB_INCLUDE_DIRS)
INC_DIRS += $(DRIVERS_INCLUDE_DIRS)
INC_DIRS += $(THIRD_PARTY_LIB_INCLUDE_DIRS)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(TARGET_EXEC): $(OBJS)
	@echo 'Building target: $@'
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
############################
# Rule to build the files in the source folder
%.o: $(TEST_BASE_DIR)/pc/linux/common/application_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '
	
############################
# Rule to build the files in the source folder
%.o: $(TEST_BASE_DIR)/common/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '


############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/EventOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '

############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/EventOS/portable/GCC/linux_simulator/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '	
	
############################
# Rule to build the files in the lib folder
%.o: $(LIB_BASE_DIR)/log/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(THIRD_PARTY_LIB_BASE_DIR)/unity/extras/fixture/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '	

############################
# Rule to build the files in the lib folder
%.o: $(THIRD_PARTY_LIB_BASE_DIR)/unity/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '	



.PHONY: clean

clean:
	$(RM) $(TARGET_EXEC) $(OBJS) $(DEPS) $(EXE)
	-@echo ' '

-include $(DEPS)

MKDIR_P ?= mkdir -p

