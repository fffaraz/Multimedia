local_dir := common/codecs/legacy/g711
local_src := $(wildcard $(local_dir)/*.c)
local_objs := $(subst .c,.o,$(local_src))
sources += $(local_src)

$(local_dir)/%.o: $(local_dir)/%.c
	$(ECHO_COMPILE)
	$(CC) $(CFLAGS) -c -o $@ $<