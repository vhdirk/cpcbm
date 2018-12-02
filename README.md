##Youtube Channel: [The Ant PCB Maker](https://www.youtube.com/channel/UCX44z-SSL7LzcB4xxgUdHHA)

##Contact us at: compact.pcb.maker.team@gmail.com

[![alternativetext](https://bitbucket.org/compactpcbmaker/cpcbm/raw/517162e310646dce856bea25bbfab23e390ca87a/resources/the_ant_logo/the_ant_logo.png =50x20)](https://www.youtube.com/channel/UCX44z-SSL7LzcB4xxgUdHHA)


The Ant is a project to develop a CNC machine able to realize single and double-layered printed circuit boards.

[Here is a video presentation](https://youtu.be/nVkbG-CYaAA)

The CNC machine is designed to achieve well-determined characteristics:

- Compact
- Low-cost
- Robust
- Scalable
- Easy to build
- Open License

To achieve these properties the following design principle have been adopted:

- Electrical, mechanical and electronics parts have been chosen to be small, low-cost and easily available on the open market or 3D printable
- Mechanical design has been oriented to minimize the space occupied by the components
- Mechanics has been designed to be robust and stress resistant
- Mechanics has been designed so that the external dimensions of the machine may be changed with a minimal change of pieces and no re-design needed.
- Mechanical and electrical designs have taken in account the assembling and disassembling operations, allowing to use easily available tools as screwdrivers, Allen wrenches and a solder for electrical contacts.
- The microcontroller board used to control the CNC machine is inexpensive (~15 $) but is equipped with a powerful STM32 processor and many peripherals.
- The machine control firmware can be loaded with almost no effort, since the microcontroller board has been chosen to have an on-board programmer.
- The control firmware is derived from GRBL v.0.9j, but it is also customized to achieve better performance, taking advantage of the powerful processor, and to be more flexible, since more configuration parameters and features have been added respect to the original firmware

