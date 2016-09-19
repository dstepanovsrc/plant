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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "config.h"
#include "database.h"

/* 
 * Add new plan in the home directory. Return 0 in case of success,
 * otherwise return 1.
 */
int plan_add(int argc, char *argv[])
{
	char path[CONFIG_PATH_LENGTH];
	struct stat st;
	int fd;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	snprintf(path, CONFIG_PATH_LENGTH, "%s/%s",
			config_get_planpath(),
			argv[0]);
	if (!stat(path, &st)) {
		printf("Plan with the \"%s\" name exists. Use another name.\n",
				argv[0]);
		return 1;
	}
	if (errno != ENOENT) {
		printf("Can't create plan %s in the %s directory.\n",
				argv[0], config_get_planpath());
		return 1;
	}

	/* Create file. */
	fd = open(path, O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1) {
		printf("Can't create plan %s in the %s directory.\n",
				argv[0], config_get_planpath());
		return 1;
	}
	close(fd);

	/* Set current plandb to the required name. */
	if (config_set_plandb(argv[0])) {
		return 1;
	}

	/* Add header to the plandb file. */
	if (plandb_header_add()) {
		return 1;
	}
	printf("Plan \"%s\" created successfully.\n", argv[0]);

	return 0;
}

/*
 * Display current plan name. Return 0 in case of success, otherwise
 * return 1.
 */
int plan_current(int argc, char *argv[])
{
	printf("Plan: \"%s\"\n", config_get_plandb());

	return 0;
}

/*
 * Switch to another plan. Return 0 in case of success, otherwise
 * return 1.
 */
int plan_switch(int argc, char *argv[])
{
	char path[CONFIG_PATH_LENGTH];
	struct stat st;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	snprintf(path, CONFIG_PATH_LENGTH, "%s/%s",
			config_get_planpath(),
			argv[0]);
	if (stat(path, &st)) {
		printf("There is no \"%s\" plan name.\n", argv[0]);
		return 1;
	}
	if (!S_ISREG(st.st_mode)) {
		printf("The \"%s\" name is not a plan name, is not a regular file.\n",
				argv[0]);
		return 1;
	}

	/* Set current plandb to the required name. */
	if (config_set_plandb(argv[0])) {
		return 1;
	}
	printf("Plan switch successfull: \"%s\"\n", config_get_plandb());

	return 0;
}

/*
 * Show current plan with all its projects and tasks. Return 0 in case
 * of success, otherwise return 1.
 */
int plan_show(int argc, char *argv[])
{
	plandb_item_show(0, ITEM_PROJECT, NULL);

	return 0;
}

/*
 * Edit current plan. Return 0 in case of success, otherwise
 * return 1.
 */
int plan_edit(int argc, char *argv[])
{
	printf("%s:%d: Unsupported command\n", __FUNCTION__, __LINE__);

	return 1;
}
