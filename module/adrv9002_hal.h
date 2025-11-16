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

struct adrv9002_priv;

int adrv9002_hal_init(struct adrv9002_priv *priv, struct spi_device *spi, struct gpio_desc *gpio);

void adrv9002_hal_deinit(struct adrv9002_priv *priv);

#endif // ADRV9002_HAL_H

