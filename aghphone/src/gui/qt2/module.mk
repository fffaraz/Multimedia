local_dir := gui/qt2
local_src := $(wildcard $(local_dir)/*.cpp) 
local_objs := $(subst .cpp,.o,$(local_src))
sources += $(local_src)
programs += $(bin_dir)/qtgui
extra_clean += $(local_dir)/Makefile $(local_dir)/moc_* $(local_dir)/ui_*.h

qtgui_objs := directory/directory.o gui/dummyui.o common/globals.o \
	common/tools.o common/codecfactory.o common/ringbuffer.o ice/iface.o ice/icecommon.o \
	transceiver/ccrtp/aghrtpsession.o common/codecs/dummycodec.o \
	transceiver/alsa/devicealsa.o transceiver/alsa/devicefactoryalsa.o \
	transceiver/alsa/audioalsa.o transceiver/ccrtp/transportccrtp.o \
	transceiver/pa/devicepa.o transceiver/pa/devicefactorypa.o \
	transceiver/pa/audiopa.o terminal/terminal.o \
	transceiver/transceiver.o transceiver/transceiverfactory.o common/msgbuffer.o \
	common/codecs/legacy/g711/g711.o common/codecs/g711.o \
	common/codecs/gsm.o common/codecs/ilbc20.o common/codecs/legacy/ilbc/libilbc.a

$(bin_dir)/qtgui: $(local_src) $(qtgui_objs)
		$(ECHO_COMPILE_EXEC)
		cd gui/qt2 && qmake qt.pro INCLUDEPATH+="../../ $(addprefix ../../,$(include_dirs))" LIBS+="$(addprefix ../../,$(qtgui_objs)) $(LDFLAGS)"
		cd gui/qt2 && make && chmod +x qt
		mv gui/qt2/qt $(bin_dir)/qtgui
