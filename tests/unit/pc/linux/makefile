TARGET_EXEC ?= EventOS_unit_tests.out

BUILD_DIR ?= ./build

############################
# Directory definition
PROJ_BASE_DIR = ../../../../..
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

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

all: $(EXE)
	@echo "*** finished building ***"

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo 'Building target: $@'
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
    @echo 'Building file: $<'
	$(MKDIR_P) $(dir $@)
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo 'Finished building: $<'
	@echo ' '
	
.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

