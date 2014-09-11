CC = gcc
CFLAGS = -Wall -pedantic -Wno-long-long
INCLUDES = -I/usr/include/gdal 
LIBS = -lgdal -lm

SRCS = driver_00.c module_initialize_00.c module_demloader_00.c module_demloader_00.h module_arrayinit_00.c module_initflow_00.c module_pulse_00.c module_distribute_00.c module_distribute_00.h module_neighbor_00.c module_activate_00.c module_output_00.c

OBJ = $(SRCS:.c=.o)

MAIN = lava_C

.PHONY: depend clean

all:	$(MAIN)
		@echo lava_C has been compiled.

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LIBS)
	mv $(MAIN) ..

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

driver_00.o: main_00.h driver_00.h module_arrayinit_00.h module_distribute_00.h module_neighbor_00.h module_activate_00.h module_demloader_00.h module_initialize_00.h module_pulse_00.h module_output_00.h module_initflow_00.h
module_initialize_00.o: main_00.h module_initialize_00.h
module_demloader_00.o: main_00.h module_demloader_00.h module_arrayinit_00.h
module_arrayinit_00.o: main_00.h module_arrayinit_00.h
module_initflow_00.o: main_00.h module_initflow_00.h module_arrayinit_00.h
module_pulse_00.o: main_00.h module_pulse_00.h
module_distribute_00.o: main_00.h module_distribute_00.h
module_neighbor_00.o: main_00.h module_neighbor_00.h
module_activate_00.o: main_00.h module_activate_00.h
module_output_00.o: main_00.h module_output_00.h

clean:
	$(RM) *.o *~ $(MAIN)

#depend: $(SRCS)
#	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
