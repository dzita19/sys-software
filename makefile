ASSEMBLER  = assembler
LINKER     = linker
EMULATOR   = emulator
SPEC_DIR   = spec
BUILD_DIR  = build

DEBUG_ENABLED = 1

FLEX_SPEC  = lexer_spec.l
BISON_SPEC = parser_spec.y

FLEX_OUTFILE  = lex.yy.c
BISON_OUTFILE = y.tab.c
BISON_SYMBOLS = y.tab.h

YY_SOURCE_LIST = \
	src/yy/$(FLEX_OUTFILE) \
	src/yy/$(BISON_OUTFILE)

UTIL_SOURCE_LIST = \
	src/util/array.c \
	src/util/byte_vector.c \
	src/util/logger_asm.c \
	src/util/linked_list.c \
	src/util/memory_safety.c \
	src/util/queue.c \
	src/util/stack.c \
	src/util/vector.c

OBJ_DATA_SOURCE_LIST = \
	src/obj_data/symtab.c \
	src/obj_data/section.c \
	src/obj_data/relatab.c \
	src/obj_data/obj_data.c

MACHINE_SOURCE_LIST = \
	src/machine/architecture.c

ASSEMBLER_SOURCE_LIST = \
	src/assembler/assembler_main.c \
	src/assembler/assembler.c \
	src/assembler/code_gen.c \
	src/assembler/directive.c \
	src/assembler/expression.c \
	src/assembler/instruction.c \
	src/assembler/literals.c

LINKER_SOURCE_LIST = \
	src/linker/linker_main.c \
	src/linker/global_def.c \
	src/linker/linker.c \
	src/linker/obj_load.c

EMULATOR_SOURCE_LIST = \
	src/emulator/emulator.c \
	src/emulator/emulator_main.c

C_SOURCE_LIST  =
C_SOURCE_LIST += $(YY_SOURCE_LIST)
C_SOURCE_LIST += $(UTIL_SOURCE_LIST)
C_SOURCE_LIST += $(MACHINE_SOURCE_LIST)
C_SOURCE_LIST += $(OBJ_DATA_SOURCE_LIST)
C_SOURCE_LIST += $(ASSEMBLER_SOURCE_LIST)
C_SOURCE_LIST += $(LINKER_SOURCE_LIST)
C_SOURCE_LIST += $(EMULATOR_SOURCE_LIST)

C_OBJECT_LIST       = 
C_OBJECT_LIST      += $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCE_LIST:.c=.o)))

ASSEMBLER_OBJ_LIST  = 
ASSEMBLER_OBJ_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(YY_SOURCE_LIST:.c=.o)))
ASSEMBLER_OBJ_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(UTIL_SOURCE_LIST:.c=.o)))
ASSEMBLER_OBJ_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(OBJ_DATA_SOURCE_LIST:.c=.o)))
ASSEMBLER_OBJ_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(MACHINE_SOURCE_LIST:.c=.o)))
ASSEMBLER_OBJ_LIST += $(addprefix $(BUILD_DIR)/, $(notdir $(ASSEMBLER_SOURCE_LIST:.c=.o)))

LINKER_OBJ_LIST     = 
LINKER_OBJ_LIST    += $(addprefix $(BUILD_DIR)/, $(notdir $(UTIL_SOURCE_LIST:.c=.o)))
LINKER_OBJ_LIST    += $(addprefix $(BUILD_DIR)/, $(notdir $(OBJ_DATA_SOURCE_LIST:.c=.o)))
LINKER_OBJ_LIST    += $(addprefix $(BUILD_DIR)/, $(notdir $(LINKER_SOURCE_LIST:.c=.o)))

EMULATOR_OBJ_LIST   =
EMULATOR_OBJ_LIST  += $(addprefix $(BUILD_DIR)/, $(notdir $(UTIL_SOURCE_LIST:.c=.o)))
EMULATOR_OBJ_LIST  += $(addprefix $(BUILD_DIR)/, $(notdir $(MACHINE_SOURCE_LIST:.c=.o)))
EMULATOR_OBJ_LIST  += $(addprefix $(BUILD_DIR)/, $(notdir $(EMULATOR_SOURCE_LIST:.c=.o)))

vpath %.c $(sort $(dir $(C_SOURCE_LIST)))

CC_PATH     = gcc
FLEX_PATH   = flex
BISON_PATH  = bison

INCLUDE     = -I src
WARNINGS    = -Wall
FLAGS_DEBUG = -g
FLAGS_DEPS  = -MMD -MP

FLAGS  =
FLAGS += $(INCLUDE)
FLAGS += $(FLAGS_DEPS)
FLAGS += $(WARNINGS)

ifeq ($(DEBUG_ENABLED), 1)
FLAGS += $(FLAGS_DEBUG)
endif

all: src/yy/$(BISON_OUTFILE) src/yy/$(FLEX_OUTFILE) $(BUILD_DIR)/$(ASSEMBLER) $(BUILD_DIR)/$(LINKER) $(BUILD_DIR)/$(EMULATOR)

src/yy/$(BISON_OUTFILE): $(SPEC_DIR)/$(BISON_SPEC) makefile
	$(BISON_PATH) -d -v -o src/yy/$(BISON_OUTFILE) $(SPEC_DIR)/$(BISON_SPEC)

src/yy/$(FLEX_OUTFILE): $(SPEC_DIR)/$(FLEX_SPEC) src/yy/$(BISON_SYMBOLS) makefile
	$(FLEX_PATH) --outfile=src/yy/$(FLEX_OUTFILE) $(SPEC_DIR)/$(FLEX_SPEC)

clean:
	rm -rf build

$(BUILD_DIR)/$(ASSEMBLER) : $(ASSEMBLER_OBJ_LIST) makefile | $(BUILD_DIR)
	$(CC_PATH) -o $(@) $(ASSEMBLER_OBJ_LIST)

$(BUILD_DIR)/$(LINKER) : $(LINKER_OBJ_LIST) makefile | $(BUILD_DIR)
	$(CC_PATH) -o $(@) $(LINKER_OBJ_LIST)

$(BUILD_DIR)/$(EMULATOR) : $(EMULATOR_OBJ_LIST) makefile | $(BUILD_DIR)
	$(CC_PATH) -o $(@) $(EMULATOR_OBJ_LIST)

$(BUILD_DIR)/%.o : %.c makefile | $(BUILD_DIR)
	$(CC_PATH) -c -o $(@) $(<) $(FLAGS)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)