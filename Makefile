#Makefile for compiling MOLASSES Modular Lava Simulator
#The Floor is Lava.

#MODULES: Alter as needed. 
#Check docs/ for module codes and destriptions.
export driver            = 00
export initialize        = 00
export DEM_loader        = 00
export array_initializer = 00
export flow_initializer  = 00
export pulse             = 00
export distribute        = 04
export neighbor_ID       = 01
export activate          = 01
export output            = 00

# Tool-related variables: Alter as needed for your system.
export CC          ?= gcc
export INSTALLPATH = /usr/local
export GDALPATH    = /usr/include/gdal

export bindir      = $(INSTALLPATH)/bin

# Package-related substitution variables
export package     = molasses
export version     = 1.0.0
export tarname     = $(package)
export distdir     = $(tarname)-$(version)

all clean check install uninstall molasses:
	$(MAKE) -C src $@

dist: $(distdir).tar.gz
	@echo "*** Made tarball at $(distdir).tar.gz ***"

$(distdir).tar.gz: FORCE $(distdir)
	tar chof - $(distdir) | gzip -9 -c >$(distdir).tar.gz
	rm -rf $(distdir)

$(distdir):
	mkdir -p $(distdir)/src
	cp Makefile $(distdir)
	cp sample.cfg $(distdir)
	cp README $(distdir)
	cp src/Makefile $(distdir)/src
	cp src/*.c $(distdir)/src
	cp src/*.h $(distdir)/src
	mkdir $(distdir)/src/pcheck
	cp src/pcheck/* $(distdir)/src/pcheck
	mkdir $(distdir)/raster_samples
	cp raster_samples/flat_1300x1300.asc $(distdir)/raster_samples
	mkdir $(distdir)/docs
	cp docs/* $(distdir)/docs

distcheck: $(distdir).tar.gz
	gzip -cd $+ | tar xvf -
	$(MAKE) -C $(distdir) all check
	$(MAKE) -C $(distdir) DESTDIR=$${PWD}/$(distdir)/_inst install uninstall
	$(MAKE) -C $(distdir) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution. ***"

FORCE:
	-rm -rf $(distdir) &>/dev/null
	-rm $(distdir).tar.gz &>/dev/null

.PHONY: FORCE all clean check dist distcheck install uninstall
