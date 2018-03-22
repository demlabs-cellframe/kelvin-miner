#include "kelvin-miner.h"

static bool action = false;

bool read_config_file(char *buf)
{
	FILE *config_file = fopen("../distr/kelvin-miner.cfg", "r");
	if(!config_file) {
		return false;
	}
	fseek(config_file, 0, SEEK_END);
	long buf_size = ftell(config_file);
	fseek(config_file, 0, SEEK_SET);
	if (!buf) {
		buf = malloc(sizeof(char) * (buf_size + 1));
	}
	else {
		char *temp = realloc(buf, sizeof(char) * (buf_size + 1));
		buf = temp;
	}
	if (buf) {
		size_t file_size = fread(buf, buf_size, sizeof(char), config_file);
		buf[file_size++] = '\0';
	}
	else {
		return false;
	}
	fclose(config_file);
	return true;
}

void handle_sig(int sig)
{
	if (sig == SIGINT || sig == SIGTERM) {
		// interrupt or terminate proc
		action = false;
		signal(SIGINT, SIG_DFL);
	} 
	else if (sig == SIGCHLD) {
		// will barely be necessary, but in case, TODO
	}
}

void daemon()
{
	pid_t pid, sid;
	pid = 0; sid = 0;
	int fd;

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

	char pid_c[64];
	
	int pid_fd = open("pid_file", O_APPEND); // filename not fixed yet
	if (pid_fd < 0) {
		// Handle the fact we can't save pid
	}
	itoa(getpid(), pid_c, 10);
	write(pid_fd, pid_c, strlen(pid_c));
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
		daemon();
	}
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	
	char *config_buf;
	if (read_conf_file(config_buf)) {
		// TODO make use of cfg content
	}
	action = true;
	while (action) {
		// TODO stuff, use select / poll for async handling
		sleep(1);
	}
	return EXIT_SUCCESS;
}