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

#if !defined(PLANT_TASK_H)
#define PLANT_TASK_H

#include "plant_limits.h"
#include "timestamp.h"

#define MAXRESOURCE_NAME_LENGTH 10

enum {
	TASK_STATUS_ACTIVE,
	TASK_STATUS_SUSPEND, TASK_STATUS_COMPLETED,
	TASK_STATUS_NUM
};

struct task_s {
	char name[MAXNAME_LENGTH];
	char resource[MAXRESOURCE_NAME_LENGTH];
	unsigned char progress;
	unsigned char status;
	unsigned int hours_estimated;
	unsigned int sec_spent;
	int hours_extra;
	unsigned int ts_num;
};

int task_add(int argc, char *argv[]);
int task_remove(int argc, char *argv[]);
int task_current(int argc, char *argv[]);
int task_show(int argc, char *argv[]);
int task_active(int argc, char *argv[]);
int task_suspend(int argc, char *argv[]);
int task_complete(int argc, char *argv[]);
int task_start(int argc, char *argv[]);
int task_stop(int argc, char *argv[]);
int task_assign(int argc, char *argv[]);
int task_track(int argc, char *argv[]);
int task_estimate(int argc, char *argv[]);
int task_progress(int argc, char *argv[]);
int task_rename(int argc, char *argv[]);
int task_removetime(int argc, char *argv[]);

void task_show_item(struct task_s *task, int ident, int id,
		struct show_format_s *fmt);
int task_is_completed(struct task_s *task);

#endif /* PLANT_TASK_H */
