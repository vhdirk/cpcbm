##
## This file is part of the grbl_port_opencm3 project.
##
## Copyright (C) 2017 Angelo Di Chello
##
## Grbl_port_opencm3 is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Grbl_port_opencm3 is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Grbl_port_opencm3.  If not, see <http://www.gnu.org/licenses/>.
##

PREFIX		?= arm-none-eabi
#PREFIX		?= arm-elf

TARGETS		:= stm32/f4
# These targets are commented since not used at the moment.
#TARGETS		:= stm32/f0 stm32/f1 stm32/f2 stm32/f3 stm32/l0 stm32/l1
#TARGETS		+= lpc/lpc13xx lpc/lpc17xx #lpc/lpc43xx
#TARGETS		+= tiva/lm3s tiva/lm4f
#TARGETS		+= efm32/efm32tg efm32/efm32g efm32/efm32lg efm32/efm32gg
#TARGETS		+= vf6xx

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

OPENCM3_DIR ?= $(realpath libopencm3)
GRBL_PORT_RULES = elf

all: build

grbl: GRBL_PORT_RULES += bin
bin: GRBL_PORT_RULES += bin
hex: GRBL_PORT_RULES += hex
srec: GRBL_PORT_RULES += srec
list: GRBL_PORT_RULES += list
images: GRBL_PORT_RULES += images

bin: build
hex: build
srec: build
list: build
images: build

build: lib grbl_port

lib:
	$(Q)if [ ! "`ls -A $(OPENCM3_DIR)`" ] ; then \
		printf "######## ERROR ########\n"; \
		printf "\tlibopencm3 is not initialized.\n"; \
		printf "\tPlease run:\n"; \
		printf "\t$$ git submodule init\n"; \
		printf "\t$$ git submodule update\n"; \
		printf "\tbefore running make.\n"; \
		printf "######## ERROR ########\n"; \
		exit 1; \
		fi
	$(Q)$(MAKE) -C $(OPENCM3_DIR)

GRBL_PORT_DIRS:=$(sort $(dir $(wildcard $(addsuffix /*/Makefile,$(addprefix grbl_port/,$(TARGETS))))))
$(GRBL_PORT_DIRS): lib
	@printf "  BUILD   $@\n";
	$(Q)$(MAKE) --directory=$@ OPENCM3_DIR=$(OPENCM3_DIR) $(GRBL_PORT_RULES)
	
grbl_port: $(GRBL_PORT_DIRS)
	$(Q)true

# Compile executables only, and assumes lib have been compiled
grbl:
	$(Q)$(foreach x,$(GRBL_PORT_DIRS), \
	printf "  BUILD   $(x)\n"; \
	$(MAKE) --directory=$(x) OPENCM3_DIR=$(OPENCM3_DIR) $(GRBL_PORT_RULES);	)
	

clean: $(GRBL_PORT_DIRS:=.clean) styleclean
	$(Q)$(MAKE) -C libopencm3 clean

stylecheck: $(GRBL_PORT_DIRS:=.stylecheck)
styleclean: $(GRBL_PORT_DIRS:=.styleclean)

# Clean executables directories only
clean_grbl: $(GRBL_PORT_DIRS:=.clean) 
	$(Q)true

%.clean:
	$(Q)if [ -d $* ]; then \
		printf "  CLEAN   $*\n"; \
		$(MAKE) -C $* clean OPENCM3_DIR=$(OPENCM3_DIR) || exit $?; \
	fi;

%.styleclean:
	$(Q)$(MAKE) -C $* styleclean OPENCM3_DIR=$(OPENCM3_DIR)

%.stylecheck:
	$(Q)$(MAKE) -C $* stylecheck OPENCM3_DIR=$(OPENCM3_DIR)


.PHONY: build lib grbl_port grbl clean_grbl install clean stylecheck styleclean \
        bin hex srec list images

