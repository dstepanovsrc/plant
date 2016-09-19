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
#include <string.h>
#include "plant_limits.h"
#include "config.h"
#include "database.h"
#include "plan.h"
#include "project.h"
#include "task.h"

#define MAXOBJ_CMDS 3
#define MAXPLAN_CMDS 5
#define MAXPROJ_CMDS 4
#define MAXTASK_CMDS 15

/* Main types to parse. */
enum {
	PARSE_OBJ,
	PARSE_PLAN,
	PARSE_PROJECT,
	PARSE_TASK
};

/* Structure to parse the command line. */
struct plant_cmd_s {
	char name[CMDNAME_LENGTH];
	int (*func)(int argc, char *argv[]);
};

/* Define general commands to parse the command line. */
static int parse_plan(int argc, char *argv[]);
static int parse_project(int argc, char *argv[]);
static int parse_task(int argc, char *argv[]);

static struct plant_cmd_s obj_cmds[MAXOBJ_CMDS] = {
	{"plan", parse_plan},
	{"project", parse_project},
	{"task", parse_task}
};

static struct plant_cmd_s plan_cmds[MAXPLAN_CMDS] = {
	{"add", plan_add},
	{"current", plan_current},
	{"switch", plan_switch},
	{"show", plan_show},
	{"edit", plan_edit}
};

static struct plant_cmd_s project_cmds[MAXPROJ_CMDS] = {
	{"add", project_add},
	{"remove", project_remove},
	{"show", project_show},
	{"rename", project_rename}
};

static struct plant_cmd_s task_cmds[MAXTASK_CMDS] = {
	{"add", task_add},
	{"remove", task_remove},
	{"current", task_current},
	{"show", task_show},
	{"active", task_active},
	{"suspend", task_suspend},
	{"complete", task_complete},
	{"start", task_start},
	{"stop", task_stop},
	{"assign", task_assign},
	{"track", task_track},
	{"estimate", task_estimate},
	{"progress", task_progress},
	{"rename", task_rename},
	{"removetime", task_removetime}
};

/* Print to stdout how to use the application. */
static void usage(char *app)
{
	printf("Usage: %s <object> <command> <args>\n", app);

	return;
	printf("Possible objects:\n");
	printf("\tplan\t- it is the main object to work with. All the\n");
	printf("\t\t  projects and tasks should be in the plan. There\n");
	printf("\t\t  can be more than one plan. Switch between plans\n");
	printf("\t\t  to change working projects and tasks.\n");
	printf("\tproject\t- object to work with projects. Each plan\n");
	printf("\t\t  can have different number of projects: from 0\n");
	printf("\t\t  to any other number. Each project contains\n");
	printf("\t\t  different number of projects and tasks in it.\n");
	printf("\t\t  No resource can be assigned to the project.\n");
	printf("\ttask\t- object to work with tasks. This is the main\n");
	printf("\t\t  object to track. Every task can be part of the\n");
	printf("\t\t  project or without any project at all. Resources\n");
	printf("\t\t  can be assigned to work with the task.\n");

	printf("\nCommand list to work with plan:\n");
	printf("\tadd\t- add new plan in the home directory\n");
	printf("\t\t  add <name>\n");
	printf("\tremove\t- remove plan from the home directory\n");
	printf("\t\t  remove <name>\n");
	printf("\tcurrent\t- display current plan name\n");
	printf("\t\t  current\n");
	printf("\tlist\t- list all plans in the home directory\n");
	printf("\t\t  list plan\n");
	printf("\tswitch\t- switch to another plan\n");
	printf("\t\t  switch <name>\n");
	printf("\tshow\t- show current plan\n");
	printf("\t\t  show plan <name>\n");
	printf("\tedit\t- edit current plan\n");
	printf("\t\t  edit name <newname>\n");

	printf("\n");
	printf("Command list to work with projects in the current plan:\n");
	printf("\tadd\t- add project\n");
	printf("\t\t  add <[project:]name>\n");
	printf("\tremove\t- remove project\n");
	printf("\t\t  remove <name>\n");
	printf("\tshow\t- show required project and its tasks\n");
	printf("\t\t  show <name>\n");
	printf("\tedit\t- edit required project\n");
	printf("\t\t  edit <name> name <newname>\n");

	printf("\n");
	printf("Command list to work with tasks in the current plan:\n");
	printf("\tadd\t- add task\n");
	printf("\t\t  add <[project:]name>\n");
	printf("\tremove\t- remove task\n");
	printf("\t\t  remove <name>\n");
	printf("\tcurrent\t- display current task (or tasks)\n");
	printf("\t\t  current\n");
	printf("\tshow\t- show required task and its options\n");
	printf("\t\t  show <name>\n");
	printf("\ttrack\t- track the required task\n");
	printf("\t\t  track <name> <stime> <etime>\n");
	printf("\tstart\t- start the required task\n");
	printf("\t\t  start <name>\n");
	printf("\tstop\t- stop the required task\n");
	printf("\t\t  stop <name>\n");
	printf("\tassign\t- assign resource to the task\n");
	printf("\t\t  assign <name> <resource>\n");
	printf("\tedit\t- edit required task\n");
	printf("\t\t  edit <name> name <newname>\n");
	printf("\t\t  edit <name> stime <newstime>\n");
	printf("\t\t  edit <name> etime <newetime>\n");
	printf("\t\t  edit <name> resource <newresource>\n");
}

/* Main routine to parse the command line of the application. */
static int parse_cmd(int argc, char *argv[], int type)
{
	int i;
	int len;
	struct plant_cmd_s *cmds;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	switch (type) {
	case PARSE_OBJ:
		len = MAXOBJ_CMDS;
		cmds = obj_cmds;
		break;
	case PARSE_PLAN:
		len = MAXPLAN_CMDS;
		cmds = plan_cmds;
		break;
	case PARSE_PROJECT:
		len = MAXPROJ_CMDS;
		cmds = project_cmds;
		break;
	case PARSE_TASK:
		len = MAXTASK_CMDS;
		cmds = task_cmds;
		break;
	default:
		printf("Wrong type to parse.\n");
		return 1;
	}

	/* Find a command. */
	for (i = 0; i < len; i++) {
		if (!strncmp(argv[0], cmds->name,
					strlen(argv[0]) + 1)) {
			break;
		}
		cmds++;
	}
	if (i == len) {
		printf("Wrong argument = %s, type = %d.\n", argv[0], type);
		return 1;
	}

	/* Run a special parser. */
	cmds->func(argc - 1, &argv[1]);

	return 0;
}

/* 
 * Parse the plan command. Return 0 in case of success, otherwise
 * return 1.
 */
static int parse_plan(int argc, char *argv[])
{
	int ret;

	/* 
	 * Check that the command isn't an add command, because
	 * in this case the database isn't created. If it is not
	 * an add command, then the database can be opened.
	 */
	if (!(argc && !strncmp(argv[0], "add", strlen("add")))) {
		if (plandb_open()) {
			return 1;
		}
	}
	ret = parse_cmd(argc, argv, PARSE_PLAN);
	plandb_close();

	return ret;
}

/* 
 * Parse the project command. Return 0 in case of success, otherwise
 * return 1.
 */
static int parse_project(int argc, char *argv[])
{
	int ret;

	if (plandb_open()) {
		return 1;
	}
	ret = parse_cmd(argc, argv, PARSE_PROJECT);
	plandb_close();

	return ret;
}

/* 
 * Parse the task command. Return 0 in case of success, otherwise
 * return 1.
 */
static int parse_task(int argc, char *argv[])
{
	int ret;

	if (plandb_open()) {
		return 1;
	}
	ret = parse_cmd(argc, argv, PARSE_TASK);
	plandb_close();

	return ret;
}

int
main(int argc, char *argv[])
{
	if (config_init(CONFIG_NOPRINT)) {
		/* 
		 * Problem with the configuration file. Can't do anything
		 * in this case. Exit from application.
		 */
		return 1;
	}

	if (parse_cmd(argc - 1, &argv[1], PARSE_OBJ)) {
		usage(argv[0]);
		/* 
		 * Get some problems with the command line. Nothing to
		 * do in this case.
		 */
		return 1;
	}
	/* Work with database. */

	return 0;
}
