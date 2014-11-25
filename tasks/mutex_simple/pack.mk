PROGS_DAGGER_OBJS := mutex_simple.o
PROGS_DAGGER_OBJS := $(PROGS_DAGGER_OBJS:%=$(TDIR)/mutex_simple/%)
ALL_OBJS += $(PROGS_DAGGER_OBJS)

$(TDIR)/bin/mutex_simple : $(TSTART) $(PROGS_DAGGER_OBJS) $(TLIBC)

