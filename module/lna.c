#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/slab.h>
#include "lna.h"

/**
 * Parse DT and request GPIO array
 * expected property:
 *      amp-gpios = <&gpioX A>, <&gpioX B>, <&gpioX C>, <&gpioX D>;
 */
int parse_lna_config(struct device *dev, struct lna *lna)
{
    struct gpio_descs *gpios;

    if (!lna)
        return -EINVAL;

    lna->dev = dev;
    lna->available = false;

    gpios = devm_gpiod_get_array_optional(dev, "amp", GPIOD_OUT_LOW);
    if (IS_ERR(gpios)) {
        dev_err(dev, "Failed to get amp GPIOs: %ld\n", PTR_ERR(gpios));
        return PTR_ERR(gpios);
    }

    if (!gpios) {
        dev_warn(dev, "No LNA GPIOs provided in DT\n");
        lna->gpios = NULL;
        lna->num_gpios = 0;
        return 0;
    }

    if (gpios->ndescs > LNA_MAX_GPIOS) {
        dev_warn(dev, "Too many gpios (%d), expected max %d\n",
                 gpios->ndescs, LNA_MAX_GPIOS);
    }

    lna->gpios = gpios;
    lna->num_gpios = gpios->ndescs;
    lna->available = (lna->num_gpios > 0);

    return 0;
}


int lna_init(struct lna *lna)
{
    int i, ret;

    if (!lna)
        return -EINVAL;

    if (!lna->gpios || lna->num_gpios <= 0) {
        lna->available = false;
        return 0;
    }

    for (i = 0; i < lna->num_gpios; i++) {
        ret = gpiod_direction_output(lna->gpios->desc[i], 0);
        if (ret) {
            if (lna->dev)
                dev_err(lna->dev, "Failed to configure GPIO %d as output\n", i);
            else
                pr_err("Failed to configure GPIO %d as output\n", i);
            lna->available = false;
            return ret;
        }
    }

    lna->lna_state = 0;
    lna->available = true;
    return 0;
}

void lna_deinit(struct lna *lna)
{
    int i;

    if (!lna || !lna->available || !lna->gpios)
        return;

    for (i = 0; i < lna->num_gpios; i++)
        gpiod_set_value(lna->gpios->desc[i], 0);
}


/**
 * Enables LNA channels based on mask
 */
int lna_enable(struct lna *lna, u8 mask)
{
    int i;

    if (!lna || !lna->available || !lna->gpios)
        return 0;

    if (lna->num_gpios < 8)
        mask &= (1 << lna->num_gpios) - 1;

    for (i = 0; i < lna->num_gpios; i++) {
        if (mask & (1 << i)) {
            gpiod_set_value(lna->gpios->desc[i], 1);
        }
    }

    lna->lna_state |= mask;
    return 0;
}

/**
 * Disable channels by mask
 */
int lna_disable(struct lna *lna, u8 mask)
{
    int i;

    if (!lna || !lna->available || !lna->gpios)
        return 0;

    if (lna->num_gpios < 8)
        mask &= (1 << lna->num_gpios) - 1;

    for (i = 0; i < lna->num_gpios; i++) {
        if (mask & (1 << i))
            gpiod_set_value(lna->gpios->desc[i], 0);
    }

    lna->lna_state &= ~mask;
    return 0;
}
