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
#include "task.h"
#include "project.h"
#include "timestamp.h"

/* Character representation for the task status. */
static char task_status_char[TASK_STATUS_NUM] = {'A', 'P', 'C'};

/* Change task status to new value. */
static void task_change_status(struct task_s *task, int status)
{
	if (task->status == status) {
		/* Do nothing. */
		return;
	}

	/* Update the parent project, if the status was completed. */
	if (task->status == TASK_STATUS_COMPLETED) {
		if (task->progress == 100) {
			task->progress = 99;
		}
		project_parent_complete_dec(task);
	}
	task->status = status;
	/* Update the parent project, if the status become completed. */
	if (task->status == TASK_STATUS_COMPLETED) {
		task->progress = 100;
		project_parent_complete_inc(task);
	}
}

/*
 * Check if the task should be shown or skipped. Return 1 if it should be
 * skipped, return 0 otherwise.
 */
static int task_show_skip(struct task_s *task, struct show_format_s *fmt)
{
	if (!fmt) {
		return 0;
	}

	if (fmt->mask_type) {
		if (!(fmt->mask_type & (1 << ITEM_TASK))) {
			return 1;
		}
	}
	if (fmt->mask_status) {
		if (!(fmt->mask_status & (1 << task->status))) {
			return 1;
		}
	}

	return 0;
}

/* 
 * Add task to the current plan. Return 0 in case of success,
 * otherwise return 1.
 */
int task_add(int argc, char *argv[])
{
	struct task_s *task;
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

	/* Fill the task structure. */
	task = plandb_item_add(parent_id, ITEM_TASK, sizeof(*task));
	if (!task) {
		return 1;
	}

	memset(task, 0, sizeof(*task));
	strncpy(task->name, argv[0], MAXNAME_LENGTH);
	task->name[MAXNAME_LENGTH - 1] = 0;
	task->status = TASK_STATUS_SUSPEND;

	return 0;
}

/* 
 * Remove task from the current plan. Return 0 in case of success,
 * otherwise return 1.
 */
int task_remove(int argc, char *argv[])
{
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	id = atoi(argv[0]);
	plandb_item_remove(id, ITEM_TASK);

	return 0;
}

/* 
 * Display all the active tasks. Return 0 in case of success,
 * otherwise return 1.
 */
int task_current(int argc, char *argv[])
{
	struct show_format_s fmt;

	/* Show only active tasks. */
	fmt.mask_type = (1 << ITEM_TASK);
	fmt.mask_status = (1 << TASK_STATUS_ACTIVE);

	/* Show all the plan with the required format. */
	plandb_item_show(0, ITEM_PROJECT, &fmt);

	return 1;
}

/* 
 * Show required task and its options. Return 0 in case of success,
 * otherwise return 1.
 */
int task_show(int argc, char *argv[])
{
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	id = atoi(argv[0]);
	plandb_item_show(id, ITEM_TASK, NULL);

	return 0;
}

/* 
 * Change task status to active. Return 0 in case of success, otherwise
 * return 1.
 */
int task_active(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no item with the requested id.\n");
		return 1;
	}

	task_change_status(task, TASK_STATUS_ACTIVE);

	return 0;
}

/* 
 * Change task status to pending. Return 0 in case of success, otherwise
 * return 1.
 */
int task_suspend(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no item with the requested id.\n");
		return 1;
	}

	task_change_status(task, TASK_STATUS_SUSPEND);

	return 0;
}

/* 
 * Change task status to completed. Return 0 in case of success, otherwise
 * return 1.
 */
int task_complete(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no item with the requested id.\n");
		return 1;
	}

	task_change_status(task, TASK_STATUS_COMPLETED);

	return 0;
}

/* 
 * Start the required task. Return 0 in case of success, otherwise
 * return 1.
 */
int task_start(int argc, char *argv[])
{
	struct task_s *task;
	struct timestamp_s *ts;
	int id;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	/* Check that the current task isn't already running. */
	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}
	/* Check the latest timestamp. */
	if (task->ts_num) {
		ts = (void *)(task + 1) + (sizeof(*ts) * (task->ts_num - 1));
		if (timestamp_running(ts)) {
			printf("Can't start the task, because it is already running.\n");
			return 1;
		}
	}

	/* Add new timestamp element to the current task. */
	task = plandb_task_add_data(id, sizeof(*ts));
	if (!task) {
		return 1;
	}

	/* Update the parent task, if the status was completed. */
	task_change_status(task, TASK_STATUS_ACTIVE);
	task->ts_num++;

	/* Get address for the current active timestamp. */
	ts = (void *)(task + 1) + (sizeof(*ts) * (task->ts_num - 1));
	timestamp_start(ts);

	return 0;
}

/*
 * Stop the required task. Return 0 in case of success, otherwise
 * return 1.
 */
int task_stop(int argc, char *argv[])
{
	struct task_s *task;
	unsigned int diff;
	int id;
	struct timestamp_s *ts;

	if (!argc) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}

	/*
	 * Get required task and update the timestamp according to its
	 * value. Also the status field of the task should be updated.
	 */
	id = atoi(argv[0]);
	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to stop.\n");
		return 1;
	}

	/* Get address for the current active timestamp. */
	if (!task->ts_num) {
		printf("Can't stop the task, because it wasn't started.\n");
		return 1;
	}
	ts = (void *)(task + 1) + (sizeof(*ts) * (task->ts_num - 1));
	diff = timestamp_stop(ts);
	task->sec_spent += diff;
	/* Update the parent task, if the status was completed. */
	task_change_status(task, TASK_STATUS_SUSPEND);

	return 0;
}

/* 
 * Assign resource to the task. Return 0 in case of success, otherwise
 * return 1.
 */
int task_assign(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}

	strncpy(task->resource, argv[1], MAXRESOURCE_NAME_LENGTH);
	task->resource[MAXRESOURCE_NAME_LENGTH - 1] = 0;

	return 0;
}

/* 
 * Track extra time for the requested task. Return 0 in case success,
 * otherwise return 1.
 */
int task_track(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}

	task->hours_extra += atoi(argv[1]);
	if (task->hours_extra < 0) {
		task->hours_extra = 0;
	}

	return 0;
}

/* 
 * Set estimation for the requested task. Return 0 in case of succes,
 * otherwise return 1.
 */
int task_estimate(int argc, char *argv[])
{
	struct task_s *task;
	int id;
	int estimate;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}

	estimate = atoi(argv[1]);
	if (estimate < 0) {
		printf("Estimation should be more than 0.\n");
		return 1;
	}
	task->hours_estimated = estimate;

	return 0;
}

/* 
 * Set progress for the requested task. Return 0 in case of succes,
 * otherwise return 1.
 */
int task_progress(int argc, char *argv[])
{
	struct task_s *task;
	int id;
	int progress;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}

	progress = atoi(argv[1]);
	if ((progress < 0) || (progress > 100)) {
		printf("Wrong progress number, can be from 0 to 100.\n");
		return 1;
	}

	task->progress = progress;
	if (task->progress == 100) {
		task_change_status(task, TASK_STATUS_COMPLETED);
	} else {
		if (task->status == TASK_STATUS_COMPLETED) {
			task_change_status(task, TASK_STATUS_SUSPEND);
		}
	}

	return 0;
}

/* 
 * Rename the requested task. Return 0 in case of success, otherwise
 * return 1.
 */
int task_rename(int argc, char *argv[])
{
	struct task_s *task;
	int id;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id.\n");
		return 1;
	}

	strncpy(task->name, argv[1], MAXNAME_LENGTH);
	task->name[MAXNAME_LENGTH - 1] = 0;

	return 0;
}

/*
 * Remove the requested timestamp in the task structure. Return 0 in case
 * of success, otherwise return 1.
 */
int task_removetime(int argc, char *argv[])
{
	struct task_s *task;
	int id;
	int ts_id;
	struct timestamp_s *ts;

	if (argc < 2) {
		printf("Not enough arguments, for the current command.\n");
		return 1;
	}
	id = atoi(argv[0]);

	task = plandb_task_by_id(id);
	if (!task) {
		printf("There is no task with such id to start.\n");
		return 1;
	}

	ts_id = atoi(argv[1]);
	ts_id--;
	if ((ts_id < 0) || (ts_id >= task->ts_num)) {
		printf("There is no timestamp with such id for the required task.\n");
		return 1;
	}
	task->ts_num--;
	ts = (void *)(task + 1) + (sizeof(*ts) * ts_id);
	plandb_task_remove_data(task, ts, sizeof(*ts));

	return 0;
}

/* Show task item on screen. */
void task_show_item(struct task_s *task, int ident, int id,
		struct show_format_s *fmt)
{
	unsigned int hours;
	unsigned int mins;
	unsigned int secs;
	char buf[OUTPUT_NAME_LENGTH];
	int i;
	struct timestamp_s *ts;

	if (task_show_skip(task, fmt)) {
		/* Nothing to do in this case. */
		return;
	}

	printf(" %03d | ", id);
	snprintf(buf, OUTPUT_NAME_LENGTH, "%*s%s", ident, "",
			task->name);
	secs = task->sec_spent;
	hours = secs / 3600;
	hours += task->hours_extra;
	secs %= 3600;
	mins = secs / 60;
	secs %= 60;

	printf("%-40s | %c | %03d%% | %4d:%02d:%02d | %5dh |",
			buf, task_status_char[task->status], task->progress,
			hours, mins, secs,
			task->hours_estimated);
	if (strlen(task->resource)) {
		printf(" %s\n", task->resource);
	} else {
		printf("\n");
	}
	if (task->hours_extra) {
		printf("     | ");
		snprintf(buf, OUTPUT_NAME_LENGTH,
				"       extra hours (not tracked): %4dh",
				task->hours_extra);
		printf("%-40s |   |      |            |        |\n", buf);
	}
	ts = (struct timestamp_s *)(task + 1);
	for (i = 0; i < task->ts_num; i++) {
		timestamp_show(ts, i + 1);
		ts++;
	}
}

/* Return 1 if task is completed, otherwise return 0. */
int task_is_completed(struct task_s *task)
{
	return (task->status == TASK_STATUS_COMPLETED);
}
