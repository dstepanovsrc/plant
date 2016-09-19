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

#if !defined(PLANT_DATABASE_H)
#define PLANT_DATABASE_H

enum {
	ITEM_PROJECT,
	ITEM_TASK,
};

struct plan_desc_s {
	int fd;
	void *mem;
	size_t size;
};

struct plan_header_s {
	int version;
	int reserved1;
	int reserved2;
	int reserved3;
};

struct plan_item_s {
	/* Item type. */
	int type;
	/* Size of the current element. */
	int size;
	/* 
	 * Size of the previous element. Field used to go through array
	 * in the reverse order.
	 */
	int prev_size;
	/* Number of the child elements. */
	int num;
	/* Relative ident from the root element. */
	int ident;
};

/* Structure to format the output for the show command. */
struct show_format_s {
	unsigned long long mask_type;
	unsigned long long mask_status;
};

void *plandb_project_by_id(int id);
void *plandb_task_by_id(int id);
void *plandb_parent_by_data(void *data);
void *plandb_task_add_data(int id, int size);
void plandb_task_remove_data(void *task, void *data, int size);
int plandb_open();
void plandb_close();
void *plandb_item_add(int pid, int type, int size);
void plandb_plan_clean();
void plandb_item_remove(int id, int type);
void plandb_item_show(int id, int type, struct show_format_s *fmt);
int plandb_get_child_num(void *data);
int plandb_header_add();
void *plandb_get_task(int id);
unsigned int plandb_stop_task(void *task);

#endif /* PLANT_DATABASE_H */
