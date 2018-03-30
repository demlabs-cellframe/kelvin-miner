#include "kelvin-miner.h"

#define LOG_TAG "kelvin-miner"
#define CFG_PATH "/opt/dap/etc/"
#define K_DAT kelvin_data ->
#define CFG_GET_ITEM(sec, nm) dap_config_get_item_str(l_config, #sec, #nm);
bool action = false;

static dap_server_t* sh;

const char *s_cfg_path = CFG_PATH;
const char *s_log_file = CFG_PATH "kelvin-log";
const char *s_cfg_file = CFG_PATH "kelvin-miner.cfg";
static FILE* file_gold;
static FILE* file_silver;
static FILE* file_copper;

int kelvin_blocks_mined 		= 0;
int kelvin_blocks_mined_copper 	= 0;
int kelvin_blocks_mined_silver	= 0;
int kelvin_blocks_mined_gold 	= 0;

void kelvin_init() {
	dap_common_init(s_log_file);
    dap_config_init(s_cfg_path);
    dap_enc_init();
    dap_chain_init();
}

void log_on_terminate() {
	log_it(L_INFO, "Total hashes for gold %s B", ftell(file_gold) );
	fclose(file_gold);

    log_it(L_INFO, "Total hashes for silver %s B", ftell(file_silver));
    fclose(file_silver);

    log_it(L_INFO, "Total hashes for copper %s B", ftell(file_copper));
    fclose(file_copper);

    log_it(L_INFO, "Total blocks mined %s ", kelvin_blocks_mined);
    log_it(L_INFO, "Gold blocks mined %s ", kelvin_blocks_mined_copper);
    log_it(L_INFO, "Silver blocks mined %s ", kelvin_blocks_mined_silver);
    log_it(L_INFO, "Copper blocks mined %s ", kelvin_blocks_mined_gold);
    log_it(L_INFO, "Total mining speed %s ", total_hashes_in_minute / kelvin_blocks_mined);
}

pid_t daemon()
{
	pid_t pid, sid;
	pid = 0; sid = 0;
	pid = fork();
	if (pid < 0) {
		// Child was not born, handle it someway, not obligatory fail.
	}
	else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}
	umask(0);
	chdir("/");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return getpid();
}

void handle_sig(int sig)
{
	if (sig == SIGINT || sig == SIGTERM) {
		action = false;
		log_on_terminate();
        if (sh) {
            dap_server_delete(sh);
        }
		signal(SIGINT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
	} 
	else if (sig == SIGCHLD) {
		// will barely be necessary, but in case, TODO
		signal(SIGCHLD, SIG_IGN);
	}
}

void *pt_kelvin_mine(void *pt_arg) {
    pt_data_t *kelvin_data = (pt_data_t *)pt_arg;
	file_copper = fopen("kelvin_block_copper.txt", "w");
	file_silver = fopen("kelvin_block_silver.txt", "w");
	file_gold 	= fopen("kelvin_block_gold.txt", "w");
	if (!file_copper || !file_silver || !file_gold) {
		log_it(L_INFO, "Error: couldn't open file to save blocks!");
        pthread_exit(NULL);
	}
	K_DAT l_chain_name 			= CFG_GET_ITEM(general, name);
    K_DAT l_chain_file_storage 	= CFG_GET_ITEM(general, file_storage);
    K_DAT l_chain_file_cache 	= CFG_GET_ITEM(general, file_cache);
	if (K_DAT l_chain_name) {
    	K_DAT l_chain = dap_chain_open(K_DAT l_chain_file_storage, K_DAT l_chain_file_cache);
	}
	for ( ; ; ) {
		K_DAT l_block_cache = dap_chain_allocate_next_block(K_DAT l_chain);
		if ( dap_chain_mine_block(K_DAT l_block_cache, false, 0) == 0 ) {
			++kelvin_blocks_mined;
			char *l_hash_str = dap_chain_hash_to_str_new(&(K_DAT l_block_cache) -> block_hash);
        	log_it(L_INFO, "Kelvin block mined with hash %s ", l_hash_str);
			if (dap_chain_hash_kind_check(K_DAT l_block_cache, K_DAT l_block_cache -> block -> header.difficulty) == HASH_GOLD) {
                fwrite(K_DAT l_block_cache -> block, K_DAT l_block_cache -> block -> header.size, 1, file_gold);
            	++kelvin_blocks_mined_gold;
            }
			else if (dap_chain_hash_kind_check(K_DAT l_block_cache, K_DAT l_block_cache -> block -> header.difficulty) == HASH_SILVER) {
                fwrite(K_DAT l_block_cache -> block, K_DAT l_block_cache -> block -> header.size, 1, file_silver);
            	++kelvin_blocks_mined_silver;
            }
			else if (dap_chain_hash_kind_check(K_DAT l_block_cache, K_DAT l_block_cache -> block -> header.difficulty) == HASH_COPPER) {
                fwrite(K_DAT l_block_cache -> block, K_DAT l_block_cache -> block -> header.size, 1, file_copper);
            	++kelvin_blocks_mined_copper;
            }
			dap_chain_block_cache_dump(K_DAT l_block_cache);
            DAP_DELETE(l_hash_str);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	const struct option long_options[] = {
		{"daemon", no_argument, NULL, 'd'},
		{NULL, 0, NULL, 0}
	};
	int val;
	int option_index = 0;
	bool daemon_action = false;
	while ((val = getopt_long(argc, argv, "d", long_options, &option_index)) != -1) {
		switch (val) {
			case 'd':
				daemon_action = true;
				break;
			case '?':
				break;
			default:
				break;
		}
	}
	dap_config_t *l_config = dap_config_open(s_cfg_file);
	if (daemon_action) {
		pid_t pid = daemon();
		if (l_config) {
			const char *pid_file = CFG_GET_ITEM(general, pid_file);
		}
		int pid_fd = open(pid_file, O_APPEND);
		if (pid_fd < 0) {
			// Handle the fact we can't save pid
		}
		char *pid_c = itoa(pid);
		write(pid_fd, pid_c, strlen(pid_c));
		fclose(pid_fd);
	}
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	
	pt_data_t *kelvin_data;
	K_DAT l_cfg = l_config;
    pthread_t tid;
    pthread_create(&tid, NULL, pt_kelvin_mine, (void *)&kelvin_data);
    if (!pthread_detach(tid)) {
		// to this end there's no more use of kelvin struct ... 
	}
	return EXIT_SUCCESS;
}
