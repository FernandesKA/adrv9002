/**
 * @file adrv9002.h
 * @author FernandesKA (i@kfernandes.ru)
 * @brief 
 * @version 0.1
 * @date 2025-11-07
 * 
 * 
 */

 #ifndef ADRV9002_H
 #define ADRV9002_H

#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_profileutil.h"
#include "adi_common_error.h"
#include "adi_common_hal.h"

struct spi_dev {
    struct spi_device *spi;
};

struct adrv9002_priv
{
    struct spi_device *spi;
    struct cdev cdev;
    dev_t devt;
    struct class *class;
    struct device *device;
    struct spi_dev spi_dev;
    adi_common_ApiVersion_t api_version;
    adi_adrv9001_ArmVersion_t arm_version;
    adi_adrv9001_SiliconVersion_t silicon_version;
    adi_adrv9001_Device_t *adrv9001Device;
    adi_adrv9001_Init_t *adrv9001Init;
    /*gpio*/
    struct gpio_desc *reset_gpio;
    /*state tracking*/
    bool initialized;
    u64 lo_freq[2];
    u32 gain[2];
    struct mutex lock;
};
 #endif // ADRV9002_H
