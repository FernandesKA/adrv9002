#ifndef LNA_H
#define LNA_H

#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/device.h>

#define LNA_MAX_GPIOS 4

struct lna {
    struct device *dev;
    struct gpio_descs *gpios;
    u8 lna_state;
    int num_gpios;
    bool available;
};

int parse_lna_config(struct device *dev, struct lna *lna);
int lna_init(struct lna *lna);
void lna_deinit(struct lna *lna);
int lna_enable(struct lna *lna, u8 mask);
int lna_disable(struct lna *lna, u8 mask);

#endif // LNA_H
