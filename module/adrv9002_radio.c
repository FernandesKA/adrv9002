/**
 * @file adrv9002_radio.c
 * @author FernandezKA (fernandes.kir@yandex.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <linux/ktime.h>
#include <linux/lockdep.h>

#include "adrv9002_radio.h"
#include "adrv9002.h"
#include "adrv9002_hal.h"

/* Helper to log detailed ARM error information */
static void log_arm_error(struct adrv9002_priv *priv, const char *context, int error_code)
{
  adi_common_ErrStruct_t *err;
  
  if (!priv || !priv->adrv9001Device)
    return;
    
  err = &priv->adrv9001Device->common.error;
  
  dev_err(&priv->spi->dev, "%s failed with code %d\n", context, error_code);
  dev_err(&priv->spi->dev, "  ARM Error: src=0x%x, code=0x%x, action=%d\n",
          err->errSource, err->errCode, err->newAction);
  dev_err(&priv->spi->dev, "  Location: %s:%u in %s\n",
          err->errFile ? err->errFile : "unknown",
          err->errLine,
          err->errFunc ? err->errFunc : "unknown");
  if (err->errormessage[0] != '\0')
    dev_err(&priv->spi->dev, "  Message: %s\n", err->errormessage);
}

static int wait_for_channel_state(struct adrv9002_priv *priv,
                                  adi_common_Port_e port,
                                  adi_common_ChannelNumber_e channel,
                                  adi_adrv9001_ChannelState_e desired,
                                  int max_iters,
                                  int sleep_low_us,
                                  int sleep_high_us)
{
  int ret;
  int counter = max_iters;
  int check_arm_interval = max_iters / 4; /* Check ARM every 25% */
  adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
  adi_adrv9001_ChannelState_e prev_state = ADI_ADRV9001_CHANNEL_STANDBY;
  ktime_t start_time = ktime_get();

  /* First check without sleep - state might already be correct */
  ret = adi_adrv9001_Radio_Channel_State_Get(priv->adrv9001Device, port,
                                             channel, &state);
  if (ret) {
    log_arm_error(priv, "State_Get initial", ret);
    return ret;
  }
  
  if (state == desired)
    return 0;

  prev_state = state;
  /* Now wait with sleeps */
  while (counter-- > 0) {
    usleep_range(sleep_low_us, sleep_high_us);
    ret = adi_adrv9001_Radio_Channel_State_Get(priv->adrv9001Device, port,
                                               channel, &state);
    if (ret) {
      log_arm_error(priv, "State_Get polling", ret);
      return ret;
    }
    
    /* Log state changes */
    if (state != prev_state) {
      prev_state = state;
    }
    
    if (state == desired) {
      return 0;
    }
    
    /* Periodically check ARM health */
    if (check_arm_interval > 0 && (counter % check_arm_interval) == 0) {
      ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 10000);
      if (ret) {
        s64 elapsed_us = ktime_us_delta(ktime_get(), start_time);
        dev_err(&priv->spi->dev, "ARM check failed during state wait after %lld us\n", elapsed_us);
        log_arm_error(priv, "ARM StartStatus during wait", ret);
        return ret;
      }
    }
  }

  s64 elapsed_us = ktime_us_delta(ktime_get(), start_time);
  dev_err(&priv->spi->dev, "TIMEOUT: waited %lld us for state=%d, stuck at state=%d\n",
          elapsed_us, desired, state);
  log_arm_error(priv, "State wait timeout", -ETIMEDOUT);
  return -ETIMEDOUT;
}

void adrv9002_hw_reset(struct adrv9002_priv *priv) {
  struct adrv9002_hal_context *hal_ctx =
      (struct adrv9002_hal_context *)priv->hal_context;

  dev_dbg(&priv->spi->dev, "Resetting hardware\n");
  adrv9002_reset_assert(hal_ctx->reset_config);
  adrv9002_reset_deassert(hal_ctx->reset_config);
}

int adrv9002_do_init(struct adrv9002_priv *priv,
                     struct adrv9002_init_params *params) {
  int ret = 0;
  dev_info(&priv->spi->dev, "Starting initialization...\n");

  if (!priv) {
    return -EINVAL;
  }

  adrv9002_free_device(priv);

  priv->adrv9001Device = kzalloc(sizeof(adi_adrv9001_Device_t), GFP_KERNEL);
  if (!priv->adrv9001Device)
    return -ENOMEM;

  priv->adrv9001Init = kzalloc(sizeof(adi_adrv9001_Init_t), GFP_KERNEL);
  if (!priv->adrv9001Init) {
    ret = -ENOMEM;
    goto err_free_device;
  }

  adrv9002_hw_reset(priv);

  ret = adi_adrv9001_profileutil_Parse(priv->adrv9001Device, priv->adrv9001Init,
                                       params->profile_buf,
                                       params->profile_buf_len);

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

  ret = initialize(priv->adrv9001Device, priv->adrv9001Init, params->stream_buf,
                   params->stream_buf_len);
  if (ret) {
    dev_err(&priv->spi->dev, "Initialization failed: %d\n", ret);
    return ret;
  } else {
    dev_info(&priv->spi->dev, "Initialization complete\n");
  }

  ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 5000000);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't answer on status arm request. Try to "
                             "reinit with hardware gpio\n");
    return -EAGAIN;
  }

  ret = calibrate(priv->adrv9001Device);
  if (ret) {
    dev_err(&priv->spi->dev, "Calibration failed: %d\n", ret);
    return ret;
  } else {
    dev_info(&priv->spi->dev, "Calibration complete\n");
  }

  ret = configure(priv->adrv9001Device);
  if (ret) {
    dev_err(&priv->spi->dev, "Configure failed: %d\n", ret);
    return ret;
  } else {
    dev_info(&priv->spi->dev, "Configure complete\n");
  }

  ret = prime(priv->adrv9001Device);
  if (ret) {
    dev_err(&priv->spi->dev, "Prime failed: %d\n", ret);
    return ret;
  } else {
    dev_info(&priv->spi->dev, "Prime complete\n");
  }

  ret = begin_receiving(priv->adrv9001Device);
  if (ret) {
    dev_err(&priv->spi->dev, "Begin receiving failed: %d\n", ret);
    return ret;
  } else {
    dev_info(&priv->spi->dev, "Begin receiving complete\n");
  }

  dev_info(&priv->spi->dev, "Initialization complete\n");

  return 0;

err_free_device:
  adrv9002_free_device(priv);
  return ret;
}

int adrv9002_get_frequency(struct adrv9002_priv *priv,
                           struct adrv9002_freq_params *freq) {
  if (freq->channel >= 2)
    return -EINVAL;

  /* Return cached value */
  freq->freq_hz = priv->lo_freq[freq->channel];

  return 0;
}

int adrv9002_set_frequency(struct adrv9002_priv *priv,
                           struct adrv9002_freq_params *freq) {
  int ret = 0;
  adi_common_ChannelNumber_e channel;
  ktime_t start_time = ktime_get();
  u64 prev_freq;
  bool verbose = adrv9002_detailed_logs;  /* Local copy for this operation */

  /* Ensure callers serialize frequency tuning; ioctl holds priv->lock */
  lockdep_assert_held(&priv->lock);

  switch (freq->port) {
  case ADI_RX:
  case ADI_TX:
  case ADI_ORX:
  case ADI_ILB:
  case ADI_ELB:
    break;
  default:
    dev_err(&priv->spi->dev, "Invalid port: %u\n", freq->port);
    return -EINVAL;
  }

  prev_freq = (freq->channel < 2) ? priv->lo_freq[freq->channel] : 0;

  adi_adrv9001_ChannelState_e ch_state;
  channel = ADI_CHANNEL_1;
  ch_state = ADI_ADRV9001_CHANNEL_STANDBY;

  /* Check ARM status before frequency change */
  ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 10000);
  if (ret) {
    dev_err(&priv->spi->dev, "ARM not responding before freq change: %d\n", ret);
    log_arm_error(priv, "ARM StartStatus before freq change", ret);
    return ret;
  }

  ret = adi_common_index_to_channel(freq->channel, &channel);
  if (ret) {
    dev_err(&priv->spi->dev, "Invalid channel: %u\n", freq->channel);
    return -EINVAL;
  }

  if (freq->freq_hz < ADI_ADRV9001_CARRIER_FREQUENCY_MIN_HZ ||
      freq->freq_hz > ADI_ADRV9001_CARRIER_FREQUENCY_MAX_HZ) {
    dev_err(&priv->spi->dev, "Invalid frequency: %llu Hz\n", freq->freq_hz);
    return -EINVAL;
  }

  ret = adi_adrv9001_Radio_Channel_State_Get(priv->adrv9001Device, freq->port,
                                             channel, &ch_state);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't get channel state: %d\n", ret);
    log_arm_error(priv, "State_Get", ret);
    return ret;
  }

  if (ch_state != ADI_ADRV9001_CHANNEL_RF_ENABLED &&
      ch_state != ADI_ADRV9001_CHANNEL_PRIMED) {
    dev_err(&priv->spi->dev,
            "Channel is not in the RF enabled or primed state: %d\n", ch_state);
    return -EAGAIN;
  }

  switch (ch_state) {
  case (ADI_ADRV9001_CHANNEL_RF_ENABLED): {
    ret = adi_adrv9001_Radio_Channel_EnableRf(priv->adrv9001Device, freq->port,
                                              channel, false);
    if (ret) {
      dev_err(&priv->spi->dev, "Can't disable RF: %d\n", ret);
      log_arm_error(priv, "EnableRf(false)", ret);
      return -EAGAIN;
    }
    break;
  }
  case (ADI_ADRV9001_CHANNEL_PRIMED): {
    break;
  }

  default: {
    return -EINVAL;
  }
  }

  {
    int sleep_base = adrv9002_freq_change_timeout_us / 10; /* smaller polling granularity */
    if (sleep_base < 50) sleep_base = 50; /* 50us minimum */
    if (sleep_base > 500) sleep_base = 500; /* cap to 0.5ms */
    ret = wait_for_channel_state(priv, freq->port, channel,
                                 ADI_ADRV9001_CHANNEL_PRIMED,
                                 150, sleep_base, sleep_base * 2);
  }
  if (ret) {
    dev_err(&priv->spi->dev, "Timeout waiting for PRIMED state\n");
    return ret;
  }

  ret = adi_adrv9001_Radio_Channel_Prime(priv->adrv9001Device, freq->port,
                                         channel, false);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't unprime channel: %d\n", ret);
    log_arm_error(priv, "Channel_Prime(false)", ret);
    return ret;
  }

  /* Wait for CALIBRATED state */
  {
    int sleep_base = adrv9002_freq_change_timeout_us / 10;
    if (sleep_base < 50) sleep_base = 50;
    if (sleep_base > 500) sleep_base = 500;
    ret = wait_for_channel_state(priv, freq->port, channel,
                                 ADI_ADRV9001_CHANNEL_CALIBRATED,
                                 300, sleep_base * 2, sleep_base * 4);
  }
  if (ret) {
    dev_err(&priv->spi->dev, "Timeout waiting for CALIBRATED state\n");
    return ret;
  }

  /* Configure carrier */
  
  /* Verify ARM health before critical carrier configuration */
  ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 50000);
  if (ret) {
    dev_err(&priv->spi->dev, "ARM check failed before carrier config: %d\n", ret);
    log_arm_error(priv, "ARM StartStatus before carrier config", ret);
    return ret;
  }
  
  adi_adrv9001_Carrier_t carrier = {
      .loGenOptimization = ADI_ADRV9001_LO_GEN_OPTIMIZATION_PHASE_NOISE,
      .carrierFrequency_Hz = freq->freq_hz,
      .intermediateFrequency_Hz = 0,
      .manualRxport = freq->port};

  ret = adi_adrv9001_Radio_Carrier_Configure(priv->adrv9001Device, freq->port,
                                             channel, &carrier);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't configure carrier: %d\n", ret);
    log_arm_error(priv, "Carrier_Configure", ret);
    return ret;
  }

  /* Increased delay after carrier configuration for ARM stabilization */
  usleep_range(1000, 2000);

  /* Pre-prime sanity: ARM alive and channel calibrated */
  ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 50000);
  if (ret) {
    dev_err(&priv->spi->dev, "ARM not responding before prime: %d\n", ret);
    log_arm_error(priv, "ARM StartStatus before prime", ret);
    return ret;
  }

  ret = adi_adrv9001_Radio_Channel_State_Get(priv->adrv9001Device, freq->port,
                                             channel, &ch_state);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't get channel state before prime: %d\n", ret);
    log_arm_error(priv, "State_Get before prime", ret);
    return ret;
  }
  if (ch_state != ADI_ADRV9001_CHANNEL_CALIBRATED) {
    dev_err(&priv->spi->dev,
            "Channel not CALIBRATED before prime: state=%d\n", ch_state);
    return -EAGAIN;
  }

  ret = adi_adrv9001_Radio_Channel_Prime(priv->adrv9001Device, freq->port,
                                         channel, true);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't prime channel: %d\n", ret);
    log_arm_error(priv, "Channel_Prime(true)", ret);

    /* Retry with exponential backoff to handle ARM timing issues */
    int retry_attempt;
    int max_retries = adrv9002_prime_retry_count;
    if (max_retries < 0) max_retries = 0;
    if (max_retries > 5) max_retries = 5;
    
    for (retry_attempt = 1; retry_attempt <= max_retries; retry_attempt++) {
      /* Exponential backoff: 5-10ms, then 10-15ms, etc */
      usleep_range(5000 * retry_attempt, 5000 * retry_attempt + 5000);

      ret = adi_adrv9001_Radio_Channel_State_Get(priv->adrv9001Device, freq->port,
                                                 channel, &ch_state);
      if (ret) {
        dev_err(&priv->spi->dev, "State read failed before prime retry %d: %d\n", 
                retry_attempt, ret);
        log_arm_error(priv, "State_Get before prime retry", ret);
        if (retry_attempt == max_retries)
          return ret;
        continue;
      }
      if (ch_state != ADI_ADRV9001_CHANNEL_CALIBRATED) {
        dev_err(&priv->spi->dev,
                "Retry %d aborted: channel not CALIBRATED (state=%d)\n", 
                retry_attempt, ch_state);
        if (retry_attempt == max_retries)
          return -EAGAIN;
        continue;
      }

      ret = adi_adrv9001_arm_StartStatus_Check(priv->adrv9001Device, 100000);
      if (ret) {
        dev_err(&priv->spi->dev, "ARM check failed before prime retry %d: %d\n", 
                retry_attempt, ret);
        log_arm_error(priv, "ARM StartStatus before prime retry", ret);
        if (retry_attempt == max_retries)
          return ret;
        continue;
      }

      ret = adi_adrv9001_Radio_Channel_Prime(priv->adrv9001Device, freq->port,
                                             channel, true);
      if (ret == 0) {
        dev_warn(&priv->spi->dev, "Prime succeeded on retry %d after initial error\n", 
                 retry_attempt);
        break;
      }
      
      if (retry_attempt < max_retries) {
        dev_warn(&priv->spi->dev, "Prime retry %d failed: %d, trying again...\n", 
                 retry_attempt, ret);
        log_arm_error(priv, "Channel_Prime(true) retry", ret);
      } else {
        dev_err(&priv->spi->dev, "Prime retry %d failed: %d\n", retry_attempt, ret);
        log_arm_error(priv, "Channel_Prime(true) final retry", ret);
        return ret;
      }
    }
  }

  /* Wait for PRIMED state before enabling RF */
  {
    int sleep_base = adrv9002_freq_change_timeout_us / 10;
    if (sleep_base < 50) sleep_base = 50;
    if (sleep_base > 500) sleep_base = 500;
    ret = wait_for_channel_state(priv, freq->port, channel,
                                 ADI_ADRV9001_CHANNEL_PRIMED,
                                 150, sleep_base, sleep_base * 2);
  }
  if (ret) {
    dev_err(&priv->spi->dev, "Timeout waiting for PRIMED state after prime\n");
    return ret;
  }

  /* Enable RF */
  ret = adi_adrv9001_Radio_Channel_EnableRf(priv->adrv9001Device, freq->port,
                                            channel, true);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't enable RF: %d\n", ret);
    log_arm_error(priv, "EnableRf(true)", ret);
    return ret;
  }

  if (ret == 0) {
    priv->lo_freq[freq->channel] = freq->freq_hz;
    s64 total_time_us = ktime_us_delta(ktime_get(), start_time);
    ADRV9002_LOG_INFO(priv, "Frequency set ch%u: %llu Hz (%lld us)\n",
                      freq->channel, freq->freq_hz, total_time_us);
  }

  return ret;
}

int adrv9002_set_gain(struct adrv9002_priv *priv,
                      struct adrv9002_gain_params *gain) {
  int ret = 0;
  adi_common_ChannelNumber_e channel = ADI_CHANNEL_1;

  /* Serialize against concurrent SET_FREQ/SET_GAIN/ioctls */
  lockdep_assert_held(&priv->lock);

  if (gain->channel >= 2) {
    dev_err(&priv->spi->dev, "Invalid channel: %u\n", gain->channel);
    return -EINVAL;
  }

  ADRV9002_LOG_INFO(priv, "Set gain ch%u: %u\n", gain->channel, gain->gain);

  ret = adi_common_index_to_channel(gain->channel, &channel);
  if (ret) {
    dev_err(&priv->spi->dev, "Invalid channel: %u\n", gain->channel);
    return -EINVAL;
  }

  ret = adi_adrv9001_Rx_Gain_Set(priv->adrv9001Device, channel, gain->gain);
  if (ret) {
    dev_err(&priv->spi->dev, "Can't set gain: %d\n", ret);
    return ret;
  }

  if (ret == 0)
    priv->gain[gain->channel] = gain->gain;

  return ret;
}

int adrv9002_enable_rx(struct adrv9002_priv *priv,
                       struct adrv9002_channel_ctrl *ctrl) {
  int ret = 0;

  if (ctrl->channel >= 2) {
    dev_err(&priv->spi->dev, "Invalid channel: %u\n", ctrl->channel);
    return -EINVAL;
  }

  ADRV9002_LOG_INFO(priv, "RX ch%u: %s\n", ctrl->channel,
                    ctrl->enable ? "enable" : "disable");

  if (ctrl->enable) {
    ret = begin_receiving(priv->adrv9001Device);
  } else {
    ret = stop_receiving(priv->adrv9001Device);
  }

  return ret;
}

int adrv9002_enable_tx(struct adrv9002_priv *priv,
                       struct adrv9002_channel_ctrl *ctrl) {
  int ret = 0;

  if (ctrl->channel >= 2)
    return -EINVAL;

  ADRV9002_LOG_INFO(priv, "TX ch%u: %s\n", ctrl->channel,
                    ctrl->enable ? "enable" : "disable");

  if (ctrl->enable) {
    ret = begin_transmitting(priv->adrv9001Device);
  } else {
    ret = stop_transmitting(priv->adrv9001Device);
  }

  return ret;
}

void adrv9002_free_device(struct adrv9002_priv *priv) {
  if (!priv)
    return;

  kfree(priv->adrv9001Device);
  priv->adrv9001Device = NULL;

  kfree(priv->adrv9001Init);
  priv->adrv9001Init = NULL;
}
