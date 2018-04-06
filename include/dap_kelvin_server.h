#pragma once

#include "dap_server.h"
#include "dap_client.h"
#include "uthash.h"


typedef struct dap_chain_miner {
    dap_server_t * server;
    char server_name[256];
} dap_chain_miner_t;

typedef struct dap_chain_miner_processor {
    dap_chain_miner_t *si;
    char *cmd;
    void * _inheritor;
    // UT_hash_handle hh;
    dap_kelvin_miner_client_callback_t new_clb;
    dap_kelvin_miner_client_callback_t read_clb;
    dap_kelvin_miner_client_callback_t write_clb;
} dap_chain_miner_processor_t;

int     dap_chain_miner_new(dap_server_t *, const char *);
void    dap_chain_miner_delete(dap_server_t *, void *);
int     dap_chain_miner_init();
void    dap_chain_miner_deinit();
void    dap_chain_miner_add_proc(dap_server_t *, char *, void *,
                                 dap_kelvin_miner_client_callback_t,
                                 dap_kelvin_miner_client_callback_t,
                                 dap_kelvin_miner_client_callback_t);