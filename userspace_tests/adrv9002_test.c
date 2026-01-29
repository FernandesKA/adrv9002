/**
 * @file adrv9002_test.c
 * @author FernandezKA
 * @brief Userspace test application for ADRV9002 driver
 * @version 0.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <getopt.h>

#include "adrv9002_ioctl.h"

#define DEVICE_PATH "/dev/adrv9002"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

/* Global device file descriptor */
static int g_fd = -1;

/* Default paths */
static const char *g_profile_path = "./etc/pfile.json";
static const char *g_stream_path = "./etc/stream.bin";

/**
 * Print error message with color
 */
static void print_error(const char *msg)
{
    fprintf(stderr, COLOR_RED "[ERROR] %s: %s\n" COLOR_RESET, msg, strerror(errno));
}

/**
 * Print info message with color
 */
static void print_info(const char *msg)
{
    printf(COLOR_GREEN "[INFO] %s\n" COLOR_RESET, msg);
}

/**
 * Print warning message with color
 */
static void print_warn(const char *msg)
{
    printf(COLOR_YELLOW "[WARN] %s\n" COLOR_RESET, msg);
}

/**
 * Open device node
 */
static int adrv9002_open(void)
{
    g_fd = open(DEVICE_PATH, O_RDWR);
    if (g_fd < 0) {
        print_error("Failed to open device");
        return -1;
    }
    
    printf(COLOR_BLUE "Device opened: %s (fd=%d)\n" COLOR_RESET, DEVICE_PATH, g_fd);
    return 0;
}

/**
 * Close device node
 */
static void adrv9002_close(void)
{
    if (g_fd >= 0) {
        close(g_fd);
        print_info("Device closed");
        g_fd = -1;
    }
}

/**
 * Initialize ADRV9002
 */
static int adrv9002_init(const char *profile, const char *stream)
{
    struct adrv9002_init_params params;
    int ret;

    memset(&params, 0, sizeof(params));

    char *profile_buf = malloc(64 * 1024);
    if (!profile_buf) {
        print_error("Failed to allocate memory for profile");
        return -1;
    }

    FILE *fp = fopen(profile, "rb");
    if (!fp) {
        print_error("Failed to open profile file");
        free(profile_buf);
        return -1;
    }
    size_t profile_buf_size = fread(profile_buf, 1, 64 * 1024, fp);
    fclose(fp);

    char *stream_buf = malloc(64 * 1024);
    if (!stream_buf) {
        print_error("Failed to allocate memory for stream");
        free(profile_buf);
        return -1;
    }

    FILE *fp2 = fopen(stream, "rb");
    if (!fp2) {
        print_error("Failed to open stream file");
        free(profile_buf);
        free(stream_buf);
        return -1;
    }
    size_t stream_buf_size = fread(stream_buf, 1, 64 * 1024, fp2);
    fclose(fp2);

    printf("\n" COLOR_BLUE ">>> Initializing ADRV9002\n" COLOR_RESET);
    printf("    Profile: %s\n", profile);
    printf("    Stream:  %s\n", stream);
    printf("Read profile size: %zu bytes\n", profile_buf_size);
    printf("Read stream size: %zu bytes\n", stream_buf_size);

    params.profile_buf = profile_buf;
    params.profile_buf_len = profile_buf_size;
    params.stream_buf = stream_buf;
    params.stream_buf_len = stream_buf_size;

    ret = ioctl(g_fd, ADRV9002_IOC_INIT, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_INIT failed");
        free(profile_buf);
        free(stream_buf);
        return -1;
    }

    print_info("ADRV9002 initialized successfully");

    free(profile_buf);
    free(stream_buf);

    return 0;
}


/**
 * Set frequency for specific channel
 */
static int adrv9002_set_frequency(uint8_t channel, uint8_t port, uint64_t freq_hz)
{
    struct adrv9002_freq_params params;
    int ret;
    
    memset(&params, 0, sizeof(params));
    params.channel = channel;
    params.port = port;
    params.freq_hz = freq_hz;
    
    printf("\n" COLOR_BLUE ">>> Setting frequency\n" COLOR_RESET);
    printf("    Channel: %u, Port: %u, Frequency: %llu Hz (%.2f MHz)\n",
           channel, port, (unsigned long long)freq_hz, freq_hz / 1e6);
    
    ret = ioctl(g_fd, ADRV9002_IOC_SET_FREQ, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_SET_FREQ failed");
        return -1;
    }
    
    print_info("Frequency set successfully");
    return 0;
}

/**
 * Get current frequency
 */
static int adrv9002_get_frequency(uint8_t channel, uint8_t port)
{
    struct adrv9002_freq_params params;
    int ret;
    
    memset(&params, 0, sizeof(params));
    params.channel = channel;
    params.port = port;
    
    printf("\n" COLOR_BLUE ">>> Getting frequency\n" COLOR_RESET);
    printf("    Channel: %u, Port: %u\n", channel, port);
    
    ret = ioctl(g_fd, ADRV9002_IOC_GET_FREQ, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_GET_FREQ failed");
        return -1;
    }
    
    printf(COLOR_GREEN "    Current frequency: %llu Hz (%.2f MHz)\n" COLOR_RESET,
           (unsigned long long)params.freq_hz, params.freq_hz / 1e6);
    return 0;
}

/**
 * Set gain for specific channel
 */
static int adrv9002_set_gain(uint8_t channel, uint8_t port, uint32_t gain)
{
    struct adrv9002_gain_params params;
    int ret;
    
    memset(&params, 0, sizeof(params));
    params.channel = channel;
    params.port = port;
    params.gain = gain;
    
    printf("\n" COLOR_BLUE ">>> Setting gain\n" COLOR_RESET);
    printf("    Channel: %u, Port: %u, Gain: %u\n", channel, port, gain);
    
    ret = ioctl(g_fd, ADRV9002_IOC_SET_GAIN, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_SET_GAIN failed");
        return -1;
    }
    
    print_info("Gain set successfully");
    return 0;
}

/**
 * Enable/Disable RX
 */
static int adrv9002_rx_control(uint8_t channel, uint8_t enable)
{
    struct adrv9002_channel_ctrl params;
    int ret;
    
    memset(&params, 0, sizeof(params));
    params.channel = channel;
    params.enable = enable;
    
    printf("\n" COLOR_BLUE ">>> %s RX\n" COLOR_RESET, enable ? "Enabling" : "Disabling");
    printf("    Channel: %u\n", channel);
    
    ret = ioctl(g_fd, ADRV9002_IOC_RX_ENABLE, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_RX_ENABLE failed");
        return -1;
    }
    
    printf(COLOR_GREEN "    RX %s successfully\n" COLOR_RESET,
           enable ? "enabled" : "disabled");
    return 0;
}

/**
 * Enable/Disable TX
 */
static int adrv9002_tx_control(uint8_t channel, uint8_t enable)
{
    struct adrv9002_channel_ctrl params;
    int ret;
    
    memset(&params, 0, sizeof(params));
    params.channel = channel;
    params.enable = enable;
    
    printf("\n" COLOR_BLUE ">>> %s TX\n" COLOR_RESET, enable ? "Enabling" : "Disabling");
    printf("    Channel: %u\n", channel);
    
    ret = ioctl(g_fd, ADRV9002_IOC_TX_ENABLE, &params);
    if (ret < 0) {
        print_error("ADRV9002_IOC_TX_ENABLE failed");
        return -1;
    }
    
    printf(COLOR_GREEN "    TX %s successfully\n" COLOR_RESET,
           enable ? "enabled" : "disabled");
    return 0;
}

/**
 * Reset device
 */
static int adrv9002_reset(void)
{
    int ret;
    
    printf("\n" COLOR_BLUE ">>> Resetting device\n" COLOR_RESET);
    
    ret = ioctl(g_fd, ADRV9002_IOC_RESET);
    if (ret < 0) {
        print_error("ADRV9002_IOC_RESET failed");
        return -1;
    }
    
    print_info("Device reset successfully");
    return 0;
}

/**
 * Enable LNA
 */
static int adrv9002_lna_enable(uint8_t mask)
{
    int ret;

    printf("\n" COLOR_BLUE ">>> Enabling LNA\n" COLOR_RESET);
    printf("    Mask: 0x%02X\n", mask);

    ret = ioctl(g_fd, ADRV9002_IOC_LNA_ENABLE, &mask);
    if (ret < 0) {
        print_error("ADRV9002_IOC_LNA_ENABLE failed");
        return -1;
    }

    print_info("LNA enabled successfully");
    return 0;
}

/**
 * Disable LNA
 */
static int adrv9002_lna_disable(uint8_t mask)
{
    int ret;

    printf("\n" COLOR_BLUE ">>> Disabling LNA\n" COLOR_RESET);
    printf("    Mask: 0x%02X\n", mask);

    ret = ioctl(g_fd, ADRV9002_IOC_LNA_DISABLE, &mask);
    if (ret < 0) {
        print_error("ADRV9002_IOC_LNA_DISABLE failed");
        return -1;
    }

    print_info("LNA disabled successfully");
    return 0;
}


/**
 * Interactive menu
 */
static void print_menu(void)
{
    printf("\n" COLOR_BLUE "=== ADRV9002 Test Application ===" COLOR_RESET "\n");
    printf("1. Initialize device\n");
    printf("2. Set frequency\n");
    printf("3. Get frequency\n");
    printf("4. Set gain\n");
    printf("5. Enable RX\n");
    printf("6. Disable RX\n");
    printf("7. Enable TX\n");
    printf("8. Disable TX\n");
    printf("9. Reset device\n");
    printf("10. Enable LNA\n");
    printf("11. Disable LNA\n");
    printf("0. Exit\n");
    printf("Select option: ");
}

/**
 * Print usage information
 */
static void print_usage(const char *prog)
{
    printf("Usage: %s [OPTIONS]\n", prog);
    printf("\nOptions:\n");
    printf("  -p <path>    Path to profile config file (default: ./etc/pfile.json)\n");
    printf("  -s <path>    Path to stream/ARM binary file (default: ./etc/stream.bin)\n");
    printf("  -h           Show this help message\n");
    printf("\nExample:\n");
    printf("  %s -p /path/to/profile.json -s /path/to/stream.bin\n", prog);
}

/**
 * Main function
 */
int main(int argc, char *argv[])
{
    int choice;
    char input[256];
    uint64_t freq;
    uint32_t gain;
    uint8_t channel, port;
    int opt;
    
    /* Show help if no arguments provided */
    if (argc == 1) {
        print_usage(argv[0]);
        printf("\nNote: Running with default paths.\n");
        printf("Press Enter to continue or Ctrl+C to exit...\n");
        getchar();
    }
    
    /* Parse command line arguments */
    while ((opt = getopt(argc, argv, "p:s:h")) != -1) {
        switch (opt) {
        case 'p':
            g_profile_path = optarg;
            break;
        case 's':
            g_stream_path = optarg;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }
    
    printf(COLOR_BLUE);
    printf("╔════════════════════════════════════════╗\n");
    printf("║   ADRV9002 Kernel Driver Test App      ║\n");
    printf("║   Version 0.1                          ║\n");
    printf("║   Author: FernandezKA                  ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    printf("\nConfiguration:\n");
    printf("  Profile path: %s\n", g_profile_path);
    printf("  Stream path:  %s\n", g_stream_path);
    
    /* Open device */
    if (adrv9002_open() < 0) {
        return 1;
    }
    
    /* Interactive loop */
    while (1) {
        print_menu();
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        choice = atoi(input);
        
        switch (choice) {
        case 1:
            /* Initialize - use paths from command line or defaults */
            adrv9002_init(g_profile_path, g_stream_path);
            break;
            
        case 2:
            /* Set frequency */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            printf("Enter port (0-1): ");
            fgets(input, sizeof(input), stdin);
            port = atoi(input);
            
            printf("Enter frequency (Hz): ");
            fgets(input, sizeof(input), stdin);
            freq = strtoull(input, NULL, 10);
            
            adrv9002_set_frequency(channel, port, freq);
            break;
            
        case 3:
            /* Get frequency */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            printf("Enter port (0-1): ");
            fgets(input, sizeof(input), stdin);
            port = atoi(input);
            
            adrv9002_get_frequency(channel, port);
            break;
            
        case 4:
            /* Set gain */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            printf("Enter port (0-1): ");
            fgets(input, sizeof(input), stdin);
            port = atoi(input);
            
            printf("Enter gain (0-255): ");
            fgets(input, sizeof(input), stdin);
            gain = atoi(input);
            
            adrv9002_set_gain(channel, port, gain);
            break;
            
        case 5:
            /* Enable RX */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            adrv9002_rx_control(channel, 1);
            break;
            
        case 6:
            /* Disable RX */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            adrv9002_rx_control(channel, 0);
            break;
            
        case 7:
            /* Enable TX */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            adrv9002_tx_control(channel, 1);
            break;
            
        case 8:
            /* Disable TX */
            printf("Enter channel (0-1): ");
            fgets(input, sizeof(input), stdin);
            channel = atoi(input);
            
            adrv9002_tx_control(channel, 0);
            break;
            
        case 9:
            /* Reset */
            adrv9002_reset();
            break;

        case 10: {
            uint8_t mask;
            printf("Enter LNA mask (hex, e.g. FF): ");
            fgets(input, sizeof(input), stdin);
            mask = strtoul(input, NULL, 16);
            adrv9002_lna_enable(mask);
            break;
        }
        
        case 11: {
            uint8_t mask;
            printf("Enter LNA mask (hex, e.g. FF): ");
            fgets(input, sizeof(input), stdin);
            mask = strtoul(input, NULL, 16);
            adrv9002_lna_disable(mask);
            break;
        }
            
        case 0:
            print_info("Exiting...");
            goto cleanup;
            
        default:
            print_warn("Invalid option");
        }
    }
    
cleanup:
    adrv9002_close();
    return 0;
}
