local_dir := common/codecs/legacy/ilbc
local_src := $(wildcard $(local_dir)/*.c)
local_objs := $(subst .c,.o,$(local_src))
sources += $(local_src)

extra_clean += $(local_dir)/libilbc.a

$(local_dir)/libilbc.a: $(local_objs)
		$(ECHO_AR)
		$(AR) ru $@ $^

$(local_dir)/%.o: $(local_dir)/%.c
	$(ECHO_COMPILE)
	$(CC) $(CFLAGS) -c -o $@ $<
	