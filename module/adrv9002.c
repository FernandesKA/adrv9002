/**
 * @file adrv9002.c
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/version.h>

#include <linux/spi/spi.h>
#include "adrv9002.h"
#include "adrv9002_ioctl.h"

#include "tes/initialize.h"

#define DRIVER_NAME "adrv9002"
#define DRIVER_VERSION "0.1.0"
/*private device structure*/

struct adrv9002_priv *g_priv;

/* ============================================================================
 * Hardware control functions
 * ============================================================================ */

static void adrv9002_hw_reset(struct adrv9002_priv *priv)
{

    dev_dbg(&priv->spi->dev, "DIRTY HACK! Write into regs PL\n");
    void __iomem *regs = ioremap(0x43c00000, 4);
    writel(0x00u, regs);
    writel(0x03u, regs);
    iounmap(regs);

   dev_dbg(&priv->spi->dev, "DIRTY HACK! Write into regs PL ok. TODO: Need to remove\n");

    if (!priv->reset_gpio)
        return;

    dev_dbg(&priv->spi->dev, "Resetting hardware\n");

    gpiod_set_value_cansleep(priv->reset_gpio, 1);
    usleep_range(5000, 6000);
    gpiod_set_value_cansleep(priv->reset_gpio, 0);
    usleep_range(5000, 6000);
}


static int spi_dev_init(struct spi_dev *dev, struct spi_device *spi)
{
    if (!dev || !spi)
    {
        return -EINVAL;
    }

    dev->spi = spi;
    return 0;
}

static int spi_dev_send(struct spi_dev *dev, const void *buf, size_t len)
{
    struct spi_transfer transfer = {
        .tx_buf = buf,
        .len = len,
    };

    struct spi_message msg;
    int ret;

    spi_message_init(&msg);
    spi_message_add_tail(&transfer, &msg);

    ret = spi_sync(dev->spi, &msg);
    if (ret < 0)
    {
        dev_err(&dev->spi->dev, "spi_sync failed: %d\n", ret);
    }

    return ret;
}

static int spi_dev_recv(struct spi_dev *dev, void *buf, size_t len)
{
    struct spi_transfer transfer = {
        .rx_buf = buf,
        .len = len,
    };

    struct spi_message msg;
    int ret;

    spi_message_init(&msg);
    spi_message_add_tail(&transfer, &msg);

    ret = spi_sync(dev->spi, &msg);
    if (ret < 0)
    {
        dev_err(&dev->spi->dev, "spi_sync failed: %d\n", ret);
    }

    return ret;
}

static int spi_dev_transfer(struct spi_dev *dev, const void *tx_buf, void *rx_buf, size_t len)
{
    struct spi_transfer transfer = {
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len = len,
    };

    struct spi_message msg;
    int ret;

    spi_message_init(&msg);
    spi_message_add_tail(&transfer, &msg);

    ret = spi_sync(dev->spi, &msg);
    if (ret < 0)
    {
        dev_err(&dev->spi->dev, "spi_sync failed: %d\n", ret);
    }

    return ret;
}

static int spi_dev_transfer_buf(struct spi_dev *dev, u8 *tx_data, u8 *rx_data, size_t len)
{
    return spi_dev_transfer(dev, (const void *)tx_data, (void *)rx_data, len);
}


static int adrv9002_spi_write_cmd(struct adrv9002_priv *priv, const u8 *buf, size_t len) {
    return spi_dev_send(&priv->spi_dev, buf, len);
}

static int adrv9002_spi_read_data(struct adrv9002_priv *priv, u8 *buf, size_t len) {
    return spi_dev_recv(&priv->spi_dev, buf, len);
}

static int adrv9002_spi_xfer(struct adrv9002_priv *priv, const u8 *tx, u8 *rx, size_t len) {
    return spi_dev_transfer(&priv->spi_dev, tx, rx, len);
}

/* ============================================================================
 * ADRV9002 initialization sequence
 * ============================================================================ */
static int adrv9002_do_init(struct adrv9002_priv *priv,
                            struct adrv9002_init_params *params)
{

    err_free_profile:
    int ret =  0;
    dev_info(&priv->spi->dev, "Starting initialization...\n");

    if (!priv) {
        printk(KERN_ERR "priv is NULL\n");
        return -EINVAL;
    }

    priv->adrv9001Device = kzalloc(sizeof(adi_adrv9001_Device_t), GFP_KERNEL);
    if (!priv->adrv9001Device) {
        printk(KERN_ERR "adrv9001Device is NULL\n");
        return -EINVAL;
    }
    
    priv->adrv9001Init = kzalloc(sizeof(adi_adrv9001_Init_t), GFP_KERNEL);
    if (!priv->adrv9001Init) {
        printk(KERN_ERR "adrv9001Init is NULL\n");
        return -EINVAL;
    }

    printk(KERN_ERR "we are here\n");

    // dev_info(&priv->spi->dev, "Make reset\n");
    // adrv9002_hw_reset(priv);

    /* TODO: Load profile JSON */
    dev_info(&priv->spi->dev, "Begin profile parsing \n");
    ret = adi_adrv9001_profileutil_Parse(priv->adrv9001Device, priv->adrv9001Init, params->profile_buf, params->profile_buf_len);

    if (ret) {
        dev_err(&priv->spi->dev, "Profile parsing failed: %d\n", ret);
        return ret;
    }

    if (priv->hal_context) {
        priv->adrv9001Device->common.devHalInfo = priv->hal_context;
    } else {
        dev_err(&priv->spi->dev, "HAL context is NULL\n");
        return -EINVAL;
    }

    dev_err(&priv->spi->dev, "DIRTY HACK! Write into regs PL\n");
    void __iomem *regs = ioremap(0x43c00000, 4);
    writel(0x03u, regs);
    iounmap(regs);
    dev_err(&priv->spi->dev, "DIRTY HACK! Write into regs PL ok. TODO: Need to remove\n");

    ret = initialize(priv->adrv9001Device, priv->adrv9001Init, params->stream_buf, params->stream_buf_len);
    if (ret) {
        dev_err(&priv->spi->dev, "Initialization failed: %d\n", ret);
        return ret;
    }
    /* TODO: Load stream binary */
    /* ret = adrv9002_load_stream(priv, params->stream_path); */

    /* TODO: Vendor API initialization */
    /* ret = adi_adrv9001_Initialize(...); */

    /* TODO: ARM start check */
    /* ret = adi_adrv9001_arm_StartStatusCheck(...); */

    /* TODO: Calibration */
    /* ret = adi_adrv9001_Calibrate(...); */

    /* TODO: Configure */
    /* ret = adi_adrv9001_Configure(...); */

    /* TODO: Prime */
    /* ret = adi_adrv9001_Prime(...); */

    if (ret)
    {
        dev_err(&priv->spi->dev, "Initialization failed: %d\n", ret);
        return ret;
    }

    priv->initialized = true;
    dev_info(&priv->spi->dev, "Initialization complete\n");

    return 0;
}

/* ============================================================================
 * Runtime configuration functions
 * ============================================================================ */

static int adrv9002_set_frequency(struct adrv9002_priv *priv,
                                  struct adrv9002_freq_params *freq)
{
    int ret = 0;

    if (freq->channel >= 2)
    {
        dev_err(&priv->spi->dev, "Invalid channel: %u\n", freq->channel);
        return -EINVAL;
    }

    dev_info(&priv->spi->dev, "Set freq ch%u port%u: %llu Hz\n",
             freq->channel, freq->port, freq->freq_hz);

    /* TODO: Vendor API call */
    /* ret = adi_adrv9001_Radio_Carrier_Configure(...); */

    if (ret == 0)
        priv->lo_freq[freq->channel] = freq->freq_hz;

    return ret;
}

static int adrv9002_get_frequency(struct adrv9002_priv *priv,
                                  struct adrv9002_freq_params *freq)
{
    if (freq->channel >= 2)
        return -EINVAL;

    /* Return cached value */
    freq->freq_hz = priv->lo_freq[freq->channel];

    return 0;
}

static int adrv9002_set_gain(struct adrv9002_priv *priv,
                             struct adrv9002_gain_params *gain)
{
    int ret = 0;

    if (gain->channel >= 2)
    {
        dev_err(&priv->spi->dev, "Invalid channel: %u\n", gain->channel);
        return -EINVAL;
    }

    dev_info(&priv->spi->dev, "Set gain ch%u: %u\n",
             gain->channel, gain->gain);

    /* TODO: Vendor API call */
    /* ret = adi_adrv9001_Rx_Gain_Set(...); */

    if (ret == 0)
        priv->gain[gain->channel] = gain->gain;

    return ret;
}

static int adrv9002_enable_rx(struct adrv9002_priv *priv,
                              struct adrv9002_channel_ctrl *ctrl)
{
    int ret = 0;

    if (ctrl->channel >= 2)
        return -EINVAL;

    dev_info(&priv->spi->dev, "RX ch%u: %s\n",
             ctrl->channel, ctrl->enable ? "enable" : "disable");

    /* TODO: Vendor API call */
    /* ret = adi_adrv9001_Radio_Channel_EnableRf(...); */

    return ret;
}

static int adrv9002_enable_tx(struct adrv9002_priv *priv,
                              struct adrv9002_channel_ctrl *ctrl)
{
    int ret = 0;

    if (ctrl->channel >= 2)
        return -EINVAL;

    dev_info(&priv->spi->dev, "TX ch%u: %s\n",
             ctrl->channel, ctrl->enable ? "enable" : "disable");

    /* TODO: Vendor API call */
    /* ret = adi_adrv9001_Radio_Channel_EnableRf(...); */

    return ret;
}

/* ============================================================================
 * Character device operations
 * ============================================================================ */

static int adrv9002_open(struct inode *inode, struct file *filp)
{
    struct adrv9002_priv *priv;

    priv = container_of(inode->i_cdev, struct adrv9002_priv, cdev);
    filp->private_data = priv;

    dev_dbg(&priv->spi->dev, "Device opened\n");
    return 0;
}

static int adrv9002_release(struct inode *inode, struct file *filp)
{
    struct adrv9002_priv *priv = filp->private_data;

    dev_dbg(&priv->spi->dev, "Device closed\n");
    return 0;
}

static long adrv9002_ioctl(struct file *filp, unsigned int cmd,
                           unsigned long arg)
{
    struct adrv9002_priv *priv = filp->private_data;
    int ret = 0;

    /* Validate ioctl command */
    if (_IOC_TYPE(cmd) != ADRV9002_IOC_MAGIC)
    {
        dev_err(&priv->spi->dev, "Invalid ioctl magic: 0x%x\n",
                _IOC_TYPE(cmd));
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > ADRV9002_IOC_MAXNR)
    {
        dev_err(&priv->spi->dev, "Invalid ioctl number: %u\n",
                _IOC_NR(cmd));
        return -ENOTTY;
    }

    mutex_lock(&priv->lock);

    switch (cmd)
    {
    case ADRV9002_IOC_INIT:
    {
        struct adrv9002_init_params params;

        if (priv->initialized)
        {
            dev_warn(&priv->spi->dev, "Already initialized\n");
            ret = -EBUSY;
            break;
        }

        if (copy_from_user(&params, (void __user *)arg, sizeof(params)))
        {
            ret = -EFAULT;
            break;
        }

        if (!params.profile_buf || params.profile_buf_len <= 0) {
            dev_err(&priv->spi->dev, "Profile buffer is null\n");
            ret = -EINVAL;
            break;
        }

        if (!params.stream_buf || params.stream_buf_len <= 0) {
            dev_err(&priv->spi->dev, "Stream buffer is null\n");
            ret = -EINVAL;
            break;
        }

        char *kernel_profile_buf = kmalloc(params.profile_buf_len, GFP_KERNEL);
        if (!kernel_profile_buf) {
            dev_err(&priv->spi->dev, "Failed to allocate profile buffer\n");
            ret = -ENOMEM;
            break;
        }

        char *kernel_stream_buf = kmalloc(params.stream_buf_len, GFP_KERNEL);
        if (!kernel_stream_buf) {
            dev_err(&priv->spi->dev, "Failed to allocate stream buffer\n");
            goto err_free_profile;
            ret = -ENOMEM;
            break;
        }

        if (copy_from_user(kernel_profile_buf, params.profile_buf, params.profile_buf_len)) {
            dev_err(&priv->spi->dev, "Failed to copy profile buffer\n");
            goto err_free_all;
            ret = -EFAULT;
            break;
        }

        if (copy_from_user(kernel_stream_buf, params.stream_buf, params.stream_buf_len)) {
            dev_err(&priv->spi->dev, "Failed to copy stream buffer\n");
            goto err_free_all;
            ret = -EFAULT;
            break;
        }

        params.profile_buf = kernel_profile_buf;
        params.stream_buf = kernel_stream_buf;

        ret = adrv9002_do_init(priv, &params);

        err_free_all:
        kfree(kernel_stream_buf);
        err_free_profile:
        kfree(kernel_profile_buf);
        break;
    }

    case ADRV9002_IOC_SET_FREQ:
    {
        struct adrv9002_freq_params freq;

        if (!priv->initialized)
        {
            dev_err(&priv->spi->dev, "Not initialized\n");
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&freq, (void __user *)arg, sizeof(freq)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_set_frequency(priv, &freq);
        break;
    }

    case ADRV9002_IOC_GET_FREQ:
    {
        struct adrv9002_freq_params freq;

        if (copy_from_user(&freq, (void __user *)arg, sizeof(freq)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_get_frequency(priv, &freq);

        if (ret == 0 && copy_to_user((void __user *)arg, &freq, sizeof(freq)))
            ret = -EFAULT;
        break;
    }

    case ADRV9002_IOC_SET_GAIN:
    {
        struct adrv9002_gain_params gain;

        if (!priv->initialized)
        {
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&gain, (void __user *)arg, sizeof(gain)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_set_gain(priv, &gain);
        break;
    }

    case ADRV9002_IOC_RX_ENABLE:
    {
        struct adrv9002_channel_ctrl ctrl;

        if (!priv->initialized)
        {
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_enable_rx(priv, &ctrl);
        break;
    }

    case ADRV9002_IOC_TX_ENABLE:
    {
        struct adrv9002_channel_ctrl ctrl;

        if (!priv->initialized)
        {
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_enable_tx(priv, &ctrl);
        break;
    }

    case ADRV9002_IOC_RESET:
        adrv9002_hw_reset(priv);
        priv->initialized = false;
        dev_info(&priv->spi->dev, "Device reset\n");
        break;

    default:
        dev_err(&priv->spi->dev, "Unknown ioctl: 0x%x\n", cmd);
        ret = -ENOTTY;
    }

    mutex_unlock(&priv->lock);
    return ret;
}

static const struct file_operations adrv9002_fops = {
    .owner = THIS_MODULE,
    .open = adrv9002_open,
    .release = adrv9002_release,
    .unlocked_ioctl = adrv9002_ioctl,
};

/* ============================================================================
 * SPI driver probe/remove
 * ============================================================================ */

static int adrv9002_probe(struct spi_device *spi)
{
    struct adrv9002_priv *priv;
    int ret;

    dev_info(&spi->dev, "Probing ADRV9002 driver v%s\n", DRIVER_VERSION);

    /* Allocate private data */
    priv = devm_kzalloc(&spi->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    priv->spi = spi;
    g_priv = priv;
    mutex_init(&priv->lock);

    /* Get GPIO from Device Tree */
    priv->reset_gpio = devm_gpiod_get_optional(&spi->dev, "reset-gpio",
                                               GPIOD_OUT_LOW);
    if (IS_ERR(priv->reset_gpio))
    {
        ret = PTR_ERR(priv->reset_gpio);
        dev_err(&spi->dev, "Failed to get reset GPIO: %d\n", ret);
        return ret;
    }

    ret = adrv9002_hal_init(priv, priv->spi, priv->reset_gpio);
    if (ret) {
        dev_err(&spi->dev, "HAL init failed\n");
        return ret;
    }

    /* Allocate character device region */
    ret = alloc_chrdev_region(&priv->devt, 0, 1, DRIVER_NAME);
    if (ret < 0)
    {
        dev_err(&spi->dev, "Failed to allocate chrdev region: %d\n", ret);
        return ret;
    }

    /* Initialize character device */
    cdev_init(&priv->cdev, &adrv9002_fops);
    priv->cdev.owner = THIS_MODULE;

    ret = cdev_add(&priv->cdev, priv->devt, 1);
    if (ret < 0)
    {
        dev_err(&spi->dev, "Failed to add cdev: %d\n", ret);
        goto err_chrdev_region;
    }

    /* Create device class */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
    priv->class = class_create(THIS_MODULE, DRIVER_NAME);
#else
    priv->class = class_create(DRIVER_NAME);
#endif

    if (IS_ERR(priv->class))
    {
        ret = PTR_ERR(priv->class);
        dev_err(&spi->dev, "Failed to create class: %d\n", ret);
        goto err_cdev;
    }

    /* Create device node */
    priv->device = device_create(priv->class, &spi->dev, priv->devt,
                                 NULL, DRIVER_NAME);
    if (IS_ERR(priv->device))
    {
        ret = PTR_ERR(priv->device);
        dev_err(&spi->dev, "Failed to create device: %d\n", ret);
        goto err_class;
    }

    spi_set_drvdata(spi, priv);

    ret = spi_dev_init(&priv->spi_dev, spi);
    if (ret < 0)
    {
        dev_err(&spi->dev, "Failed to initialize SPI device: %d\n", ret);
        goto err_class;
    }

    dev_info(&spi->dev, "SPI device initialized successfully");
    dev_info(&spi->dev, "ADRV9002 driver probed successfully\n");
    dev_info(&spi->dev, "Device node: /dev/%s\n", DRIVER_NAME);
    dev_info(&spi->dev, "Use ioctl(ADRV9002_IOC_INIT) to initialize\n");
    return 0;

err_class:
    class_destroy(priv->class);
err_cdev:
    cdev_del(&priv->cdev);
err_chrdev_region:
    unregister_chrdev_region(priv->devt, 1);
    return ret;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
static int adrv9002_remove(struct spi_device *spi)
#else
static void adrv9002_remove(struct spi_device *spi)
#endif
{
    struct adrv9002_priv *priv = spi_get_drvdata(spi);

    dev_info(&spi->dev, "Removing ADRV9002 driver\n");


    /* TODO: Cleanup vendor API structures */
    if (priv->adrv9001Device) {
        kfree(priv->adrv9001Device);
    }

    if (priv->adrv9001Init) {
        kfree(priv->adrv9001Init);
    }

    device_destroy(priv->class, priv->devt);
    class_destroy(priv->class);
    cdev_del(&priv->cdev);
    unregister_chrdev_region(priv->devt, 1);

    g_priv = NULL;

    dev_info(&spi->dev, "ADRV9002 driver removed\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
    return 0;
#endif
}

/* Device Tree matching */
static const struct of_device_id adrv9002_of_match[] = {
    {.compatible = "adi,adrv9002"},
    {}};
MODULE_DEVICE_TABLE(of, adrv9002_of_match);

static struct spi_driver adrv9002_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = adrv9002_of_match,
    },
    .probe = adrv9002_probe,
    .remove = adrv9002_remove,
};

module_spi_driver(adrv9002_driver);

MODULE_AUTHOR("FernandezKA <fernandes.kir@yandex.ru>");
MODULE_DESCRIPTION("ADRV9002 RF Transceiver Driver");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");
