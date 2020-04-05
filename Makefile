CXX = g++ 

FLGS = -std=c++14 

CXXFLAGS = $(FLGS) -g -O0 -Wall -W

LDFLAGS = s7.o 
LDLIBS = 

OUT_DIR = out
OUT_FLGS = $(FLGS) -O3 -DNDEBUG 
OUT_LDFLGS = $(LDFLAGS)
OUT_LDLIBS = $(LDLIBS)

################################################################

CPP_HMS = $(wildcard *.cpp.hm)
HM_CPPS = $(CPP_HMS:.cpp.hm=.cpp)
HM_OBJS = $(HM_CPPS:.cpp=.o)
HPP_HMS = $(wildcard *.hpp.hm)
HM_HPPS = $(HPP_HMS:.hpp.hm=.hpp)

EXEC = $(shell basename `pwd`)
SRCS = $(wildcard *.cpp)
DEPS = $(SRCS:.cpp=.d)
CXXFLAGS += -MMD -MP
OBJS = $(filter-out $(HM_OBJS), $(SRCS:.cpp=.o))
OUT_OBJS = $(addprefix $(OUT_DIR)/, $(OBJS))
OUT_HM_OBJS = $(addprefix $(OUT_DIR)/, $(HM_OBJS))

.PHONY: debug
debug: haschem $(EXEC) 

$(EXEC): $(HM_OBJS) $(OBJS)
	$(CXX) -o $(EXEC) $(LDFLAGS) $(LDLIBS) $(HM_OBJS) $(OBJS)

-include $(DEPS)

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)
	rm -f -d -r $(OUT_DIR)

.PHONY: release
release: haschem $(OUT_DIR)/$(EXEC)

$(OUT_DIR)/$(EXEC): $(OUT_DIR) $(OUT_HM_OBJS) $(OUT_OBJS) 
	$(CXX) -o $(OUT_DIR)/$(EXEC) $(OUT_LDFLGS) $(OUT_LDLIBS) $(OUT_HM_OBJS) $(OUT_OBJS)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(OUT_DIR)/%.o: %.cpp %.o  
	$(CXX) $(OUT_FLGS) -c -o $@ $< 

.PHONY: haschem
haschem: $(HM_CPPS) $(HM_HPPS)

%.cpp: %.cpp.hm
	haschem $< $@

%.hpp: %.hpp.hm
	haschem $< $@

