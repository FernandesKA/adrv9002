#!/bin/sh

sed -i '/#include "adi_fpga9001.h"/d' ./*.c
sed -i '/#include "adi_fpga9001_clock.h"/d' ./*.c
sed -i '/#include "adi_fpga9001_hal_wrapper.h"/d' ./*.c
sed -i '/#include "adi_fpga9001_ssi.h"/d' ./*.c
sed -i '/#include "linux_uio_init.h"/d' ./*.c
sed -i '/#include "adi_fpga9001_tdd.h"/d' ./*.c
sed -i '/#include "adi_fpga9001_datachain.h"/d' ./*.c

sed -i '/#include "adi_fpga9001.h"/d' ./*.h
sed -i '/#include "adi_fpga9001_clock.h"/d' ./*.h
sed -i '/#include "adi_fpga9001_hal_wrapper.h"/d' ./*.h
sed -i '/#include "adi_fpga9001_ssi.h"/d' ./*.h
sed -i '/#include "linux_uio_init.h"/d' ./*.h
sed -i '/#include "adi_fpga9001_tdd.h"/d' ./*.h
sed -i '/#include "adi_fpga9001_datachain.h"/d' ./*.h

sed -i 's/, adi_fpga9001_Device_t \* fpga9001Device_0//' ./*.c
sed -i 's/, adi_fpga9001_Device_t \* fpga9001Device_0//' ./*.h

sed -i 's/adi_fpga9001_Device_t \* fpga9001Device_0, //' ./*.c
sed -i 's/adi_fpga9001_Device_t \* fpga9001Device_0, //' ./*.h

sed -i '/fpga9001Device_0/,/AUTOGENERATOR_ERROR_HANDLER(error_code);/d' ./*.c
sed -i '/fpga9001Device_0/,/AUTOGENERATOR_ERROR_HANDLER(error_code);/d' ./*.h

sed -i '/adi_fpga9001_Version_t/,/AUTOGENERATOR_ERROR_HANDLER(error_code);/d' ./*.c
sed -i '/adi_fpga9001_SsiCalibrationCfg_t/,/}};/d' ./*.c
sed -i '/adi_fpga9001_ClockStatus_t/,/};/d' ./*.c
sed -i '/adi_fpga9001_DmaCfg_t/,/};/d' ./*.c

#sed -i 's/ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED/ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_CMOS_STROBE_IN_N_ENABLED/' ./initializeinit7.c

rm main.c
rm dataCapture.c
rm dataCapture.h

#sed -i 's/\.chanInitCalMask = .*/\.chanInitCalMask = ADI_ADRV9001_INIT_CAL_SYSTEM_ALL,/' ./calibrate.c

