local_dir := directory
local_src := $(wildcard $(local_dir)/*.cpp)
local_objs := $(subst .cpp,.o,$(local_src))
sources += $(local_src)
extra_clean += $(local_dir)/directory.cpp $(local_dir)/directory.h $(local_dir)/directory.o

directory/directory.cpp: directory/directory.ice ice/icecommon.cpp
		$(ECHO_ICE)
		cd directory && $(SLICE) $(<F)

programs += $(bin_dir)/directory

$(bin_dir)/directory: directory/directory.o common/tools.o ice/icecommon.o $(local_objs) 
		$(ECHO_COMPILE_EXEC)
		$(CXX) $(LDFLAGS) -o $@ $^

