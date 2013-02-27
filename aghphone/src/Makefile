programs :=
sources :=
libraries :=
extra_clean :=

objects = $(subst .c,.o,$(subst .cpp,.o,$(sources)))

include_dirs := common ice directory terminal transceiver common/codecs gui transceiver/alsa transceiver/pa transceiver/ccrtp common/codecs mixer gui/qt2

bin_dir := ../bin

CXX := g++
CC := gcc
AR := ar
CFLAGS = -Wall -ansi -pedantic -g -I. $(addprefix -I ,$(include_dirs))
CXXFLAGS = -Wall -ansi -pedantic -g -I. -D__UNIX $(addprefix -I ,$(include_dirs))
LDFLAGS = -lIce -lm -lccgnu2 -lboost_regex -llog4cxx -lportaudio -lccrtp1 -lgsm
SLICEFLAGS = -I ${ICE_HOME}/slice/
SLICE := slice2cpp ${SLICEFLAGS}
vpath %.h $(include_dirs)

MV := mv -f
RM := rm -f
SED := sed

ECHO_COMPILE_EXEC = @echo -e "\n\033[01;32m***********************************\n* Compiling $(@) executable\n***********************************\033[0m"
ECHO_COMPILE = @echo -e "\n\033[01;32m* Compiling $(?)\n\033[0m"
ECHO_AR = @echo -e "\n\033[01;32m* Generating archive $(@)\n\033[0m"
ECHO_ICE = @echo -e "\n\033[01;32m* Generating ICE stubs $(?)\n\033[0m"
ECHO_CLEAN = @echo -e "\n\033[01;32m***********************************\n* Cleaning all targets\n***********************************\033[0m"

all:

include common/module.mk
include ice/module.mk
include directory/module.mk
include terminal/module.mk
include transceiver/module.mk
include transceiver/alsa/module.mk
include transceiver/ccrtp/module.mk
include transceiver/pa/module.mk
include common/codecs/module.mk
include gui/module.mk
include gui/qt2/module.mk
include mixer/module.mk
include common/codecs/legacy/g711/module.mk
include common/codecs/legacy/ilbc/module.mk

.PHONY: all
all: ice/icecommon.o directory/directory.ice ice/iface.o  $(programs)

.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
	$(ECHO_CLEAN)
	$(RM) $(objects) $(programs) $(libraries) $(extra_clean)

#ifneq "$(MAKECMDGOALS)" "clean"
#	include $(dependencies)
#endif

%.o: %.cpp
	$(ECHO_COMPILE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
