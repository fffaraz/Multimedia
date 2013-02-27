local_dir := mixer
local_src := $(wildcard $(local_dir)/*.cpp)
local_objs := $(subst .cpp,.o,$(local_src))
sources += $(local_src)

programs += $(bin_dir)/mixer

$(bin_dir)/mixer: $(local_objs) \
	mixer/mixer.o common/globals.o common/tools.o common/codecfactory.o \
	ice/iface.o ice/icecommon.o common/codecs/dummycodec.o directory/directory.o \
	common/msgbuffer.o transceiver/ccrtp/aghrtpsession.o transceiver/ccrtp/transportccrtp.o \
	common/ringbuffer.o \
	common/codecs/g711.o common/codecs/legacy/g711/g711.o \
	common/codecs/gsm.o common/codecs/ilbc20.o \
	common/codecs/ilbc20.o common/codecs/legacy/ilbc/libilbc.a

	$(ECHO_COMPILE_EXEC)
	$(CXX) $(LDFLAGS) -Lcommon/codecs/legacy/ilbc -lilbc -o $@ $^
