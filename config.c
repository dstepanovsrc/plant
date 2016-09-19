/*
 * Copyright 2016 Dima Stepanov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include "config.h"
#include "plant_limits.h"

enum {
	KEY_PLANDB,
	KEY_PLANPATH,
	MAXNUM_KEYS,
	KEY_PRIVATE_CONFPATH,
	ALLNUM_KEYS
};

/* Describe the configuration values. */
struct config_keys_s {
	char name[CMDNAME_LENGTH];
	char value[MAXNAME_LENGTH];
};

struct config_keys_s config_keys[ALLNUM_KEYS] = {
	{"PlanDB", ""},
	{"PlanPath", ""},
	{"", ""},
	{"KP_CONFPATH", ""}
};

/* Show current configuration. */
static void show_config()
{
	int i;

	printf("Current configuration:\n");
	for (i = 0; i < ALLNUM_KEYS; i++) {
		if (i == MAXNUM_KEYS) {
			printf("\tPrivate keys:\n");
			continue;
		}
		printf("\t%s = %s\n", config_keys[i].name,
				config_keys[i].value);
	}
}

/* Initialize some keys to the default values. */
static void config_init_keys()
{
	memcpy(config_keys[KEY_PLANPATH].value,
			config_keys[KEY_PRIVATE_CONFPATH].value,
			MAXNAME_LENGTH);
}

/* Parse the configuration file and initialize structure. */
static void config_parse_file(FILE *file)
{
	char str[CONFIG_MAXSTR_LENGTH];
	char *name;
	char *val;
	int i;
	struct config_keys_s *conf;

	config_init_keys();
	while (fgets(str, CONFIG_MAXSTR_LENGTH, file)) {
		/* Get parameter name and its value. */
		name = strtok(str, "=\"");
		if (!name) {
			continue;
		}
		val = strtok(NULL, "=\"");
		if (!val) {
			continue;
		}

		conf = config_keys;
		for (i = 0; i < MAXNUM_KEYS; i++) {
			if (!strncmp(conf->name, name,
						strlen(conf->name))) {
				strncpy(conf->value, val, MAXNAME_LENGTH);
				conf->value[MAXNAME_LENGTH - 1] = 0;
				break;
			}
			conf++;
		}
	}
}

/*
 * Set the required value for the key in the configuration file. Return
 * 0 in case of success, otherwise return 1.
 */
static int config_file_set_value(int fd, int key, char *value)
{
	char *buf;
	char *start;
	char *end;
	off_t new_size;
	off_t old_size;
	char arg[CONFIG_MAXSTR_LENGTH];
	int arg_len;
	
	old_size = lseek(fd, 0, SEEK_END);
	if (old_size == (off_t)-1) {
		printf("Can't get the size of the configuration file.\n");
		return 1;
	}
	snprintf(arg, CONFIG_MAXSTR_LENGTH, "%s=\"%s\"\n",
			config_keys[key].name,
			value);
	arg_len = strlen(arg);
	/* 
	 * If required string is truncated, then replace the symbols at
	 * the end of the string.
	 */
	if (arg_len == CONFIG_MAXSTR_LENGTH - 1) {
		arg[arg_len - 2] = '\"';
		arg[arg_len - 1] = '\n';
	}

	/* 
	 * Note that it can be a new argument in file, so the file should
	 * be truncated.
	 */
	new_size = old_size + arg_len;
	if (ftruncate(fd, new_size)) {
		printf("Can't truncate configuration to new %ld size.\n",
				new_size);
		return 1;
	}

	buf = mmap(0, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		printf("Can't mmap configuration file into memory.\n");
		return 1;
	}

	start = buf;
	end = strchr(start, '\n');
	if (!end) {
		end = buf + old_size;
	}
	while (start != end) {
		if (!strncmp(config_keys[key].name, start,
					strlen(config_keys[key].name))) {
			/* 
			 * The required value is found, check the size
			 * of string to change in file. The '\n' character
			 * should also be taken into consideration.
			 */
			end++;
			/* Make enough space for new option and copy it. */
			memmove(start + arg_len, end, old_size - (buf - end));
			memcpy(start, arg, arg_len);
			/*
			 * Don't forget to reduce the file size. Cause
			 * the option is overwritten.
			 */
			if (ftruncate(fd, new_size - (end - start))) {
				printf("Can't truncate configuration to new %ld size.\n",
						new_size - (end - start));
				munmap(buf, new_size);
				return 1;
			}
			break;
		}

		start = end;
		while ((start < (buf + old_size)) && (*start == '\n')) {
			start++;
		}
		if (start >= (buf + old_size)) {
			break;
		}

		/* Find another string in file. */
		end = strchr(start, '\n');
		if (!end) {
			end = buf + old_size;
		}
	}
	if (start >= (buf + old_size)) {
		/*
		 * No option found, in this case just copy data, to the
		 * end of file.
		 */
		memcpy(buf + old_size, arg, arg_len);
	}

	munmap(buf, new_size);

	return 0;
}

/* 
 * Set the root directory for the configuration file. Return 0 in case
 * of success, otherwise, return 1.
 */
static int config_set_private_confpath()
{
	struct passwd *pwd;
	struct stat st;
	char path[CONFIG_PATH_LENGTH];

	/* 
	 * Check that configuration directory exists. If not create
	 * directory.
	 */
	pwd = getpwuid(getuid());
	if (pwd == NULL) {
		printf("Can't find home directory for the current user.\n");
		return 1;
	}
	snprintf(path, CONFIG_PATH_LENGTH, "%s/%s", pwd->pw_dir, CONFIG_DIR);
	if (stat(path, &st)) {
		if (errno != ENOENT) {
			printf("Can't get stat for the %s directory.\n", path);
			return 1;
		}
		if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
			printf("Can't create new %s directory.\n", path);
			return 1;
		}
	} else if (!S_ISDIR(st.st_mode)) {
		printf("Configuration %s path is not a directory.\n", path);
		return 1;
	}
	strncpy(config_keys[KEY_PRIVATE_CONFPATH].value,
			path, MAXNAME_LENGTH);
	config_keys[KEY_PRIVATE_CONFPATH].value[MAXNAME_LENGTH - 1] = 0;

	return 0;
}

/* Return the current path to the plan databases. */
static char *config_get_private_confpath()
{
	return config_keys[KEY_PRIVATE_CONFPATH].value;
}

/*
 * Parse configuration file to obtain main application variables. Return
 * 0 in case of success, otherwise return 1.
 */
int config_init(int show)
{
	char path[CONFIG_PATH_LENGTH];
	FILE *file;

	if (config_set_private_confpath()) {
		return 1;
	}

	/* Open the configuration file. Create it if required. */
	snprintf(path, CONFIG_PATH_LENGTH, "%s/%s",
			config_get_private_confpath(), CONFIG_NAME);
	file = fopen(path, "a+");
	if (file == NULL) {
		printf("Can't open the %s configuration file.\n",
				path);
		return 1;
	}

	config_parse_file(file);

	fclose(file);
	if (show) {
		show_config();
	}

	return 0;
}

/* 
 * Set the "PlanDB" configuration option to the required value. Return 0
 * in case of success, otherwise return 1.
 */
int config_set_plandb(char *name)
{
	char path[CONFIG_PATH_LENGTH];
	int fd;

	snprintf(path, CONFIG_PATH_LENGTH, "%s/%s",
			config_get_private_confpath(), CONFIG_NAME);
	fd = open(path, O_RDWR);
	if (fd == -1) {
		printf("Can't open the %s configuration file.\n",
				path);
		return 1;
	}

	config_file_set_value(fd, KEY_PLANDB, name);

	close(fd);

	strncpy(config_keys[KEY_PLANDB].value, name, MAXNAME_LENGTH);
	config_keys[KEY_PLANDB].value[MAXNAME_LENGTH - 1] = 0;

	return 0;
}

/* Return the current plan name. */
char *config_get_plandb()
{
	return config_keys[KEY_PLANDB].value;
}

/* Return the current path to the plan databases. */
char *config_get_planpath()
{
	return config_keys[KEY_PLANPATH].value;
}
