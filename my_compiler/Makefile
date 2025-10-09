# Minimal Makefile for my_compiler (textual LLVM IR only)

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude -MMD -MP
LDFLAGS :=

BIN := build/my_compiler

SOURCES := \
  main.cpp \
  src/lexer/lexer.cpp \
  src/parser/parser.cpp \
  src/semantic/semantic.cpp \
  src/semantic/symbol_table.cpp \
  src/semantic/type_system.cpp \
  src/ir/ir_generator.cpp \
  src/ir/ir_utils.cpp \
  src/ir/optimizer.cpp \
  src/utils/logger.cpp \
  src/utils/error.cpp

OBJECTS := $(patsubst %.cpp, build/%.o, $(SOURCES))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean run dirs

all: $(BIN)

$(BIN): $(OBJECTS) | dirs
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

build/%.o: %.cpp | dirs
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BIN) examples/test.c | dirs
	@mkdir -p outputs
	./$(BIN) examples/test.c -o outputs/output.ll
	@echo "LLVM IR written to outputs/output.ll"

clean:
	rm -rf build outputs/output.ll outputs/tokens.out outputs/ast_dump.json outputs/semantic.log

-include $(DEPS)

dirs:
	@mkdir -p build outputs
