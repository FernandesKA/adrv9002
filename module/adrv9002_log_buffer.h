/**
 * @file adrv9002_log_buffer.h
 * @brief Ring buffer for detailed ADRV9002 frequency change logging
 * @date 2025-12-26
 */

#ifndef ADRV9002_LOG_BUFFER_H
#define ADRV9002_LOG_BUFFER_H

#include <linux/types.h>
#include <linux/spinlock.h>

/* Forward declaration */
struct adrv9002_priv;

/* Ring buffer size - adjust if needed */
#define ADRV9002_LOG_BUFFER_SIZE (32 * 1024)  /* 32KB */

/**
 * @brief Ring buffer structure for storing detailed logs
 */
struct adrv9002_log_buffer {
    char *buffer;           /* Buffer memory */
    size_t size;            /* Total buffer size */
    size_t write_pos;       /* Current write position */
    size_t read_pos;        /* Current read position */
    spinlock_t lock;        /* Lock for thread safety */
    bool enabled;           /* Buffer active flag */
    bool frozen;            /* Stop writing after first failure */
};

/**
 * @brief Initialize the log buffer
 * @param priv Driver private data
 */
void adrv9002_log_buffer_init(struct adrv9002_priv *priv);

/**
 * @brief Free the log buffer
 * @param priv Driver private data
 */
void adrv9002_log_buffer_free(struct adrv9002_priv *priv);

/**
 * @brief Write a formatted message to the log buffer
 * @param priv Driver private data
 * @param fmt Format string (printf-style)
 */
void adrv9002_log_buffer_write(struct adrv9002_priv *priv, const char *fmt, ...);

/**
 * @brief Read contents of the log buffer
 * @param priv Driver private data
 * @param buf Output buffer
 * @param size Size of output buffer
 * @return Number of bytes read
 */
int adrv9002_log_buffer_read(struct adrv9002_priv *priv, char *buf, size_t size);

/**
 * @brief Clear the log buffer
 * @param priv Driver private data
 */
void adrv9002_log_buffer_clear(struct adrv9002_priv *priv);

/**
 * @brief Freeze the buffer (stop writing new logs)
 * @param buf Log buffer pointer
 */
void adrv9002_log_buffer_freeze(struct adrv9002_log_buffer *buf);

/**
 * @brief Check if buffer is enabled
 * @param buf Log buffer pointer
 * @return true if enabled, false otherwise
 */
static inline bool adrv9002_log_buffer_is_enabled(struct adrv9002_log_buffer *buf)
{
    extern bool adrv9002_detailed_logs;
    return adrv9002_detailed_logs && buf && buf->enabled && !buf->frozen;
}

/**
 * @brief Disable the log buffer (e.g., after first reinit)
 * @param buf Log buffer pointer
 */
static inline void adrv9002_log_buffer_disable(struct adrv9002_log_buffer *buf)
{
    if (buf)
        buf->enabled = false;
}

/**
 * @brief Register procfs interface for log buffer
 * @return 0 on success, negative error code on failure
 */
int adrv9002_log_buffer_procfs_init(void);

/**
 * @brief Unregister procfs interface for log buffer
 */
void adrv9002_log_buffer_procfs_exit(void);

/**
 * @brief Set the priv pointer for procfs access
 * @param priv Driver private data (can be NULL)
 */
void adrv9002_log_buffer_set_priv(struct adrv9002_priv *priv);

#endif /* ADRV9002_LOG_BUFFER_H */
