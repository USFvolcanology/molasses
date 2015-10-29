### NAME
	**MOLASSES**
 
### DESCRIPTION

MOLASSES is an in-progress fluid flow simulator, written in C. It is not yet complete and is in need of testing and additional programming before it can be used effectively.

MOLASSES stands for *MO*dular *LA*va *S*imulation *S*oftware for *E*arth *S*cience.

### REQUIREMENTS

MOLASSES requires the GDAL C libraries and a C compiler. To get the GDAL libraries, install gdal-devel on your UNIX machine. We have tested this program on computers that use the C compiler gcc.

### INSTALLATION

Before compilation of MOLASSES it is prudent to modify the Makefile in this directory for your use. Check to make sure that the installation path and the path to GDAL are properly set. Modify the module numbers at the top of the Makefile to change the model algorithm to what you require.

To compile and install MOLASSES execute the following commands:
		make
		make install

You may need to be in super-user mode to perform make install.

### EXECUTING

To run the code after installation, use the command

		molasses <config-file>

where <config-file> is a text configuration file. A sample configuration file (sample.cfg) is given in the main directory. An elevation model is required to run molasses, and at least one output file path and one vent location must be given in the configuration file. More information is documented in the docs directory.

### UNINSTALLATION

To uninstall MOLASSES run
		make uninstall

### AUTHORS

Jacob Richardson (jarichardson@mail.usf.edu)

Laura Connor     (lconnor@usf.edu)

Chuck Connor     (cbconnor@usf.edu)
  
#COPYRIGHT

Copyright 2014, Creative Commons Attribution-NonCommercial 3.0 Unported (CC BY-NC 3.0). http://creativecommons.org/licenses/by-nc/3.0/
