/**
 * @file adrv9002_radio.h
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <linux/delay.h>
#include <linux/mutex.h>

#include "adrv9002.h"
#include "adrv9002_hal.h"
#include "adrv9002_ioctl.h"

#include "tes/initialize.h"
#include "tes/calibrate.h"
#include "tes/configure.h"
#include "tes/prime.h"
#include "tes/begin_receiving.h"
#include "tes/stop_receiving.h"
#include "tes/begin_transmitting.h"
#include "tes/stop_transmitting.h"

#ifndef ADRV9002_RADIO_H
#define ADRV9002_RADIO_H

extern bool adrv9002_measure_timing;

void adrv9002_hw_reset(struct adrv9002_priv *priv);

int adrv9002_do_init(struct adrv9002_priv *priv,
                     struct adrv9002_init_params *params);

int adrv9002_set_frequency(struct adrv9002_priv *priv,
                           struct adrv9002_freq_params *freq);

int adrv9002_set_gain(struct adrv9002_priv *priv,
                      struct adrv9002_gain_params *gain);

int adrv9002_get_frequency(struct adrv9002_priv *priv,
                           struct adrv9002_freq_params *freq);

int adrv9002_enable_rx(struct adrv9002_priv *priv,
                       struct adrv9002_channel_ctrl *ctrl);

int adrv9002_enable_tx(struct adrv9002_priv *priv,
                       struct adrv9002_channel_ctrl *ctrl);

void adrv9002_free_device(struct adrv9002_priv *priv);

#endif
