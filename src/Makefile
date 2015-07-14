CFLAGS = -Wall -pedantic -g -O2 -Wno-long-long
INCLUDES = -I$(GDALPATH)
LIBS = -lgdal -lm

SRCS = driver_$(driver).c module_initialize_$(initialize).c \
	module_demloader_$(DEM_loader).c module_arrayinit_$(array_initializer).c \
	module_initflow_$(flow_initializer).c module_pulse_$(pulse).c \
	module_distribute_$(distribute).c module_neighbor_$(neighbor_ID).c \
	module_activate_$(activate).c module_output_$(output).c

OBJ = $(SRCS:.c=.o)

MAIN = molasses

.PHONY: depend clean check install uninstall

all:	$(MAIN)
		@echo "*** $(MAIN) has been compiled. ***"

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $+ $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

$(OBJ): structs.h prototypes.h

check: all
	@echo "Testing for conservation of mass..."
	@./$(MAIN) ./pcheck/pancake.cfg | grep 'SUCCESS: MASS CONSERVED'
	@echo "*** All TESTS PASSED ***"

clean:
	$(RM) *.o *~ $(MAIN) pcheck/out pcheck/*~

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 $(MAIN) $(DESTDIR)$(bindir)

uninstall:
	-rm $(DESTDIR)$(bindir)/$(MAIN) &>/dev/null

#depend: $(SRCS)
#	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
