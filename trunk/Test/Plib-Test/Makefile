# Make Plib Test Solution.
# Powered By Push Chen
# 
#

PLIB_ROOT = ../..
DEV_LIB = $(PLIB_ROOT)/Plib
REL_LIB = $(PLIB_ROOT)/Plib
TEST_ROOT = ./

OUT_DIR = $(TEST_ROOT)/$(MAKECMDGOALS)

PLIB_DEFINES = -DLINUX -DPLIB -DVERSION=\"1.1.0.3\"

ifeq "$(MAKECMDGOALS)" "release"
	DEFINES = $(PLIB_DEFINES) -DPLIB_RELEASE -DPLIB_USE_MEMPOOL -DPLIB_LOG_DEBUG_
	CPPFLAGS = 
	CFLAGS = -O2 -Wall $(DEFINES) -I$(REL_LIB) -fPIC -pthread
	CXXFLAGS = -O2 -Wall $(DEFINES) -I$(REL_LIB) -fPIC -pthread
	CMDGOAL_DEF := $(MAKECMDGOALS)
else
	ifeq "$(MAKECMDGOALS)" "withpg"
		DEFINES = $(PLIB_DEFINES) -DPLIB_WITHPG -D_PLIB_ERR_DEBUG_ -DPLIB_MEMORY_DEBUG -DPLIB_OBJECT_DEBUG
		CPPFLAGS = 
		CFLAGS = -g -pg -Wall $(DEFINES) -I$(DEV_LIB) -fPIC -pthread
		CXXFLAGS = -g -pg -Wall $(DEFINES) -I$(DEV_LIB) -fPIC -pthread
		CMDGOAL_DEF := $(MAKECMDGOALS)
	endif
	ifeq "$(MAKECMDGOALS)" "debug"
		DEFINES = $(PLIB_DEFINES) -DPLIB_DEBUG -DPLIB_STD_DEBUG -DPLIB_MEMORY_DEBUG #-DPLIB_LOCK_DEBUG
		CPPFLAGS = 
		CFLAGS = -g -Wall $(DEFINES) -I$(DEV_LIB) -fPIC -pthread
		CXXFLAGS = -g -Wall $(DEFINES) -I$(DEV_LIB) -fPIC -pthread
		CMDGOAL_DEF := $(MAKECMDGOALS)
	endif
endif

vpath %.h   ./plib-test/
vpath %.cpp	./plib-test/
vpath %.h	./plibClient/
vpath %.cpp ./plibClient/

CC	 = g++
CPP	 = g++
CXX	 = g++
AR	 = ar

STATIC_LIBS = 
DYNAMIC_LIBS = 
EXECUTABLE = psvr pclient
TEST_CASE = 

RELAY_OBJECT = 

all	: PreProcess $(STATIC_LIBS) $(DYNAMIC_LIBS) $(EXECUTABLE) $(TEST_CASE) AfterMake

PreProcess :
ifndef CMDGOAL_DEF
	@$(MAKE) cmdgoalError
	@exit 1
else
	@if test -d $(OUT_DIR); then rm -rf $(OUT_DIR); fi
	@mkdir -p $(OUT_DIR)/static
	@mkdir -p $(OUT_DIR)/dynamic
	@mkdir -p $(OUT_DIR)/bin
	@mkdir -p $(OUT_DIR)/test
	@mkdir -p $(OUT_DIR)/data
	@mkdir -p $(OUT_DIR)/conf
	@mkdir -p $(OUT_DIR)/log
endif

cmdgoalError :
	@echo '***************************************************'
	@echo '******You must specified a make command goal.******'
	@echo '***************************************************'

clean :
	@echo '***************************************************'
	@echo 'You should use debugclean, relclean or pgclean to clean things.'
	@echo 'Or you can use cleanall to clean everything.'
	@echo '***************************************************'

debugclean :
	@rm -rf $(TEST_ROOT)/debug

relclean :
	@rm -rf $(TEST_ROOT)/release

pgclean : 
	@rm -rf $(TEST_ROOT)/withpg

AfterMake : 
	$(shell rm -rf *.o)
	$(shell if [ -x "$(STATIC_LIBS)" ]; then mv $(STATIC_LIBS) $(OUT_DIR)/static; fi)
	$(shell if [ -x "$(DYNAMIC_LIBS)" ]; then mv $(DYNAMIC_LIBS) $(OUT_DIR)/dynamic; fi)
	$(shell mv -f $(EXECUTABLE) $(OUT_DIR)/bin)
	$(shell if [ -x "$(TEST_CASE)" ]; then mv $(TEST_CASE) $(OUT_DIR)/test; fi)

debug : PreProcess $(STATIC_LIBS) $(DYNAMIC_LIBS) $(EXECUTABLE) $(TEST_CASE) AfterMake
	@exit 0

release : PreProcess $(STATIC_LIBS) $(DYNAMIC_LIBS) $(EXECUTABLE) $(TEST_CASE) AfterMake
	@exit 0

withpg : PreProcess $(STATIC_LIBS) $(DYNAMIC_LIBS) $(EXECUTABLE) $(TEST_CASE) AfterMake
	@exit 0

psvr : plibT.o
	g++ -o psvr ./plib-test/plibT.cpp $(CXXFLAGS)

pclient : plibClient.o
	g++ -o pclient ./plibClient/plibClient.cpp $(CXXFLAGS)

