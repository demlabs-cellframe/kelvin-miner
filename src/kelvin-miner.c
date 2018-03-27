#include "kelvin-miner.h"

bool action = false;
static dap_server_t* sh;

void *pt_block_new(void *pt_arg) {
    pt_data *data = (pt_data *)pt_arg;
    data -> l_block = (dap_chain_block_t *)calloc(1, sizeof(data -> l_block -> header));
    if(!(data -> l_block)) {
         pthread_exit(NULL);
    }
    else {
        data -> l_block -> header.signature = DAP_CHAIN_BLOCK_SIGNATURE;
        data -> l_block -> header.version = 1;
        data -> l_block -> header.timestamp = time(NULL);
        if (data -> l_block_cache -> l_block) {
            memcpy(&(data -> l_block) -> header.prev_block, data -> l_block_cache -> l_block, sizeof(data -> l_block -> header.prev_block));
        }
        else {
             memset(&(data -> l_block) -> header.prev_block, 0xff, sizeof(data -> l_block -> header.prev_block));
        }
        data -> l_block -> header.size = sizeof(data -> l_block -> header);
        pthread_exit(NULL); // how, or if, should we tell this fromblock alloc failure ?
        //return (void *)(data -> l_block);  // 
    }
}

void handle_sig(int sig)
{
	if (sig == SIGINT || sig == SIGTERM) {
		// interrupt or terminate proc
		action = false;
        if (sh) {
            dap_server_delete(sh);
        }
		signal(SIGINT, SIG_DFL);
	} 
	else if (sig == SIGCHLD) {
		// will barely be necessary, but in case, TODO
	}
}

pid_t daemon()
{
	pid_t pid, sid;
	pid = 0; sid = 0;
	pid = fork();
	if (pid < 0) {
		// Child was not born, handle it someway, not obligatory fail.
	}
	if (pid > 0) {
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
	if (daemon_action) {
		pid_t pid = daemon();
		char pid_c[64];
		dap_config_t *l_config = dap_config_open(l_config_name);
		if (l_config) {
			const char *pid_file = dap_config_get_item_str(l_config, "general", "pid_file");
		}
		int pid_fd = open(pid_file, O_APPEND);
		if (pid_fd < 0) {
			// Handle the fact we can't save pid
		}
		itoa(pid, pid_c, 10);
		write(pid_fd, pid_c, strlen(pid_c));
		fclose(pid_fd);
	}
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
    pthread_t tid;
    pt_data data;
    // To this end here must be some 'data -> l_block_cache' init for further use.
    pthread_create(&tid, NULL, pt_block_new, (void *)&data);
/*  dap_chain_block_t *l_b;
    pthread_join(tid, (void *)&lb); */ // TODO - resolve which ret handling suits better here. I recommend the current.
    pthread_join(tid, NULL);
    if (data -> l_block) {
        // pass it thru
    }
    char addr[] = "127.0.0.1";
    uint16_t port = 8888;
    dap_server_type_t s_type = DAP_SERVER_TCP;
    sh = dap_server_listen(addr, port, s_type);
    if (sh) {
        dap_server_init();
	    dap_server_loop(sh);
        action = true;
    }
    else {
        // TODO - handle this
    }
	return EXIT_SUCCESS;
}
