/**
 * @file adrv9002_hal.c
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include "adi_adrv9001_hal.h"
#include "adrv9002.h"

struct adrv9002_hal_context
{
   struct spi_device *spi;
   struct gpio_desc *reset_gpio;
   struct adrv9002_priv *priv;
};

static int32_t adrv9002_hal_spi_write(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes)
{
   struct adrv9002_hal_context *hal_ctx = (struct adrv9002_hal_context *)devHalCfg;
   struct spi_device *spi = hal_ctx->spi;
   int ret = 0;

   if (!spi || !txData || !numTxBytes)
   {
      return -EINVAL;
   }

   ret = spi_write(spi, txData, numTxBytes);
   if (ret < 0)
   {
      dev_err(&spi->dev, "spi_write() failed: %d\n", ret);
      return ret;
   }

   return 0;
}

static int32_t adrv9002_hal_spi_read(void *devHalCfg,
                                      const uint8_t txData[],
                                      uint8_t rxData[],
                                      uint32_t numRxBytes)
{
    struct adrv9002_hal_context *hal_ctx = (struct adrv9002_hal_context *)devHalCfg;
    struct spi_device *spi = hal_ctx->spi;
    struct spi_transfer xfer = {0};
    struct spi_message msg;
    int ret;

    if (!spi || !txData || !rxData || numRxBytes == 0)
        return -EINVAL;

    // Используем ОДИН full-duplex transfer вместо двух
    spi_message_init(&msg);
    
    xfer.tx_buf = txData;
    xfer.rx_buf = rxData;
    xfer.len = numRxBytes;
    xfer.speed_hz = 32000000;  // Как в userspace
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


static int32_t adrv9002_hal_reset_pin_set(void *devHalCfg, uint8_t pinLevel)
{
   struct adrv9002_hal_context *hal_ctx = (struct adrv9002_hal_context *)devHalCfg;
   struct gpio_desc *reset_gpio = hal_ctx->reset_gpio;

   if (!reset_gpio)
   {
      dev_warn(&hal_ctx->spi->dev, "Reset GPIO not set\n");
      return 0;
   }

   gpiod_set_value(reset_gpio, pinLevel);

   if (pinLevel == 0)
   {
      usleep_range(1000, 2000); // 1-2 ms low
   }
   else
   {
      usleep_range(10000, 15000); // 10-15 ms high
   }

   return 0;
}

static int32_t adrv9002_hal_open(void *devHalCfg)
{
   struct adrv9002_hal_context *hal_ctx = (struct adrv9002_hal_context *)devHalCfg;

   if (!hal_ctx || !hal_ctx->spi)
   {
      return -EINVAL;
   }

   dev_info(&hal_ctx->spi->dev, "ADRV9002 HAL opened\n");
   return 0;
}

static int32_t adrv9002_hal_close(void *devHalCfg)
{
   struct adrv9002_hal_context *hal_ctx = (struct adrv9002_hal_context *)devHalCfg;

   if (!hal_ctx)
   {
      return -EINVAL;
   }

   dev_info(&hal_ctx->spi->dev, "ADRV9002 HAL closed\n");
   return 0;
}

int adrv9002_hal_init(struct adrv9002_priv *priv,
                      struct spi_device *spi,
                      struct gpio_desc *reset_gpio)
{
   struct adrv9002_hal_context *hal_ctx;

   hal_ctx = kzalloc(sizeof(*hal_ctx), GFP_KERNEL);
   if (!hal_ctx)
   {
      return -ENOMEM;
   }

   hal_ctx->spi = spi;
   hal_ctx->reset_gpio = reset_gpio;
   hal_ctx->priv = priv;

   adi_hal_SpiWrite = adrv9002_hal_spi_write;
   adi_hal_SpiRead = adrv9002_hal_spi_read;
   adi_adrv9001_hal_resetbPin_set = adrv9002_hal_reset_pin_set;
   adi_adrv9001_hal_open = adrv9002_hal_open;
   adi_adrv9001_hal_close = adrv9002_hal_close;

   priv->hal_context = hal_ctx;

   dev_info(&spi->dev, "ADRV9002 HAL initialized\n");
   return 0;
}
EXPORT_SYMBOL(adrv9002_hal_init);

void adrv9002_hal_deinit(struct adrv9002_priv *priv)
{
   if (priv && priv->hal_context)
   {
      kfree(priv->hal_context);
      priv->hal_context = NULL;
   }

   adi_hal_SpiWrite = NULL;
   adi_hal_SpiRead = NULL;
   adi_adrv9001_hal_resetbPin_set = NULL;
   adi_adrv9001_hal_open = NULL;
   adi_adrv9001_hal_close = NULL;
}
EXPORT_SYMBOL(adrv9002_hal_deinit);