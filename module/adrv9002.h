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

#include <linux/device.h>
#include <linux/atomic.h>

#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_profileutil.h"
#include "adi_common_error.h"
#include "adi_common_hal.h"
#include "adrv9002_hal.h"

#include "lna.h"

extern bool adrv9002_verbose_logs;
extern int adrv9002_freq_change_timeout_us;
extern int adrv9002_prime_retry_count;
extern bool adrv9002_detailed_logs;

#include "adrv9002_log_buffer.h"

#define ADRV9002_LOG_INFO(priv, fmt, ...)                                     \
    do {                                                                        \
        if (adrv9002_verbose_logs)                                                \
            dev_info(&(priv)->spi->dev, fmt, ##__VA_ARGS__);                        \
    } while (0)

#define ADRV9002_DETAILED_LOG(priv, fmt, ...)                                 \
    do {                                                                        \
        if (adrv9002_log_buffer_is_enabled(&(priv)->log_buffer)) {             \
            adrv9002_log_buffer_write((priv), fmt, ##__VA_ARGS__);              \
        }                                                                       \
    } while (0)

#define ADRV9002_LOG_DEV(dev, fmt, ...)                                       \
    do {                                                                        \
        if (adrv9002_verbose_logs)                                                \
            dev_info((dev), fmt, ##__VA_ARGS__);                                    \
    } while (0)

struct spi_dev {
    struct spi_device *spi;
};

#define ADRV9002_PRIV_MAGIC 0xAD900201

struct adrv9002_priv
{
    uint32_t magic;
    struct spi_device *spi;
    struct cdev cdev;
    dev_t devt;
    struct class *class;
    struct device *device;
    struct spi_dev spi_dev;
    atomic_t open_count; /* number of open file descriptors */
    bool removed;        /* device is being/was removed */
    void *hal_context;
    adi_common_ApiVersion_t api_version;
    adi_adrv9001_ArmVersion_t arm_version;
    adi_adrv9001_SiliconVersion_t silicon_version;
    adi_adrv9001_Device_t *adrv9001Device;
    adi_adrv9001_Init_t *adrv9001Init;
    /*state tracking removed: use adrv9001Device presence instead*/
    struct lna *lna;
    u64 lo_freq[2];
    u32 gain[2];
    /* timing measurements for IOCTL operations */
    u64 ioctl_time_us[9]; /* indexed by IOCTL command number */
    /* saved init buffers for auto-reinit on errors */
    char *saved_profile_buf;
    size_t saved_profile_buf_len;
    char *saved_stream_buf;
    size_t saved_stream_buf_len;
    atomic_t reinit_count; /* tracks number of auto-reinits */
    struct mutex lock;
    struct adrv9002_log_buffer log_buffer;
    struct adrv9002_reset_config *reset_config; /* stored for hw reset operations */
    uint32_t canary;
};

#define ADRV9002_PRIV_CANARY 0xDEADBEEF

 #endif // ADRV9002_H
