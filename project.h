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

#if !defined(PLANT_PROJECT_H)
#define PLANT_PROJECT_H

#include "plant_limits.h"

enum {
	PROJECT_CHAR_INPROGRESS = 'P',
	PROJECT_CHAR_COMPLETED = 'C'
};

struct project_s {
	char name[MAXNAME_LENGTH];
	unsigned int tasks_completed;
};

int project_add(int argc, char *argv[]);
int project_remove(int argc, char *argv[]);
int project_show(int argc, char *argv[]);
int project_rename(int argc, char *argv[]);

void project_show_item(struct project_s *project, int ident, int id,
		struct show_format_s *fmt);
void project_parent_complete_dec(void *data);
void project_parent_complete_inc(void *data);
void projects_update_after_add(struct project_s *project, int type);
void projects_update_after_remove(struct project_s *project, int is_completed);

#endif /* PLANT_PROJECT_H */
