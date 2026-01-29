/**
 * @file adrv9002_log_buffer.c
 * @brief Ring buffer implementation for detailed ADRV9002 logging
 * @date 2025-12-26
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/ktime.h>
#include <linux/string.h>
#include <linux/math64.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#include "adrv9002.h"
#include "adrv9002_log_buffer.h"

extern bool adrv9002_detailed_logs;

/* Global priv pointer for procfs access */
static struct adrv9002_priv *g_adrv9002_priv = NULL;
static struct proc_dir_entry *g_proc_entry = NULL;

void adrv9002_log_buffer_init(struct adrv9002_priv *priv)
{
    if (!priv)
        return;
    
    priv->log_buffer.buffer = vmalloc(ADRV9002_LOG_BUFFER_SIZE);
    if (!priv->log_buffer.buffer) {
        dev_warn(&priv->spi->dev, "Failed to allocate log buffer\n");
        priv->log_buffer.enabled = false;
        return;
    }
    
    priv->log_buffer.size = ADRV9002_LOG_BUFFER_SIZE;
    priv->log_buffer.write_pos = 0;
    priv->log_buffer.read_pos = 0;
    priv->log_buffer.enabled = true;
    priv->log_buffer.frozen = false;
    spin_lock_init(&priv->log_buffer.lock);
    
    dev_info(&priv->spi->dev, "Detailed log buffer initialized (%zu bytes)\n",
             priv->log_buffer.size);
}

void adrv9002_log_buffer_free(struct adrv9002_priv *priv)
{
    if (!priv)
        return;
        
    if (priv->log_buffer.buffer) {
        vfree(priv->log_buffer.buffer);
        priv->log_buffer.buffer = NULL;
    }
    priv->log_buffer.enabled = false;
    priv->log_buffer.frozen = true;
}

void adrv9002_log_buffer_freeze(struct adrv9002_log_buffer *buf)
{
    unsigned long flags;

    if (!buf)
        return;

    spin_lock_irqsave(&buf->lock, flags);
    buf->frozen = true;
    spin_unlock_irqrestore(&buf->lock, flags);
}

void adrv9002_log_buffer_write(struct adrv9002_priv *priv, const char *fmt, ...)
{
    va_list args;
    unsigned long flags;
    char temp[256];
    int len;
    u64 timestamp_ns;
    u64 timestamp_us;
    u64 ts_sec;
    u64 ts_sub_usec;
    
    if (!priv || !priv->log_buffer.buffer || !priv->log_buffer.enabled || priv->log_buffer.frozen)
        return;
    
    timestamp_ns = ktime_get_real_ns();
    timestamp_us = div64_u64(timestamp_ns, 1000); /* convert ns to us safely */
    ts_sec = div64_u64(timestamp_us, 1000000);
    ts_sub_usec = timestamp_us - ts_sec * 1000000;
    
    va_start(args, fmt);
    len = vsnprintf(temp, sizeof(temp) - 1, fmt, args);
    va_end(args);
    
    if (len <= 0)
        return;
    
    spin_lock_irqsave(&priv->log_buffer.lock, flags);
    
    /* Add timestamp prefix */
    len = snprintf(priv->log_buffer.buffer + priv->log_buffer.write_pos,
                   priv->log_buffer.size - priv->log_buffer.write_pos,
                   "[%llu.%06llu] %s",
                   ts_sec, ts_sub_usec, temp);
    
    if (len > 0) {
        priv->log_buffer.write_pos += len;
        
        /* Wrap around if buffer full */
        if (priv->log_buffer.write_pos >= priv->log_buffer.size - 512) {
            priv->log_buffer.write_pos = 0;
            /* Mark that we wrapped */
            snprintf(priv->log_buffer.buffer, 50, "\n*** BUFFER WRAPPED ***\n");
            priv->log_buffer.write_pos = strlen(priv->log_buffer.buffer);
        }
    }
    
    spin_unlock_irqrestore(&priv->log_buffer.lock, flags);
}

int adrv9002_log_buffer_read(struct adrv9002_priv *priv, char *buf, size_t size)
{
    unsigned long flags;
    size_t len;
    
    if (!priv || !priv->log_buffer.buffer || !buf || size == 0)
        return 0;
    
    spin_lock_irqsave(&priv->log_buffer.lock, flags);
    
    len = min(size - 1, priv->log_buffer.write_pos);
    if (len > 0)
        memcpy(buf, priv->log_buffer.buffer, len);
    buf[len] = '\0';
    
    spin_unlock_irqrestore(&priv->log_buffer.lock, flags);
    
    return len;
}

void adrv9002_log_buffer_clear(struct adrv9002_priv *priv)
{
    unsigned long flags;
    
    if (!priv || !priv->log_buffer.buffer)
        return;
    
    spin_lock_irqsave(&priv->log_buffer.lock, flags);
    priv->log_buffer.write_pos = 0;
    priv->log_buffer.read_pos = 0;
    spin_unlock_irqrestore(&priv->log_buffer.lock, flags);
}

/* ====== PROCFS INTERFACE ====== */

/**
 * Procfs read handler for /proc/adrv9002_log using seq_file
 */
static int adrv9002_log_procfs_show(struct seq_file *m, void *v)
{
    struct adrv9002_priv *priv = m->private;
    unsigned long flags;
    size_t len;

    if (!priv || !priv->log_buffer.buffer)
        return 0;

    spin_lock_irqsave(&priv->log_buffer.lock, flags);

    len = priv->log_buffer.write_pos;
    if (len > 0 && len <= priv->log_buffer.size) {
        seq_write(m, priv->log_buffer.buffer, len);
    }
    
    /* Add buffer status footer */
    seq_printf(m, "\n--- Ring Buffer Status ---\n"
               "Write Position: %zu\n"
               "Buffer Size: %zu\n"
               "Enabled: %d\n"
               "Frozen: %d\n",
               priv->log_buffer.write_pos,
               priv->log_buffer.size,
               priv->log_buffer.enabled,
               priv->log_buffer.frozen);

    spin_unlock_irqrestore(&priv->log_buffer.lock, flags);

    return 0;
}

static int adrv9002_log_procfs_open(struct inode *inode, struct file *file)
{
    return single_open(file, adrv9002_log_procfs_show, g_adrv9002_priv);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops adrv9002_log_proc_ops = {
    .proc_open = adrv9002_log_procfs_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
#else
static const struct file_operations adrv9002_log_proc_ops = {
    .owner = THIS_MODULE,
    .open = adrv9002_log_procfs_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif

void adrv9002_log_buffer_set_priv(struct adrv9002_priv *priv)
{
    g_adrv9002_priv = priv;
}

int adrv9002_log_buffer_procfs_init(void)
{
    /* If entry already exists, reuse it */
    if (g_proc_entry) {
        pr_info("/proc/adrv9002_log already exists, reusing\n");
        return 0;
    }

    g_proc_entry = proc_create("adrv9002_log", 0444, NULL, &adrv9002_log_proc_ops);
    if (!g_proc_entry) {
        pr_err("Failed to create /proc/adrv9002_log\n");
        return -ENOMEM;
    }
    pr_info("Created /proc/adrv9002_log\n");
    return 0;
}

void adrv9002_log_buffer_procfs_exit(void)
{
    if (g_proc_entry) {
        proc_remove(g_proc_entry);
        g_proc_entry = NULL;
    }
    g_adrv9002_priv = NULL;
}
