#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include "dap_config.h"

extern bool action;
extern const char *s_cfg_path;
extern const char *s_log_file;
extern int kelvin_blocks_mined;
extern int kelvin_blocks_mined_copper;
extern int kelvin_blocks_mined_silver;
extern int kelvin_blocks_mined_gold;

typedef struct pt_data {
    dap_config_t *l_cfg;
    char *l_chain_name;
    char *l_chain_file_storage;
    char *l_chain_file_cache;
    dap_chain_t *l_chain;
    dap_chain_block_cache_t *l_block_cache;
    dap_chain_block_t *l_block;
} pt_data_t;

void kelvin_init();
void log_on_terminate();
void handle_sig(int);
void daemon();

