local_dir := terminal
local_src := $(wildcard $(local_dir)/*.cpp)
local_objs := $(subst .cpp,.o,$(local_src))
sources += $(local_src)

programs += $(bin_dir)/aghphone

$(bin_dir)/aghphone: $(local_objs) \
	directory/directory.o gui/dummyui.o common/globals.o common/main.o \
	common/tools.o common/codecfactory.o common/ringbuffer.o ice/iface.o ice/icecommon.o \
	transceiver/ccrtp/aghrtpsession.o common/codecs/dummycodec.o \
	transceiver/alsa/devicealsa.o transceiver/alsa/devicefactoryalsa.o \
	transceiver/alsa/audioalsa.o transceiver/ccrtp/transportccrtp.o \
	transceiver/pa/devicepa.o transceiver/pa/devicefactorypa.o \
	transceiver/pa/audiopa.o \
	transceiver/transceiver.o transceiver/transceiverfactory.o common/msgbuffer.o \
	common/codecs/legacy/g711/g711.o common/codecs/g711.o \
	common/codecs/gsm.o common/codecs/ilbc20.o common/codecs/legacy/ilbc/libilbc.a
		$(ECHO_COMPILE_EXEC)
		$(CXX) $(LDFLAGS) -Lcommon/codecs/legacy/ilbc -lilbc -o $@ $^

