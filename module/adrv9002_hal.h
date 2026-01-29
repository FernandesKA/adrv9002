/**
 * @file adrv9002_hal.h
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief 
 * @version 0.1
 * @date 2025-11-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ADRV9002_HAL_H
#define ADRV9002_HAL_H

#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include "adrv9002.h"


struct adrv9002_reset_config {
    enum {
        RESET_TYPE_NONE = 0,
        RESET_TYPE_GPIO,
        RESET_TYPE_REGISTER
    } type;

    union {
        struct gpio_desc *gpio;
        struct {
            void __iomem *base;
            u32 mask;
            u32 reset_val;
            u32 unreset_val;
        } reg;
    } u;
};

struct adrv9002_hal_context
{
   struct spi_device *spi;
   struct adrv9002_reset_config *reset_config;
   struct adrv9002_priv *priv;
};

struct adrv9002_priv;

int adrv9002_hal_init(struct adrv9002_priv *priv, struct spi_device *spi, struct adrv9002_reset_config *reset);

void adrv9002_hal_deinit(struct adrv9002_priv *priv);

int32_t adrv9002_hal_reset_pin_set(void *devHalCfg, uint8_t pinLevel);

int adrv9002_reset_assert(struct adrv9002_reset_config *cfg);

int adrv9002_reset_deassert(struct adrv9002_reset_config *cfg);

#endif // ADRV9002_HAL_H

