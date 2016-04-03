# README #

This README shall document the steps necessary to these applications up and running.

### What is this repository for? ###

These are demo projects using the Nucleo F401 board and libopencm3 libraries. 

### How do I get set up? ###

* Generic instructions

Get the repository and libopencm3 in a folder.
Install a gcc arm toolchain and make sure it's used to compile by setting the correct PATH to it.

Since the makefiles use GNU compiler tools and linux commands (e.g. "ls"), in order to compile on Windows environment something as MinGW or Cygwin should be used.
In particular for Windows, download and install:

1. msys ([http://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/MSYS-1.0.11.exe](Link URL)) or equivalent.
2. Python 2.7 ([http://www.python.org/ftp/python/2.7/python-2.7.msi](Link URL))
3. arm-none-eabi/arm-elf toolchain (for example this one [https://launchpad.net/gcc-arm-embedded](Link URL))

Set the path for the tools and the toolchain, for example:

*
set PATH=C:\msys\1.0\bin\;C:\Python27\;C:\path-to-gcc-arm-none-eabi\bin;*

Others points to be filled properly:

* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions