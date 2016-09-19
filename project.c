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
#include <stdlib.h>
#include "database.h"
#include "project.h"

/*
 * Check if the project should be shown or skipped. Return 1 if it should be
 * skipped, return 0 otherwise.
 */
static int project_show_skip(struct project_s *project,
		struct show_format_s *fmt)
{
	if (!fmt) {
		return 0;
	}

	if (fmt->mask_type) {
		if (!(fmt->mask_type & (1 << ITEM_PROJECT))) {
			return 1;
		}
	}

	return 0;
}

/* 
 * Add project to the current plan. Return 0 in case of success,
 * otherwise return 1. The arguments are as follows:
 *  argv[0] - new project name
 *  argv[1] - id of the parent project
 */
int project_add(int argc, char *argv[])
{
	struct project_s *project;
	int parent_id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	/* Get the parent project id. */
	parent_id = 0;
	if (argc > 1) {
		parent_id = atoi(argv[1]);
	}

	/* Fill the project structure. */
	project = plandb_item_add(parent_id, ITEM_PROJECT,
			sizeof(*project));
	if (!project) {
		return 1;
	}

	memset(project, 0, sizeof(*project));
	strncpy(project->name, argv[0], MAXNAME_LENGTH);
	project->name[MAXNAME_LENGTH - 1] = 0;

	return 0;
}

/* 
 * Remove project from the current plan. Return 0 in case of success,
 * otherwise return 1.
 */
int project_remove(int argc, char *argv[])
{
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	id = atoi(argv[0]);
	if (id) {
		plandb_item_remove(id, ITEM_PROJECT);
	} else {
		/* In case of id == 0, just clean all the plan. */
		plandb_plan_clean();
		printf("Successfull removal of all projects and tasks in the current plan.\n");
	}

	return 0;
}

/* 
 * Show required project and its tasks. Return 0 in case of success,
 * otherwise return 1.
 */
int project_show(int argc, char *argv[])
{
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	id = atoi(argv[0]);
	plandb_item_show(id, ITEM_PROJECT, NULL);

	return 0;
}

/* 
 * Edit required project in the current plan. Return 0 in case of success,
 * otherwise return 1.
 */
int project_rename(int argc, char *argv[])
{
	struct project_s *project;
	int id;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	project = plandb_project_by_id(id);
	if (!project) {
		printf("There is no project with such id.\n");
		return 1;
	}

	strncpy(project->name, argv[1], MAXNAME_LENGTH);
	project->name[MAXNAME_LENGTH - 1] = 0;

	return 0;
}

/* Show project item on screen. */
void project_show_item(struct project_s *project, int ident, int id,
		struct show_format_s *fmt)
{
	char ch;
	char buf[OUTPUT_NAME_LENGTH];

	if (project_show_skip(project, fmt)) {
		/* Nothing to do in this case. */
		return;
	}

	printf(" %03d | ", id);
	if (project->tasks_completed == plandb_get_child_num(project)) {
		ch = PROJECT_CHAR_COMPLETED;
	} else {
		ch = PROJECT_CHAR_INPROGRESS;
	}
	snprintf(buf, OUTPUT_NAME_LENGTH, "%*s%s (%d/%d)", ident, "",
			project->name, project->tasks_completed,
			plandb_get_child_num(project));
	printf("%-40s | %c |      |            |        |\n", buf, ch);
}

/* 
 * Update the parent project for the current data, by reducing the number
 * of completed items.
 */
void project_parent_complete_dec(void *data)
{
	struct project_s *project;
	int child_num;

	project = plandb_parent_by_data(data);
	if (!project) {
		return;
	}

	child_num = plandb_get_child_num(project);
	if (project->tasks_completed > 0) {
		if (project->tasks_completed == child_num) {
			project_parent_complete_dec(project);
		}
		project->tasks_completed--;
	}
}

/* 
 * Update the parent project for the current data, by increasing the number
 * of completed items.
 */
void project_parent_complete_inc(void *data)
{
	struct project_s *project;
	int child_num;

	project = plandb_parent_by_data(data);
	if (!project) {
		return;
	}

	child_num = plandb_get_child_num(project);
	if (project->tasks_completed < child_num) {
		project->tasks_completed++;
		if (project->tasks_completed == child_num) {
			project_parent_complete_inc(project);
		}
	}
}

/* 
 * Update the projects tree after the add command. This is required
 * for the completed state.
 */
void projects_update_after_add(struct project_s *project, int type)
{
	int child_num;

	child_num = plandb_get_child_num(project);
	if (type == ITEM_PROJECT) { 
		project->tasks_completed++;
	} else {
		/* 
		 * In case of the task item, just check if because
		 * of new item the project is become uncomplete.
		 */
		if ((project->tasks_completed + 1) == child_num) {
			project_parent_complete_dec(project);
		}
	}
}

/* 
 * Update the projects tree after the remove command. This is required
 * for the completed state.
 */
void projects_update_after_remove(struct project_s *project, int is_completed)
{
	int child_num;

	child_num = plandb_get_child_num(project);
	if (is_completed) {
		project->tasks_completed--;
	}

	if ((project->tasks_completed + 1) == child_num) {
		if (is_completed) {
			/* 
			 * Project become incomplete, because completed
			 * item is removed.
			 */
			project_parent_complete_dec(project);
		}
	} else if (project->tasks_completed == child_num) {
		if (!is_completed) {
			/* 
			 * Project become completed, because an
			 * incompleted item was removed.
			 */
			project_parent_complete_inc(project);
		}
	}
}
