/**
 * @file adrv9002_ioctl.h
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-01
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef _ADRV9002_IOCTL_H_
#define _ADRV9002_IOCTL_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define ADRV9002_IOC_MAGIC 'A'

struct adrv9002_init_params
{
    char *profile_buf;
    size_t profile_buf_len;
    char *stream_buf;
    size_t stream_buf_len;
};

struct adrv9002_freq_params
{
    __u64 freq_hz;
    __u8 channel;
    __u8 port;
};

struct adrv9002_gain_params
{
    __u32 gain;
    __u8 channel;
    __u8 port;
};

struct adrv9002_channel_ctrl
{
    __u8 channel;
    __u8 enable;
};


#define ADRV9002_IOC_INIT _IOW(ADRV9002_IOC_MAGIC, 1, struct adrv9002_init_params)
#define ADRV9002_IOC_SET_FREQ _IOW(ADRV9002_IOC_MAGIC, 2, struct adrv9002_freq_params)
#define ADRV9002_IOC_GET_FREQ _IOR(ADRV9002_IOC_MAGIC, 3, struct adrv9002_freq_params)
#define ADRV9002_IOC_SET_GAIN _IOW(ADRV9002_IOC_MAGIC, 4, struct adrv9002_gain_params)
#define ADRV9002_IOC_RX_ENABLE _IOW(ADRV9002_IOC_MAGIC, 5, struct adrv9002_channel_ctrl)
#define ADRV9002_IOC_TX_ENABLE _IOW(ADRV9002_IOC_MAGIC, 6, struct adrv9002_channel_ctrl)
#define ADRV9002_IOC_RESET _IO(ADRV9002_IOC_MAGIC, 7)
#define ADRV9002_IOC_LNA_ENABLE  _IOW(ADRV9002_IOC_MAGIC, 8, __u8)
#define ADRV9002_IOC_LNA_DISABLE _IOW(ADRV9002_IOC_MAGIC, 9, __u8)
#define ADRV9002_IOC_MAXNR 9

#endif
