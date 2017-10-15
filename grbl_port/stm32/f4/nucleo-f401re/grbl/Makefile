##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

BINARY = main

LDSCRIPT = ../nucleo-f401re_ramcode.ld

CPPFLAGS += -I.  -I./cpu_map -I./defaults

OBJS		+= reset_vector.o nuts_bolts.o report.o stepper.o settings.o serial.o system.o print.o limits.o flash.o gcode.o motion_control.o protocol.o planner.o probe.o coolant_control.o spindle_control.o

include ../../Makefile.include

LDLIBS += -lm 

