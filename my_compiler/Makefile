# Minimal Makefile for my_compiler (textual LLVM IR only)

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude -Ibuild -MMD -MP
LDFLAGS :=
FLEX :=
BISON := bison

BIN := build/my_compiler

SOURCES := \
  main.cpp \
  src/lexer/lexer.cpp \
  src/parser/driver.cpp \
  src/parser/yylex_adapter.cpp \
  src/semantic/semantic.cpp \
  src/semantic/symbol_table.cpp \
  src/semantic/type_system.cpp \
  src/ir/ir_generator.cpp \
  src/ir/ir_utils.cpp \
  src/ir/optimizer.cpp \
  src/utils/logger.cpp \
  src/utils/error.cpp

GENERATED_CPP := build/parser.tab.cpp
GENERATED_HDR := build/parser.tab.h

OBJECTS := $(patsubst %.cpp, build/%.o, $(SOURCES)) \
           $(patsubst build/%.cpp, build/%.o, $(GENERATED_CPP))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean run dirs

all: $(BIN)

# Flex/Bison generation
build/parser.tab.cpp build/parser.tab.h: src/parser/parser.y | dirs
	$(BISON) -d -o build/parser.tab.cpp --defines=build/parser.tab.h src/parser/parser.y

build/lexer.yy.cpp:
	@true

$(BIN): $(OBJECTS) | dirs
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

build/%.o: %.cpp | dirs
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: build/%.cpp | dirs
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
