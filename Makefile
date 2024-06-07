CXX := g++
CXXFLAGS := $(shell llvm-config --cxxflags --ldflags --libs all --system-libs) -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING -fexceptions -I external/argparse/include/ -std=c++20
LD := ld

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src

BIN := $(BIN_DIR)/bfc

all: $(BIN)

$(BIN): $(OBJ_DIR)/main.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/generator.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp	
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
