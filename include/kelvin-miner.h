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

typedef struct pt_data {
    dap_chain_block_cache_t *l_block_cache;
    dap_chain_block_t *l_block;
} pt_data;


void handle_sig(int);

void daemon();

