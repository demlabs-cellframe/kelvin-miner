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

bool read_config_file(char *);

void handle_sig(int);

void daemon();

