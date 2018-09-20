![alternativetext](https://bitbucket.org/compactpcbmaker/cpcbm/raw/517162e310646dce856bea25bbfab23e390ca87a/resources/the_ant_logo/the_ant_logo.png)

# README #

This README shall document the steps necessary to these applications up and running.

### What is this repository for? ###

These is the Compact PCB Maker project using the Nucleo F401 board and libopencm3 libraries. 

### How do I get set up? ###

* Generic instructions

Get the repository in a folder (I call it "main folder" from now on), and get the library libopencm3 in "libopencm3" sub-folder.

You should install a gcc arm toolchain and make sure it's used to compile by setting the correct PATH to it.
Since the makefiles use GNU compiler tools and linux commands (e.g. "ls"), in order to compile on Windows environment something as MinGW or Cygwin should be used.
In particular for Windows, download and install:

1. msys ([click here](http://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/MSYS-1.0.11.exe)) or equivalent.
2. Python 2.7 ([click here](http://www.python.org/ftp/python/2.7/python-2.7.msi))
3. arm-none-eabi/arm-elf toolchain (for example this one [here](https://launchpad.net/gcc-arm-embedded))

Set the path for the tools and the toolchain, for example:

*
set PATH=C:\msys\1.0\bin\;C:\Python27\;C:\path-to-gcc-arm-none-eabi\bin;*

Then from a command line change directory tothe main project folder and type:

make bin all

, to compile the projects and generate the binaries.
To clean use the command:

make clean
