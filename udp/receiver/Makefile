BIN=prog
CXX=gcc
EXT=c

BIN_DIR=bin
BUILD_DIR=build

SOURCES=$(wildcard *.$(EXT))
OBJECTS=$(SOURCES:%.$(EXT)=$(BUILD_DIR)/%.o)
DEPENDS=$(SOURCES:%.$(EXT)=$(BUILD_DIR)/%.d)

CXXFLAGS=-Wall -MMD
INCLUDES=-I/usr/local/include
LDFLAGS=-L/usr/local/lib 
LDLIBS= 

all: $(BIN_DIR)/$(BIN)

$(BIN_DIR)/$(BIN) : $(OBJECTS) 
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@ 

$(BUILD_DIR)/%.o : %.$(EXT)
	@mkdir -p $(BUILD_DIR) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ 

-include $(DEPENDS)

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)
