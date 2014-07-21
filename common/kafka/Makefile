INC_ROOT := ../../../external
LOG4CPLUS := $(INC_ROOT)/log4cplus
ICE := $(INC_ROOT)/ice
BOOST := $(INC_ROOT)/boost
KAFKA := $(INC_ROOT)/kafka
COMMON := ..
INC_DIR := ./ $(COMMON) $(LOG4CPLUS)/include $(ICE)/include $(BOOST)/include $(KAFKA)/include
LIB_DIR := $(LOG4CPLUS)/lib $(ICE)/lib64 $(BOOST)/lib $(KAFKA)/lib
TAG_DIR := .

LIBS := log4cplus kafka z

TARGET_LIB := $(TAG_DIR)/libkafka_wrapper.a
LIB_KAFKA_WRAPPER := $(TAG_DIR)/libkafka_wrapper.so
TEST_PROGRAM := test/kafka_app

#ACEX_MACROS := -DACEX_NTRACE

CFLAGS := -g -Wall -fPIC
CXXFLAGS := $(CFLAGS)
CPPFLAGS += $(addprefix -I, $(INC_DIR))
ARFLAGS :=
LDFLAGS := -shared

RM-F := rm -f

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cpp,$(MISSING_DEPS)))
CPPFLAGS += -MD

TEST_OBJS := $(patsubst %.cpp,%.o,$(wildcard test/*.cpp))

.PHONY : everything deps objs clean veryclean rebuild all test

everything : $(TARGET_LIB) $(TEST_PROGRAM)
all : $(TARGET_LIB) $(LIB_KAFKA_WRAPPER) $(TEST_PROGRAM)
test : $(TEST_PROGRAM)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) *.o
	@$(RM-F) *.d
	@$(RM-F) test/*.o
	@$(RM-F) test/*.d

veryclean: clean
	@$(RM-F) $(TARGET_LIB) $(LIB_KAFKA_WRAPPER) $(TEST_PROGRAM)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(TARGET_LIB) : $(OBJS)
	rm -f $@
	$(AR) -r $(TARGET_LIB) $(OBJS)

$(TEST_PROGRAM) : $(TARGET_LIB) $(TEST_OBJS)
	rm -f $@
	$(CXX) -o $(TEST_PROGRAM) $(TEST_OBJS) $(TARGET_LIB) $(addprefix -L, $(LIB_DIR)) $(addprefix -l, $(LIBS))

$(LIB_KAFKA_WRAPPER) : $(OBJS)
	rm -f $@
	$(CXX) -o $(LIB_KAFKA_WRAPPER) $(OBJS) $(addprefix -L, $(LIB_DIR)) $(addprefix -l, $(LIBS)) $(LDFLAGS)
