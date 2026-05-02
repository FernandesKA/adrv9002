# SPDX-License-Identifier: GPL-2.0

# Multiple source files
MODULE_NAME := adrv9002

KERNEL_SRC ?= $(KERNELDIR)
CROSS_COMPILE ?= $(CC)
ARCH ?= arm
EXTRA_CFLAGS += -O2 -g0
# Suppress mixed declarations warnings across all sources (incl. subdirs)
EXTRA_CFLAGS += -Wno-declaration-after-statement
EXTRA_CFLAGS += -DADI_COMPILED_LOGLEVEL=ADI_LOGLEVEL_WARN

# Remove any debug flags injected by the kernel config when building this module
KBUILD_CFLAGS := $(filter-out -g%,$(KBUILD_CFLAGS))
KBUILD_CFLAGS_MODULE := $(filter-out -g%,$(KBUILD_CFLAGS_MODULE))

ifeq ($(KERNEL_SRC),)
    KERNEL_SRC := /lib/modules/$(shell uname -r)/build
endif

PWD := $(shell pwd)

adrv9002_drv-y += adrv9002.o
adrv9002_drv-y += adrv9002_radio.o
adrv9002_drv-y += adrv9002_hal.o
adrv9002_drv-y += lna.o
adrv9002_drv-y += platforms/linux_platform.o
adrv9002_drv-y += third_party/jsmn/jsmn.o
adrv9002_drv-y += common/adi_common_log.o
adrv9002_drv-y += common/adi_common_error.o
adrv9002_drv-y += common/adi_common_hal_wrapper.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_validators.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_rx.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_crc32.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_arm.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_bf_hal.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_init.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_gpio.o
adrv9002_drv-y += devices/adrv9001/private/src/adrv9001_radio.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_common.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_tx.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_arm.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_bbdc.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_gpio.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_auxdac.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_utilities.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_cals.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_auxadc.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_radio.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_mcs.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_ssi.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_rx_gaincontrol.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_stream.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_spi.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_rx.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_orx.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_dpd.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_fh.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_powermanagement.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_powersavingandmonitormode.o
adrv9002_drv-y += devices/adrv9001/public/src/adi_adrv9001_profileutil.o
adrv9002_drv-y += tes/begin_receiving.o
adrv9002_drv-y += tes/begin_transmitting.o
adrv9002_drv-y += tes/begin_transmitting_iq_data2.o
adrv9002_drv-y += tes/begin_transmitting_iq_data4.o
adrv9002_drv-y += tes/calibrate.o
adrv9002_drv-y += tes/calibrategainTableRows8.o
adrv9002_drv-y += tes/calibrategainTableRows10.o
adrv9002_drv-y += tes/configure.o
adrv9002_drv-y += tes/configureagcCfg3.o
adrv9002_drv-y += tes/dataCaptureiData1.o
adrv9002_drv-y += tes/dataCaptureiData3.o
adrv9002_drv-y += tes/dataCaptureqData1.o
adrv9002_drv-y += tes/dataCaptureqData3.o
adrv9002_drv-y += tes/initialize.o
adrv9002_drv-y += tes/initializeattenTableRows18.o
adrv9002_drv-y += tes/initializebinary10.o
adrv9002_drv-y += tes/initializebinary11.o
adrv9002_drv-y += tes/initializegainTableRows14.o
adrv9002_drv-y += tes/initializegainTableRows15.o
adrv9002_drv-y += tes/initializegainTableRows16.o
adrv9002_drv-y += tes/initializegainTableRows17.o
adrv9002_drv-y += tes/initializegpioCtrlInitCfg26.o
adrv9002_drv-y += tes/prime.o
adrv9002_drv-y += tes/stop_receiving.o
adrv9002_drv-y += tes/stop_transmitting.o

obj-m += adrv9002_drv.o

ccflags-y += -I$(src)/devices/adrv9001/private/include \
	-I$(src)/devices/adrv9001/private/include/bitfields/c0 \
	-I$(src)/devices/adrv9001/public/include  \
	-I$(src)/common/ \
	-I$(src)/platforms/ \
	-I$(src)/third_party/jsmn/ \
	-I$(src)/third_party/adi_pmag_macros/ \
	-DADI_DYNAMIC_PROFILE_LOAD

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) \
		modules_install

.PHONY: all clean install
