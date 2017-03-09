# Hey Emacs, this is a -*- makefile -*-

# The purpose of this file is to define the build configuration variables used
# by the generic Makefile. See Makefile header for further information.

# Copyright (c) 2009 Atmel Corporation. All rights reserved. 
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer. 
#
# 2. Redistributions in binary form must reproduce the above copyright notice, 
# this list of conditions and the following disclaimer in the documentation 
# and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived 
# from this software without specific prior written permission.  
#
# 4. This software may only be redistributed and used in connection with an Atmel 
# AVR product. 
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE 
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR 
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
#

# Base paths
PRJ_PATH = ../../../..
APPS_PATH = $(PRJ_PATH)/APPLICATIONS
BRDS_PATH = $(PRJ_PATH)/BOARDS
COMP_PATH = $(PRJ_PATH)/COMPONENTS
DRVR_PATH = $(PRJ_PATH)/DRIVERS
SERV_PATH = $(PRJ_PATH)/SERVICES
UTIL_PATH = $(PRJ_PATH)/UTILS

# CPU architecture: {ap|ucr1|ucr2}
ARCH = ucr2

# Part: {none|ap7xxx|uc3xxxxx}
PART = uc3a0512

# Flash memories: [{cfi|internal}@address,size]...
FLASH = internal@0x80000000,512Kb

# Clock source to use when programming: [{xtal|extclk|int}]
PROG_CLOCK = xtal

# Device/Platform/Board include path
PLATFORM_INC_PATH = \
  $(BRDS_PATH)/

# Target name: {*.a|*.elf}
TARGET = $(PART)-Lab4.elf

# Definitions: [-D name[=definition]...] [-U name...]
# Things that might be added to DEFS:
#   BOARD             Board used: see $(BRDS_PATH)/board.h
#   EXT_BOARD         Extension board used (if any): see $(BRDS_PATH)/board.h
DEFS = -D BOARD=EVK1100 \
       -D USB_DEVICE_FEATURE=ENABLED \
       -U FREERTOS_USED

# Include path
INC_PATH = \
  $(UTIL_PATH)/ \
  $(UTIL_PATH)/PREPROCESSOR/ \
  $(SERV_PATH)/USB/CLASS/DFU/EXAMPLES/ISP/BOOT/ \
  $(DRVR_PATH)/SPI/ \
  $(DRVR_PATH)/INTC/ \
  $(DRVR_PATH)/PM/ \
  $(DRVR_PATH)/FLASHC/ \
  $(DRVR_PATH)/GPIO/ \
  $(UTIL_PATH)/DEBUG/ \
  $(DRVR_PATH)/USART/ \
  $(SERV_PATH)/DELAY/ \
  $(DRVR_PATH)/CPU/CYCLE_COUNTER \
  $(DRVR_PATH)/ADC/ \
  $(DRVR_PATH)/TC/ \
  $(DRVR_PATH)/PWM/ \
  $(COMP_PATH)/DISPLAY/DIP204/ \
  $(BRDS_PATH)/EVK1100/ \
  $(APPS_PATH)/EVK1100-CONTROl-PANEL/USB/APPLI/ \
  $(COMP_PATH)/JOYSTICK/SKRHABE010/ \
  $(COMP_PATH)/MEMORY/SD_MMC/SD_MMC_SPI/ \
  $(APPS_PATH)/EVK1100-CONTROl-PANEL/ \
  $(UTIL_PATH)/LIBS/NEWLIB_ADDONS/INCLUDE/ \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/CONF/ \
  $(SERV_PATH)/USB/CLASS/CDC/ \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/ \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/ENUM/DEVICE/ \
  $(DRVR_PATH)/USBB/ \
  $(DRVR_PATH)/USBB/ENUM/ \
  $(DRVR_PATH)/USBB/ENUM/DEVICE/ \
  $(DRVR_PATH)/USBB/ENUM/HOST/ \
  $(SERV_PATH)/USB/ \
  $(SERV_PATH)/FAT/ \
  $(SERV_PATH)/MEMORY/CTRL_ACCESS/ \
  
# C source files
CSRCS = \
  $(DRVR_PATH)/SPI/spi.c \
  $(DRVR_PATH)/INTC/intc.c \
  $(DRVR_PATH)/PM/pm.c \
  $(DRVR_PATH)/PM/pm_conf_clocks.c \
  $(DRVR_PATH)/FLASHC/flashc.c \
  $(DRVR_PATH)/GPIO/gpio.c \
  $(DRVR_PATH)/PWM/pwm.c \
  $(UTIL_PATH)/DEBUG/print_funcs.c \
  $(DRVR_PATH)/USART/usart.c \
  $(SERV_PATH)/DELAY/delay.c \
  $(DRVR_PATH)/ADC/adc.c \
  $(DRVR_PATH)/TC/tc.c \
  $(DRVR_PATH)/PM/power_clocks_lib.c \
  $(COMP_PATH)/MEMORY/SD_MMC/SD_MMC_SPI/sd_mmc_spi.c \
  $(COMP_PATH)/MEMORY/SD_MMC/SD_MMC_SPI/sd_mmc_spi_mem.c \
  $(BRDS_PATH)/EVK1100/led.c \
  $(DRVR_PATH)/USBB/usb_drv.c \
  $(COMP_PATH)/DISPLAY/DIP204/dip204.c \
  $(DRVR_PATH)/USBB/ENUM/usb_task.c \
  $(DRVR_PATH)/USBB/ENUM/DEVICE/usb_device_task.c \
  $(DRVR_PATH)/USBB/ENUM/DEVICE/usb_standard_request.c \
  $(DRVR_PATH)/USBB/ENUM/HOST/usb_host_task.c \
  $(DRVR_PATH)/USBB/ENUM/HOST/usb_host_enum.c \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/device_cdc_task.c \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/host_cdc_task.c \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/uart_usb_lib.c \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/ENUM/DEVICE/usb_descriptors.c \
  $(SERV_PATH)/USB/CLASS/CDC/EXAMPLES/ENUM/DEVICE/usb_specific_request.c \
  $(SERV_PATH)/FAT/fat.c \
  $(SERV_PATH)/FAT/file.c \
  $(SERV_PATH)/FAT/fat_unusual.c \
  $(SERV_PATH)/FAT/navigation.c \
  $(SERV_PATH)/MEMORY/CTRL_ACCESS/ctrl_access.c \
   ../../main.c

# Assembler source files
ASSRCS = \
  $(SERV_PATH)/USB/CLASS/DFU/EXAMPLES/ISP/BOOT/trampoline.S \
  $(DRVR_PATH)/INTC/exception.S

# Library path
LIB_PATH = $(UTIL_PATH)/LIBS/NEWLIB_ADDONS/AT32UCR2/

# Libraries to link with the project
LIBS = newlib_addons-at32ucr2-debug

# Linker script file if any
LINKER_SCRIPT = $(UTIL_PATH)/LINKER_SCRIPTS/AT32UC3A/0512/GCC/link_uc3a0512.lds

# Options to request or suppress warnings: [-fsyntax-only] [-pedantic[-errors]] [-w] [-Wwarning...]
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
WARNINGS = -Wall

# Options for debugging: [-g]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
DEBUG = -g

# Options that control optimization: [-O[0|1|2|3|s]]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
OPTIMIZATION = -O0 -ffunction-sections -fdata-sections

# Extra flags to use when preprocessing
CPP_EXTRA_FLAGS =

# Extra flags to use when compiling
C_EXTRA_FLAGS =

# Extra flags to use when assembling
AS_EXTRA_FLAGS =

# Extra flags to use when linking
LD_EXTRA_FLAGS = -Wl,--gc-sections -Wl,-e,_trampoline

# Documentation path
DOC_PATH = \
  ../../DOC/

# Documentation configuration file
DOC_CFG = \
  ../doxyfile.doxygen