#include "dap_kelvin_server.h"

#define LOG_TAG "dap_kelvin_miner_server"

int dap_chain_miner_init() {
    log_it(L_NOTICE, "Initialized Kelvin module");
    return 0;
}

int dap_chain_miner_new(dap_server_t *s_i, const char * s_name)
{
    s_i -> _inheritor = calloc(1,sizeof(dap_chain_miner_t));
    dap_chain_miner_t *s_kelvin_miner = (dap_chain_miner_t *) (s_i)->_inheritor;
    s_kelvin_miner -> server = s_i;
    strncpy(s_kelvin_miner -> server_name, s_name, sizeof(s_kelvin_miner -> server_name) - 1);
    return 0;
}

void dap_chain_miner_delete(dap_server_t *s_i, const char * s_name) {
    //free resources with UT hash? 
}

void dap_chain_miner_add_proc(dap_server_t * si, char *cmd, void *arg,
                                 dap_kelvin_miner_client_callback_t est_new_clb,
                                 dap_kelvin_miner_client_callback_t read_clb,
                                 dap_kelvin_miner_client_callback_t write_clb) {
// ... //
}
