/**
 * @file adrv9002_hal.c
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief Hardware Abstraction Layer (HAL) implementation for ADRV9002 device
 *
 * This file implements the HAL interface required by the ADRV9002 device
 * driver. It provides SPI communication, GPIO control, and device
 * initialization functions. The HAL layer abstracts the hardware-specific
 * details from the core driver.
 *
 * Key functionality:
 * - SPI read/write operations
 * - Reset pin control via GPIO
 * - Device initialization and cleanup
 * - HAL context management
 * @version 0.1
 * @date 2025-11-13
 * @copyright Copyright (c) 2025
 *
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/spi/spi.h>
#include <linux/atomic.h>

#include "adi_adrv9001_hal.h"
#include "adrv9002.h"

static atomic_t hal_user_count = ATOMIC_INIT(0);

static int32_t adrv9002_hal_spi_write(void *devHalCfg, const uint8_t txData[],
                                      uint32_t numTxBytes) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)devHalCfg;
  struct spi_device *spi;
  int ret = 0;

  if (!hal_ctx || !txData || !numTxBytes) {
    pr_err("adrv9002_hal: spi_write called with NULL hal_ctx\n");
    return -EINVAL;
  }

  spi = hal_ctx->spi;
  if (!spi) {
    pr_err("adrv9002_hal: spi device is NULL\n");
    return -EINVAL;
  }

  ret = spi_write(spi, txData, numTxBytes);
  if (ret < 0) {
    dev_err(&spi->dev, "spi_write() failed: %d\n", ret);
    return ret;
  }

  return 0;
}

static int32_t adrv9002_hal_spi_read(void *devHalCfg, const uint8_t txData[],
                                     uint8_t rxData[], uint32_t numRxBytes) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)devHalCfg;
  struct spi_device *spi;
  struct spi_transfer xfer = {0};
  struct spi_message msg;
  int ret;

  if (!hal_ctx || !txData || !rxData || numRxBytes == 0) {
    pr_err("adrv9002_hal: spi_read called with NULL parameter\n");
    return -EINVAL;
  }

  spi = hal_ctx->spi;
  if (!spi) {
    pr_err("adrv9002_hal: spi device is NULL\n");
    return -EINVAL;
  }

  spi_message_init(&msg);

  xfer.tx_buf = txData;
  xfer.rx_buf = rxData;
  xfer.len = numRxBytes;
  xfer.speed_hz = 32000000;
  xfer.bits_per_word = 8;

  spi_message_add_tail(&xfer, &msg);

  ret = spi_sync(spi, &msg);
  if (ret < 0) {
    dev_err(&spi->dev, "SPI read failed: %d\n", ret);
    return ret;
  }

    dev_dbg(&spi->dev, "SPI read: tx[0]=0x%02x rx[0]=0x%02x rx[1]=0x%02x\n",
      txData[0], rxData[0], rxData[1]);

  return 0;
}

int adrv9002_reset_assert(struct adrv9002_reset_config *cfg) {
  u32 val;

  switch (cfg->type) {
  case RESET_TYPE_NONE:
    /* No reset pin, nothing to do */
    return 0;

  case RESET_TYPE_GPIO:
    gpiod_set_value(cfg->u.gpio, 1);
    usleep_range(1000, 2000);
    break;

  case RESET_TYPE_REGISTER:
    val = readl(cfg->u.reg.base);
    /* active reset */
    if (cfg->u.reg.reset_val & cfg->u.reg.mask) {
      val |= (cfg->u.reg.reset_val) & cfg->u.reg.mask;
    } else {
      val &= (cfg->u.reg.reset_val & cfg->u.reg.mask);
    }
    writel(val, cfg->u.reg.base);
    usleep_range(1000, 2000);
    break;

  default:
    return -EINVAL;
  }

  return 0;
}

int adrv9002_reset_deassert(struct adrv9002_reset_config *cfg) {
  u32 val;

  switch (cfg->type) {
  case RESET_TYPE_NONE:
    /* No reset pin, nothing to do */
    return 0;

  case RESET_TYPE_GPIO:
    gpiod_set_value(cfg->u.gpio, 0);
    usleep_range(1000, 2000);
    break;

  case RESET_TYPE_REGISTER:
    val = readl(cfg->u.reg.base);
    /* active reset */
    if (cfg->u.reg.unreset_val & cfg->u.reg.mask) {
      val |= (cfg->u.reg.unreset_val) & cfg->u.reg.mask;
    } else {
      val &= (cfg->u.reg.unreset_val) & cfg->u.reg.mask;
    }
    writel(val, cfg->u.reg.base);
    usleep_range(1000, 2000);
    break;

  default:
    return -EINVAL;
  }

  return 0;
}

int32_t adrv9002_hal_reset_pin_set(void *devHalCfg, uint8_t pinLevel) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)devHalCfg;
  struct adrv9002_reset_config *reset_config;
  int ret;

  if (!hal_ctx) {
    pr_err("adrv9002_hal: reset_pin_set called with NULL hal_ctx\n");
    return -EINVAL;
  }

  reset_config = hal_ctx->reset_config;
  if (!reset_config) {
    pr_err("adrv9002_hal: reset_config is NULL\n");
    return -EINVAL;
  }

  if (!pinLevel) {
    ret = adrv9002_reset_assert(reset_config);
  } else {
    ret = adrv9002_reset_deassert(reset_config);
  }

  return ret;
}

static int32_t adrv9002_hal_open(void *devHalCfg) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)devHalCfg;

  if (!hal_ctx) {
    pr_err("adrv9002_hal: hal_open called with NULL hal_ctx\n");
    return -EINVAL;
  }

  if (!hal_ctx->spi) {
    pr_err("adrv9002_hal: hal_ctx->spi is NULL in hal_open\n");
    return -EINVAL;
  }

  if (!hal_ctx->spi->dev.parent) {
    pr_err("adrv9002_hal: spi->dev.parent is NULL (corrupted device structure)\n");
    return -EINVAL;
  }

  dev_info(&hal_ctx->spi->dev, "ADRV9002 HAL opened\n");
  return 0;
}

static int32_t adrv9002_hal_close(void *devHalCfg) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)devHalCfg;

  if (!hal_ctx) {
    pr_err("adrv9002_hal: hal_close called with NULL hal_ctx\n");
    return -EINVAL;
  }

  if (!hal_ctx->spi) {
    pr_err("adrv9002_hal: hal_ctx->spi is NULL in hal_close\n");
    return -EINVAL;
  }

  if (!hal_ctx->spi->dev.parent) {
    pr_err("adrv9002_hal: spi->dev.parent is NULL (corrupted device structure)\n");
    return -EINVAL;
  }

  dev_info(&hal_ctx->spi->dev, "ADRV9002 HAL closed\n");
  return 0;
}

int adrv9002_hal_init(struct adrv9002_priv *priv, struct spi_device *spi,
                      struct adrv9002_reset_config *reset) {
  struct adrv9002_hal_context *hal_ctx;

  hal_ctx = kzalloc(sizeof(*hal_ctx), GFP_KERNEL);
  if (!hal_ctx) {
    return -ENOMEM;
  }

  hal_ctx->spi = spi;
  hal_ctx->reset_config = reset;
  hal_ctx->priv = priv;

  if (atomic_inc_return(&hal_user_count) == 1) {
    adi_hal_SpiWrite = adrv9002_hal_spi_write;
    adi_hal_SpiRead = adrv9002_hal_spi_read;
    adi_adrv9001_hal_resetbPin_set = adrv9002_hal_reset_pin_set;
    adi_adrv9001_hal_open = adrv9002_hal_open;
    adi_adrv9001_hal_close = adrv9002_hal_close;
  }

  priv->hal_context = hal_ctx;

  dev_info(&spi->dev, "ADRV9002 HAL initialized\n");
  return 0;
}
EXPORT_SYMBOL(adrv9002_hal_init);

void adrv9002_hal_deinit(struct adrv9002_priv *priv) {
  if (priv && priv->hal_context) {
    kfree(priv->hal_context);
    priv->hal_context = NULL;
  }

  if (atomic_dec_and_test(&hal_user_count)) {
    adi_hal_SpiWrite = NULL;
    adi_hal_SpiRead = NULL;
    adi_adrv9001_hal_resetbPin_set = NULL;
    adi_adrv9001_hal_open = NULL;
    adi_adrv9001_hal_close = NULL;
  }
}
EXPORT_SYMBOL(adrv9002_hal_deinit);
