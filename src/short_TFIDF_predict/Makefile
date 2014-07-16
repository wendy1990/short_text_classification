INC_ROOT := ../../../../../external
INC_DIR := ./ $(INC_ROOT) ../../../../../external/libsvm/include $(INC_ROOT)/log4cplus/include ../../../../common ../../../classification_server ../../../../../external/boost/include
LIB_DIR := $(INC_ROOT) ../../../../../external/libsvm/lib $(INC_ROOT)/log4cplus/lib
TAG_DIR := .

EXECUTABLE := $(TAG_DIR)/libshort_predict.so
LIBS := svm log4cplus

MACROS := -DOS_LINUX

#CFLAGS := -g -Wall -O
CXXFLAGS := $(CFLAGS)
CPPFLAGS += $(addprefix -I, $(INC_DIR))
ARFLAGS :=

RM-F := rm -f

SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cpp,$(MISSING_DEPS)))
CPPFLAGS +=  -g -Wall -MD -m64 $(MACROS) -fPIC -shared

.PHONY : everything deps objs clean veryclean rebuild

everything : $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) *.o
	@$(RM-F) *.d

veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(EXECUTABLE) : $(OBJS)
	rm -f $@
	$(CXX) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJS) $(addprefix -L, $(LIB_DIR)) $(addprefix -l, $(LIBS))
