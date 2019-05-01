CC = gcc
RMDIR = rm -rf
RM = rm -f

RUN = ./

DEP_FLAGS = -MT $@ -MMD -MP -MF $(DEP_PATH)/$*.d

DIRECTIVES = -pedantic -Wall -Wextra -Wredundant-decls -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wcast-qual -Wcast-align -Wwrite-strings -Wpointer-arith -Wconversion -Wshift-overflow -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -DLOG_WARN -DLOG_ERROR -DLOG_INFO -c -I $(HEADER_PATH)

HEADER_PATH = include
SRC_PATH = src
BIN_PATH = bin
DEP_PATH = dep

CPP_FILES = $(wildcard $(SRC_PATH)/*.cpp)
OBJ_FILES = $(addprefix $(BIN_PATH)/,$(notdir $(CPP_FILES:.cpp=.o)))
DEP_FILES = $(wildcard $(DEP_PATH)/*.d)

EXEC = at.out

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(BIN_PATH)/%.o: $(SRC_PATH)/%.cpp

ifeq ($(OS), Windows_NT)
	@if not exist $(DEP_PATH) @mkdir $(DEP_PATH)
	@if not exist $(BIN_PATH) @mkdir $(BIN_PATH)
else
	@mkdir -p $(DEP_PATH) $(BIN_PATH)
endif

	$(CC) $(DEP_FLAGS) -c -o $@ $< $(DIRECTIVES)

print-% : ; @echo $* = $($*)

debug: DIRECTIVES += -ggdb -O0 -DDEBUG -DLOG_WARN -DLOG_INFO -DLOG_WARN -DLOG_ERROR
debug: all

dev: debug run

gdb: RUN := gdb $(RUN)
gdb: dev

release: DIRECTIVES += -Ofast -mtune=native
release: all

run: all
	$(RUN)$(EXEC)

clean:
	$(RMDIR) $(BIN_PATH) $(DEP_PATH)
	$(RM) $(EXEC)

.PRECIOUS: $(DEP_PATH)/%.D

.PHONY: debug clean release

-include $(DEP_FILES)