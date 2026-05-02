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
#include <linux/string.h>
#include <linux/compat.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>
#include <linux/printk.h>

#include "adrv9002.h"
#include "adrv9002_ioctl.h"
#include "adrv9002_radio.h"

/* Avoid redefinition warning from printk.h */
#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DRIVER_NAME "adrv9002"
#define DRIVER_VERSION "0.1.0"
/* Limit incoming profile/stream buffers from user space to avoid OOM */
#define ADRV9002_MAX_PROFILE_SIZE   (1 * 1024 * 1024)
#define ADRV9002_MAX_STREAM_SIZE    (1 * 1024 * 1024)
/*private device structure*/

bool adrv9002_verbose_logs;
module_param_named(verbose_logs, adrv9002_verbose_logs, bool, 0644);
MODULE_PARM_DESC(verbose_logs, "Enable verbose info logs for tuning/init operations");
bool adrv9002_detailed_logs = false;
module_param_named(detailed_logs, adrv9002_detailed_logs, bool, 0644);
MODULE_PARM_DESC(detailed_logs, "Enable detailed frequency change logging to ring buffer (disabled after first reinit)");
bool adrv9002_measure_timing;
module_param_named(measure_timing, adrv9002_measure_timing, bool, 0644);
MODULE_PARM_DESC(measure_timing, "Enable timing measurements for init and frequency tuning operations");

bool adrv9002_auto_reinit = false;
module_param_named(auto_reinit, adrv9002_auto_reinit, bool, 0644);
MODULE_PARM_DESC(auto_reinit, "Automatically reinitialize transceiver on IOCTL errors (workaround for ADI API issues)");

int adrv9002_freq_change_timeout_us = 500;
module_param_named(freq_change_timeout_us, adrv9002_freq_change_timeout_us, int, 0644);
MODULE_PARM_DESC(freq_change_timeout_us, "Baseline sleep (us) for state polling during frequency change (default 500)");

int adrv9002_prime_retry_count = 4;
module_param_named(prime_retry_count, adrv9002_prime_retry_count, int, 0644);
MODULE_PARM_DESC(prime_retry_count, "Number of retry attempts for prime operation (default 4, max 5)");

static struct proc_dir_entry *adrv9002_proc_entry;
static bool procfs_initialized = false;

/* Dump guard regions of priv to diagnose memory corruption */
/* Validate that critical pointers in priv structure are still valid */
static int adrv9002_try_auto_reinit(struct adrv9002_priv *priv)
{
    struct adrv9002_init_params params;
    int ret;

    if (!adrv9002_auto_reinit || !priv->saved_profile_buf || !priv->saved_stream_buf) {
        return -ENODATA;
    }

    dev_warn(&priv->spi->dev, "Attempting auto-reinit due to IOCTL error (count: %d)\n",
             atomic_read(&priv->reinit_count) + 1);

    /* Reset device */
    adrv9002_hw_reset(priv);
    if (priv->lna)
        lna_disable(priv->lna, 0xFF);
    adrv9002_free_device(priv);
    memset(priv->lo_freq, 0, sizeof(priv->lo_freq));
    memset(priv->gain, 0, sizeof(priv->gain));
    /* reset state is implied by NULL adrv9001Device */

    /* Reinit with saved buffers */
    params.profile_buf = priv->saved_profile_buf;
    params.profile_buf_len = priv->saved_profile_buf_len;
    params.stream_buf = priv->saved_stream_buf;
    params.stream_buf_len = priv->saved_stream_buf_len;

    ret = adrv9002_do_init(priv, &params);
    if (ret == 0) {
        atomic_inc(&priv->reinit_count);
        dev_info(&priv->spi->dev, "Auto-reinit successful\n");
    } else {
        dev_err(&priv->spi->dev, "Auto-reinit failed: %d\n", ret);
    }

    return ret;
}

static int adrv9002_handle_init_ioctl(struct adrv9002_priv *priv,
                                      struct adrv9002_init_params *params)
{
    int ret;

    /* Always (re)initialize on INIT call: previous state will be freed in do_init */

    if (!params->profile_buf || params->profile_buf_len <= 0 ||
        params->profile_buf_len > ADRV9002_MAX_PROFILE_SIZE)
    {
        dev_err(&priv->spi->dev, "Profile buffer invalid or too large\n");
        return -EINVAL;
    }

    if (!params->stream_buf || params->stream_buf_len <= 0 ||
        params->stream_buf_len > ADRV9002_MAX_STREAM_SIZE)
    {
        dev_err(&priv->spi->dev, "Stream buffer invalid or too large\n");
        return -EINVAL;
    }

    /* Allocate and copy user buffers in one step */
    char *kernel_profile_buf = memdup_user(params->profile_buf, params->profile_buf_len);
    if (IS_ERR(kernel_profile_buf))
    {
        ret = PTR_ERR(kernel_profile_buf);
        dev_err(&priv->spi->dev, "Failed to copy profile buffer: %d\n", ret);
        return ret;
    }

    char *kernel_stream_buf = memdup_user(params->stream_buf, params->stream_buf_len);
    if (IS_ERR(kernel_stream_buf))
    {
        ret = PTR_ERR(kernel_stream_buf);
        dev_err(&priv->spi->dev, "Failed to copy stream buffer: %d\n", ret);
        goto err_free_profile;
    }

    params->profile_buf = kernel_profile_buf;
    params->stream_buf = kernel_stream_buf;

    ret = adrv9002_do_init(priv, params);

    /* Save buffers for potential auto-reinit on errors */
    if (ret == 0 && adrv9002_auto_reinit) {
        /* Free old saved buffers if any */
        if (priv->saved_profile_buf) {
            kfree(priv->saved_profile_buf);
            priv->saved_profile_buf = NULL;
        }
        if (priv->saved_stream_buf) {
            kfree(priv->saved_stream_buf);
            priv->saved_stream_buf = NULL;
        }

        /* Allocate new copies */
        priv->saved_profile_buf = kmalloc(params->profile_buf_len, GFP_KERNEL);
        if (priv->saved_profile_buf) {
            memcpy(priv->saved_profile_buf, kernel_profile_buf, params->profile_buf_len);
            priv->saved_profile_buf_len = params->profile_buf_len;
        }

        priv->saved_stream_buf = kmalloc(params->stream_buf_len, GFP_KERNEL);
        if (priv->saved_stream_buf) {
            memcpy(priv->saved_stream_buf, kernel_stream_buf, params->stream_buf_len);
            priv->saved_stream_buf_len = params->stream_buf_len;
        }
    }

    kfree(kernel_stream_buf);
err_free_profile:
    kfree(kernel_profile_buf);
    return ret;
}

static int adrv9002_proc_show(struct seq_file *m, void *v)
{
    struct adrv9002_priv *priv = m->private;
    bool lna_present = false;

    if (priv && priv->lna)
        lna_present = priv->lna->available;

    seq_printf(m, "driver_version: %s\n", DRIVER_VERSION);
    seq_printf(m, "initialized: %d\n", priv && priv->adrv9001Device ? 1 : 0);
    seq_printf(m, "auto_reinit_count: %d\n", priv ? atomic_read(&priv->reinit_count) : 0);
    seq_printf(m, "lna_present: %d\n", lna_present);
    seq_printf(m, "lna_gpios: %d\n",
               lna_present ? priv->lna->num_gpios : 0);
    seq_printf(m, "lna_state: 0x%02x\n",
               lna_present ? priv->lna->lna_state : 0);
    seq_printf(m, "lo_freq[0]: %llu\n", priv ? priv->lo_freq[0] : 0);
    seq_printf(m, "lo_freq[1]: %llu\n", priv ? priv->lo_freq[1] : 0);
    seq_printf(m, "gain[0]: %u\n", priv ? priv->gain[0] : 0);
    seq_printf(m, "gain[1]: %u\n", priv ? priv->gain[1] : 0);

    if (unlikely(adrv9002_measure_timing) && priv) {
        seq_printf(m, "ioctl_init_time_us: %llu\n", priv->ioctl_time_us[0]);
        seq_printf(m, "ioctl_set_freq_time_us: %llu\n", priv->ioctl_time_us[1]);
        seq_printf(m, "ioctl_get_freq_time_us: %llu\n", priv->ioctl_time_us[2]);
        seq_printf(m, "ioctl_set_gain_time_us: %llu\n", priv->ioctl_time_us[3]);
        seq_printf(m, "ioctl_rx_enable_time_us: %llu\n", priv->ioctl_time_us[4]);
        seq_printf(m, "ioctl_tx_enable_time_us: %llu\n", priv->ioctl_time_us[5]);
        seq_printf(m, "ioctl_reset_time_us: %llu\n", priv->ioctl_time_us[6]);
        seq_printf(m, "ioctl_lna_enable_time_us: %llu\n", priv->ioctl_time_us[7]);
        seq_printf(m, "ioctl_lna_disable_time_us: %llu\n", priv->ioctl_time_us[8]);
        seq_printf(m, "freq_change_timeout_us: %d\n", adrv9002_freq_change_timeout_us);
        seq_printf(m, "prime_retry_count: %d\n", adrv9002_prime_retry_count);
    }
    
    seq_printf(m, "\n=== Detailed Logs ===\n");
    seq_printf(m, "detailed_logs enabled: %s\n", adrv9002_detailed_logs ? "yes" : "no");

    return 0;
}

static int adrv9002_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, adrv9002_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops adrv9002_proc_ops = {
    .proc_open = adrv9002_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
#else
static const struct file_operations adrv9002_proc_ops = {
    .owner = THIS_MODULE,
    .open = adrv9002_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif

/* ============================================================================
 * Hardware control functions
 * ============================================================================ */

static int spi_dev_init(struct spi_dev *dev, struct spi_device *spi)
{
    if (!dev || !spi)
    {
        return -EINVAL;
    }

    dev->spi = spi;
    return 0;
}

/* ============================================================================
 * Character device operations
 * ============================================================================ */

static int adrv9002_open(struct inode *inode, struct file *filp)
{
    struct adrv9002_priv *priv;

    priv = container_of(inode->i_cdev, struct adrv9002_priv, cdev);
    
    if (!priv || !virt_addr_valid(priv)) {
        pr_err("adrv9002: open() got invalid priv from container_of\n");
        return -EFAULT;
    }
    if (priv->removed) {
        pr_err("adrv9002: device was removed, refusing open\n");
        return -ENODEV;
    }
    
    if (!priv->spi || !virt_addr_valid(priv->spi)) {
        pr_err("adrv9002: priv->spi is invalid in open()\n");
        return -EFAULT;
    }
    
    filp->private_data = priv;
    atomic_inc(&priv->open_count);

    dev_dbg(&priv->spi->dev, "Device opened\n");
    return 0;
}

static int adrv9002_release(struct inode *inode, struct file *filp)
{
    struct adrv9002_priv *priv = filp->private_data;

    if (!priv || !virt_addr_valid(priv)) {
        pr_warn("adrv9002: release() called with invalid priv\n");
        filp->private_data = NULL;
        return 0;
    }
    
    if (!priv->spi || !virt_addr_valid(priv->spi)) {
        pr_warn("adrv9002: priv->spi invalid in release()\n");
        filp->private_data = NULL;
        goto out_dec;
    }

    dev_dbg(&priv->spi->dev, "Device closed\n");
    filp->private_data = NULL;
out_dec:
    if (atomic_dec_and_test(&priv->open_count)) {
        /* Last close: if remove already happened, finalize char device cleanup */
        if (priv->removed) {
            /* Deinitialize hardware and HAL now that last fd is closed */
            adrv9002_free_device(priv);
            adrv9002_hal_deinit(priv);

            cdev_del(&priv->cdev);
            unregister_chrdev_region(priv->devt, 1);

            /* priv was allocated w/o devm; safe to free here */
            kfree(priv);
        }
    }
    return 0;
}

static long adrv9002_ioctl(struct file *filp, unsigned int cmd,
                           unsigned long arg)
{
    struct adrv9002_priv *priv = filp->private_data;
    int ret = 0;

    if (!priv || !virt_addr_valid(priv)) {
        pr_err("adrv9002: IOCTL called with invalid priv pointer\n");
        return -EFAULT;
    }

    /* Disallow operations once device is removed */
    if (priv->removed) {
        pr_err("adrv9002: device removed, ioctl rejected\n");
        return -ENODEV;
    }

    /* Validate ioctl command */
    if (_IOC_TYPE(cmd) != ADRV9002_IOC_MAGIC)
    {
        pr_err("adrv9002: Invalid ioctl magic: 0x%x\n", _IOC_TYPE(cmd));
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > ADRV9002_IOC_MAXNR)
    {
        pr_err("adrv9002: Invalid ioctl number: %u\n", _IOC_NR(cmd));
        return -ENOTTY;
    }

    ktime_t start_time = 0, end_time;
    s64 duration_us;
    unsigned int cmd_nr = _IOC_NR(cmd);

    if (unlikely(adrv9002_measure_timing))
        start_time = ktime_get();

    mutex_lock(&priv->lock);
    /* Re-check removal under lock to handle races */
    if (priv->removed) {
        mutex_unlock(&priv->lock);
        pr_err("adrv9002: device removed (locked), ioctl rejected\n");
        return -ENODEV;
    }

    switch (cmd)
    {
    case ADRV9002_IOC_INIT:
    {
        struct adrv9002_init_params params;

        if (copy_from_user(&params, (void __user *)arg, sizeof(params)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_handle_init_ioctl(priv, &params);
        break;
    }
    case ADRV9002_IOC_SET_FREQ:
    {
        struct adrv9002_freq_params freq;

        if (!priv->adrv9001Device)
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
        if (ret != 0 && ret != -EAGAIN && ret != -EINVAL) {
            dev_warn(&priv->spi->dev, "SET_FREQ failed with ret=%d, attempting auto-reinit\n", ret);
            /* Try auto-reinit on unexpected errors */
            if (adrv9002_try_auto_reinit(priv) == 0) {
                dev_info(&priv->spi->dev, "Auto-reinit succeeded, retrying SET_FREQ\n");
                /* Retry operation after successful reinit */
                ret = adrv9002_set_frequency(priv, &freq);
            } else {
                dev_err(&priv->spi->dev, "Auto-reinit failed\n");
            }
        }
        
        if (ret != 0) {
            dev_err(&priv->spi->dev, "IOCTL: SET_FREQ failed with ret=%d\n", ret);
        }
        break;
    }

    case ADRV9002_IOC_GET_FREQ:
    {
        struct adrv9002_freq_params freq;

        if (!priv->adrv9001Device)
        {
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&freq, (void __user *)arg, sizeof(freq)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_get_frequency(priv, &freq);
        if (ret != 0 && ret != -EINVAL) {
            /* Try auto-reinit on unexpected errors */
            if (adrv9002_try_auto_reinit(priv) == 0) {
                /* Retry operation after successful reinit */
                ret = adrv9002_get_frequency(priv, &freq);
            }
        }

        if (ret == 0 && copy_to_user((void __user *)arg, &freq, sizeof(freq)))
            ret = -EFAULT;
        break;
    }

    case ADRV9002_IOC_SET_GAIN:
    {
        struct adrv9002_gain_params gain;

        if (!priv->adrv9001Device)
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
        if (ret != 0 && ret != -EINVAL) {
            /* Try auto-reinit on unexpected errors */
            if (adrv9002_try_auto_reinit(priv) == 0) {
                /* Retry operation after successful reinit */
                ret = adrv9002_set_gain(priv, &gain);
            }
        }
        break;
    }

    case ADRV9002_IOC_RX_ENABLE:
    {
        struct adrv9002_channel_ctrl ctrl;

        if (!priv->adrv9001Device)
        {
            if (adrv9002_validate_priv(priv))
                dev_err(&priv->spi->dev, "Device not initialized\n");
            else
                pr_err("adrv9002: Device not initialized\n");
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
        {
            printk(KERN_ERR "Failed to copy channel control\n");
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_enable_rx(priv, &ctrl);
        if (ret != 0 && ret != -EINVAL) {
            /* Try auto-reinit on unexpected errors */
            if (adrv9002_try_auto_reinit(priv) == 0) {
                /* Retry operation after successful reinit */
                ret = adrv9002_enable_rx(priv, &ctrl);
            }
        }
        break;
    }

    case ADRV9002_IOC_TX_ENABLE:
    {
        struct adrv9002_channel_ctrl ctrl;

        if (!priv->adrv9001Device)
        {
            dev_err(&priv->spi->dev, "Device not initialized\n");
            ret = -ENODEV;
            break;
        }

        if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
        {
            ret = -EFAULT;
            break;
        }

        ret = adrv9002_enable_tx(priv, &ctrl);
        if (ret != 0 && ret != -EINVAL) {
            /* Try auto-reinit on unexpected errors */
            if (adrv9002_try_auto_reinit(priv) == 0) {
                /* Retry operation after successful reinit */
                ret = adrv9002_enable_tx(priv, &ctrl);
            }
        }
        break;
    }

    case ADRV9002_IOC_RESET:
        adrv9002_hw_reset(priv);
        if (priv->lna)
            lna_disable(priv->lna, 0xFF);
        adrv9002_free_device(priv);
        memset(priv->lo_freq, 0, sizeof(priv->lo_freq));
        memset(priv->gain, 0, sizeof(priv->gain));
        if (adrv9002_validate_priv(priv))
            dev_info(&priv->spi->dev, "Device reset\n");
        else
            pr_info("adrv9002: Device reset\n");
        break;

        case ADRV9002_IOC_LNA_ENABLE:
        {
            __u8 mask;

            if (!priv->adrv9001Device) {
                ret = -ENODEV;
                break;
            }

            if (copy_from_user(&mask, (void __user *)arg, sizeof(mask))) {
                ret = -EFAULT;
                break;
            }

            ret = lna_enable(priv->lna, mask);
            break;
        }

        case ADRV9002_IOC_LNA_DISABLE:
        {
            __u8 mask;

            if (!priv->adrv9001Device) {
                ret = -ENODEV;
                break;
            }

            if (copy_from_user(&mask, (void __user *)arg, sizeof(mask))) {
                ret = -EFAULT;
                break;
            }

            ret = lna_disable(priv->lna, mask);
            break;
        }


    default:
        if (adrv9002_validate_priv(priv))
            dev_err(&priv->spi->dev, "Unknown ioctl: 0x%x\n", cmd);
        else
            pr_err("adrv9002: Unknown ioctl: 0x%x\n", cmd);
        ret = -ENOTTY;
    }

    mutex_unlock(&priv->lock);

    if (unlikely(adrv9002_measure_timing) && cmd_nr >= 1 && cmd_nr <= 9) {
        end_time = ktime_get();
        duration_us = ktime_to_us(ktime_sub(end_time, start_time));
        priv->ioctl_time_us[cmd_nr - 1] = (u64)duration_us;
    }

    return ret;
}

#ifdef CONFIG_COMPAT
struct adrv9002_init_params_compat {
    compat_uptr_t profile_buf;
    u32 profile_buf_len;
    compat_uptr_t stream_buf;
    u32 stream_buf_len;
};

static long adrv9002_compat_ioctl(struct file *filp, unsigned int cmd,
                                  unsigned long arg)
{
    struct adrv9002_priv *priv = filp->private_data;
    int ret = 0;

    if (_IOC_TYPE(cmd) != ADRV9002_IOC_MAGIC || _IOC_NR(cmd) > ADRV9002_IOC_MAXNR)
        return -ENOTTY;

    switch (cmd)
    {
    case ADRV9002_IOC_INIT:
    {
        struct adrv9002_init_params_compat params32;
        struct adrv9002_init_params params;

        if (copy_from_user(&params32, compat_ptr(arg), sizeof(params32)))
            return -EFAULT;

        params.profile_buf = compat_ptr(params32.profile_buf);
        params.profile_buf_len = params32.profile_buf_len;
        params.stream_buf = compat_ptr(params32.stream_buf);
        params.stream_buf_len = params32.stream_buf_len;

        mutex_lock(&priv->lock);
        ret = adrv9002_handle_init_ioctl(priv, &params);
        mutex_unlock(&priv->lock);
        return ret;
    }

    default:
        /* Other ioctl structs are layout-compatible; delegate to native path */
        return adrv9002_ioctl(filp, cmd, arg);
    }
}
#endif

static const struct file_operations adrv9002_fops = {
    .owner = THIS_MODULE,
    .open = adrv9002_open,
    .release = adrv9002_release,
    .unlocked_ioctl = adrv9002_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = adrv9002_compat_ioctl,
#endif
};

/* ============================================================================
 * SPI driver probe/remove
 * ============================================================================ */
/*
 * Parse reset configuration from Device Tree
 * Supports both GPIO-based and register-based reset
 */
static int adrv9002_parse_reset_config(struct device *dev,
                                       struct adrv9002_reset_config *cfg)
{
    struct device_node *np = dev->of_node;
    struct gpio_desc *gpio;

    if (!np)
        return -EINVAL;

    gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
    
    if (IS_ERR(gpio)) {
        dev_err(dev, "Failed to get reset GPIO: %ld\n", PTR_ERR(gpio));
        return PTR_ERR(gpio);
    }

    if (gpio) {
        dev_info(dev, "Got reset GPIO successfully\n");
        cfg->type = RESET_TYPE_GPIO;
        cfg->u.gpio = gpio;
        return 0;
    }

    if (of_property_read_bool(np, "reset-gpios")) {
        dev_warn(dev, "reset-gpios in DTS but GPIO not accessible, deferring\n");
        return -EPROBE_DEFER;
    }

    dev_info(dev, "No reset GPIO configured in DTS\n");
    cfg->type = RESET_TYPE_NONE;
    return 0;
}


static int adrv9002_probe(struct spi_device *spi)
{
    struct adrv9002_priv *priv;
    int ret;
    bool hal_inited = false;

    dev_info(&spi->dev, "Probing ADRV9002 driver v%s\n", DRIVER_VERSION);

    /* Allocate private data with explicit lifetime control (not devm) */
    priv = kzalloc(sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    priv->magic = ADRV9002_PRIV_MAGIC;
    priv->spi = spi;
    atomic_set(&priv->open_count, 0);
    priv->removed = false;

    priv->reset_config = devm_kzalloc(&spi->dev, sizeof(*priv->reset_config), GFP_KERNEL);
    if (!priv->reset_config) {
        ret = -ENOMEM;
        goto err_free_priv;
    }

    mutex_init(&priv->lock);
    
    ret = adrv9002_parse_reset_config(&spi->dev, priv->reset_config);
    if (ret) {
        if (ret != -EPROBE_DEFER) {
            dev_err(&spi->dev, "Failed to parse reset configuration: %d\n", ret);
        }
        return ret;
    }

    ret = adrv9002_hal_init(priv, priv->spi, priv->reset_config);
    if (ret)
    {
        dev_err(&spi->dev, "HAL init failed\n");
        return ret;
    }
    hal_inited = true;

    priv->lna = devm_kzalloc(&spi->dev, sizeof(*priv->lna), GFP_KERNEL);
    if (!priv->lna) {
        dev_err(&spi->dev, "Failed to allocate LNA struct\n");
        ret = -ENOMEM;
        goto err_hal_init;
    }

    ret = parse_lna_config(&spi->dev, priv->lna);
    if (ret) {
        dev_warn(&spi->dev, "LNA config not found or invalid (%d). Continuing without LNA.\n", ret);
    }

    ret = lna_init(priv->lna);
    if (ret) {
        dev_warn(&spi->dev, "LNA init failed (%d). Continuing without LNA control.\n", ret);
    }

    /* Allocate character device region */
    ret = alloc_chrdev_region(&priv->devt, 0, 1, DRIVER_NAME);
    if (ret < 0)
    {
        dev_err(&spi->dev, "Failed to allocate chrdev region: %d\n", ret);
        goto err_hal_init;
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
    
    /* Initialize device pointer to NULL to avoid kobject issues */
    priv->device = NULL;

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
        goto err_device;
    }

    /* Remove old proc entry if it exists (from previous probe) */
    if (adrv9002_proc_entry) {
        proc_remove(adrv9002_proc_entry);
        adrv9002_proc_entry = NULL;
    }

    adrv9002_proc_entry = proc_create_data("adrv9002", 0444, NULL,
                                           &adrv9002_proc_ops, priv);
    if (!adrv9002_proc_entry)
        dev_warn(&spi->dev, "Failed to create /proc/driver/adrv9002\n");

    dev_info(&spi->dev, "SPI device initialized successfully");
    dev_info(&spi->dev, "ADRV9002 driver probed successfully\n");
    dev_info(&spi->dev, "Device node: /dev/%s\n", DRIVER_NAME);
    dev_info(&spi->dev, "Use ioctl(ADRV9002_IOC_INIT) to initialize\n");
    return 0;

err_device:
    device_destroy(priv->class, priv->devt);
err_class:
    class_destroy(priv->class);
err_cdev:
    cdev_del(&priv->cdev);
err_chrdev_region:
    unregister_chrdev_region(priv->devt, 1);
err_hal_init:
    if (hal_inited)
        adrv9002_hal_deinit(priv);
err_free_priv:
    kfree(priv);
    return ret;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
static int adrv9002_remove(struct spi_device *spi)
#else
static void adrv9002_remove(struct spi_device *spi)
#endif
{
    struct adrv9002_priv *priv = spi_get_drvdata(spi);

    /* Prevent new ioctls and opens; synchronize with ongoing ones */
    mutex_lock(&priv->lock);
    priv->removed = true;

    dev_info(&spi->dev, "Removing ADRV9002 driver\n");

    /* Gracefully quiesce radio if initialized */
    if (priv->adrv9001Device) {
        dev_info(&spi->dev, "Quiescing radio (stop RX/TX) before removal\n");
        stop_receiving(priv->adrv9001Device);
        stop_transmitting(priv->adrv9001Device);
    }

    if (priv->lna)
        lna_deinit(priv->lna);

    /* Free saved init buffers */
    if (priv->saved_profile_buf)
        kfree(priv->saved_profile_buf);
    if (priv->saved_stream_buf)
        kfree(priv->saved_stream_buf);

    /* Remove proc entries first */
    if (adrv9002_proc_entry) {
        proc_remove(adrv9002_proc_entry);
        adrv9002_proc_entry = NULL;
    }

    procfs_initialized = false;

    /* Free device resources only if no open files remain */
    if (atomic_read(&priv->open_count) == 0) {
        adrv9002_free_device(priv);
    }

    /* Remove device and class now to prevent new opens */
    if (priv->device)
        device_destroy(priv->class, priv->devt);
    if (priv->class)
        class_destroy(priv->class);
    priv->device = NULL;
    priv->class = NULL;

    if (atomic_read(&priv->open_count) == 0)
        adrv9002_hal_deinit(priv);

    /* If no open files, finalize char device and free priv */
    if (atomic_read(&priv->open_count) == 0) {
        cdev_del(&priv->cdev);
        unregister_chrdev_region(priv->devt, 1);
        priv->magic = 0;
        kfree(priv);
    } else {
        /* Defer cdev/unregister + kfree to last release() */
        priv->magic = ADRV9002_PRIV_MAGIC;
    }

    mutex_unlock(&priv->lock);
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

/* SPI device ID table to support autoload via spi:adrv9002 modalias */
static const struct spi_device_id adrv9002_id[] = {
    {"adrv9002", 0},
    {}
};
MODULE_DEVICE_TABLE(spi, adrv9002_id);

static struct spi_driver adrv9002_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = adrv9002_of_match,
    },
    .id_table = adrv9002_id,
    .probe = adrv9002_probe,
    .remove = adrv9002_remove,
};

module_spi_driver(adrv9002_driver);

MODULE_AUTHOR("FernandezKA <fernandes.kir@yandex.ru>");
MODULE_DESCRIPTION("ADRV9002 RF Transceiver Driver");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");
