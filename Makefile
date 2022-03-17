# compiler env.
UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
	CXX=ccache clang++
	LDLIBS=-ldl -lasound
	CXXFLAGS=-std=c++2a -MMD -MP -Wall -fPIC
	SHARED_CXXFLAGS= -fPIC -shared
endif
ifeq ($(UNAME),Darwin)
	CXX=ccache clang++
	LDLIBS=-stdlib=libc++
	CXXFLAGS=-std=c++2a -MMD -MP -Wall
#	SHARED_CXXFLAGS= -flat_namespace -dynamiclib
	SHARED_CXXFLAGS=-dynamiclib
endif

LDFLAGS=-pthread

# --- project config -------
LIB_SRC_DIR=./src
LIB_INC_DIR=./include
LIB_TARGET_DIR=./lib
LIB_TARGET_FILENAME=libasynctask
TEST_TARGET_FILENAME=asynctasktest
BIN_DIR=./bin
OBJ_DIR=./out
TEST_DIR=./test

LIB_DEP_INC_DIR=.
LIB_DEP_LIB_DIR=.
LIB_DEP_LIB=


# --- source code config --------------
LIB_TARGET_SRCS = $(wildcard $(LIB_SRC_DIR)/*.cpp)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)

# --- the object files config --------------
LIB_TARGET_OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(LIB_TARGET_SRCS:.cpp=.o)))
TEST_OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(TEST_SRCS:.cpp=.o)))

# --- Build for shared library ------------
UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
	LIB_SO_TARGET = $(LIB_TARGET_DIR)/$(LIB_TARGET_FILENAME).so
endif
ifeq ($(UNAME),Darwin)
	LIB_SO_TARGET = $(LIB_TARGET_DIR)/$(LIB_TARGET_FILENAME).dylib
endif

LIB_TARGET_DEPS = $(LIB_TARGET_OBJS:.o=.d)

default: $(LIB_SO_TARGET)
.PHONY: default

$(LIB_SO_TARGET): $(LIB_TARGET_OBJS)
	@[ -d $(LIB_TARGET_DIR) ] || mkdir -p $(LIB_TARGET_DIR)
	$(CXX) $(LDFLAGS) $(SHARED_CXXFLAGS) $(LIB_TARGET_OBJS) -o $@ $(LDLIBS) $(LIB_DEP_LIB)

$(LIB_TARGET_OBJS): $(LIB_TARGET_SRCS)
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I $(LIB_DEP_INC_DIR) -I $(LIB_INC_DIR) -c $(LIB_SRC_DIR)/$(notdir $(@:.o=.cpp)) -o $@

-include $(LIB_TARGET_DEPS)


# --- Build for test cases w/libafw.so ---
TEST_TARGET = $(BIN_DIR)/$(TEST_TARGET_FILENAME)
TEST_LDLIBS = $(LDLIBS) -L$(LIB_DEP_LIB_DIR)
TEST_LIBS = $(LIB_DEP_LIB) $(LIB_SO_TARGET)

test: $(TEST_TARGET)
.PHONY: test

$(TEST_TARGET): $(TEST_OBJS)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) $(TEST_LDLIBS) $(TEST_OBJS) $(TEST_LIBS) -o $@ -lgtest_main -lgtest

$(TEST_OBJS): $(TEST_SRCS)
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I $(LIB_DEP_INC_DIR)  -I $(LIB_INC_DIR) -c $(TEST_DIR)/$(notdir $(@:.o=.cpp)) -o $@


.PHONY: all
all: $(LIB_SO_TARGET) $(TEST_TARGET)


# --- clean up ------------------------
clean:
	rm -f $(LIB_TARGET_OBJS) $(LIB_SO_TARGET)
