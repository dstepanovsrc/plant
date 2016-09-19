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
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include "plant_limits.h"
#include "config.h"
#include "database.h"
#include "project.h"
#include "task.h"
#include "timestamp.h"

static struct plan_desc_s *db_desc;

/*
 * Truncate file and add memory to the mmap buffer. Return 0 in case of
 * success, otherwise return 1.
 */
static int db_truncate_remap(int new_size)
{
	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return 1;
	}

	if (db_desc->size) {
		if (munmap(db_desc->mem, db_desc->size) == -1) {
			printf("Can't munmap() the previous buffer.\n");
			return 1;
		}
	}
	db_desc->size = new_size;
	if (ftruncate(db_desc->fd, db_desc->size)) {
		printf("Can't truncate the database file to new %ld size.\n",
				db_desc->size);
		return 1;
	}
	db_desc->mem = mmap(0, db_desc->size, PROT_READ | PROT_WRITE,
			MAP_SHARED, db_desc->fd, 0);
	if (!db_desc->mem) {
		printf("Can't mmap database to memory.\n");
		return 1;
	}

	return 0;
}

/* Initialize the plan header with the default values. */
static void header_init()
{
	struct plan_header_s *header;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return;
	}

	header = db_desc->mem;
	memset(header, 0, sizeof(*header));
	header->version = PLANT_VERSION;
}

/* Initialize the root item of the project/task array for the current plan. */
static void root_item_init()
{
	struct plan_item_s *item;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return;
	}

	item = db_desc->mem + sizeof(struct plan_header_s);
	memset(item, 0, sizeof(*item));
	item->type = ITEM_PROJECT;
}

/*
 * Return the pointer to the item with the required id. If such element is
 * not found, then return NULL.
 */
static struct plan_item_s *find_item_by_id(int id)
{
	struct plan_item_s *item;
	void *mem_end;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return NULL;
	}

	mem_end = db_desc->mem + db_desc->size;
	item = db_desc->mem + sizeof(struct plan_header_s);
	while ((unsigned long long)item < (unsigned long long)mem_end) {
		if (!id) {
			/* Item found. */
			return item;
		}
		id--;
		item = (void *)item + sizeof(*item) + item->size;
	}

	/* Item not found. */
	return NULL;
}

/*
 * Return the pointer to the parent for the requested item. If such element is
 * not found, then return NULL.
 */
static struct plan_item_s *find_parent_by_item(struct plan_item_s *item)
{
	int num;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return NULL;
	}

	num = 0;
	do {
		num++;
		item = (void *)item - sizeof(*item) - item->prev_size;
		if ((unsigned long long)item <
				((unsigned long long)db_desc->mem +
				 sizeof(struct plan_header_s))) {
			return NULL;
		}
		num -= item->num;

	} while (num > 0);

	return item;
}

/*
 * Find the tail item in the child list of the requested item. Return NULL
 * in case of error.
 */
static struct plan_item_s *find_parent_tail(struct plan_item_s *parent)
{
	int i;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return NULL;
	}

	for (i = (parent->num - 1); i > 0; i--) {
		parent = (void *)parent + sizeof(*parent) + parent->size;
		if ((unsigned long long)parent >
				(unsigned long long)(db_desc->mem +
					db_desc->size)) {
			printf("The database is broken. Can't add element.\n");
			return NULL;
		}
		i += parent->num;
	}

	return parent;
}

/* Show item of specified type. */
static void show_item(void *data, int type, int ident, int id,
		struct show_format_s *fmt)
{
	if (!id) {
		/* It is the root item. Don't show it. */
		return;
	}

	switch (type) {
	case ITEM_PROJECT:
		project_show_item(data, ident, id, fmt);
		break;
	case ITEM_TASK:
		task_show_item(data, ident, id, fmt);
		break;
	default:
		printf("Wrong item type.\n");
		break;
	}
}

/* 
 * Return the address to the project structure, for the requested id. In case
 * of error return NULL.
 */
void *plandb_project_by_id(int id)
{
	struct plan_item_s *item;
	
	if (id <= 0) {
		return NULL;
	}
	item = find_item_by_id(id);
	if (!item) {
		return NULL;
	}
	if (item->type != ITEM_PROJECT) {
		return NULL;
	}

	return (item + 1);
}

/* 
 * Return the address to the task structure, for the requested id. In case
 * of error return NULL.
 */
void *plandb_task_by_id(int id)
{
	struct plan_item_s *item;
	
	item = find_item_by_id(id);
	if (!item) {
		return NULL;
	}
	if (item->type != ITEM_TASK) {
		return NULL;
	}

	return (item + 1);
}

/*
 * Return the address of the parent element of the required data item. In
 * case of error return NULL.
 */
void *plandb_parent_by_data(void *data)
{
	struct plan_item_s *item;
	struct plan_item_s *parent;

	item = data - sizeof(*item);
	parent = find_parent_by_item(item);
	if (!parent) {
		return NULL;
	}

	return (parent + 1);
}

/* 
 * Add data to the requested task. Return pointer to the task structure.
 * In case of error return NULL.
 */
void *plandb_task_add_data(int id, int size)
{
	void *data;
	struct plan_item_s *item;
	unsigned long long offset;

	item = find_item_by_id(id);
	if (item->type != ITEM_TASK) {
		return NULL;
	}

	/*
	 * Store the parent offset which will be used after munmap/mmap
	 * operation. This is important because the address can differ
	 * after remap call.
	 */
	offset = (unsigned long long)item - (unsigned long long)db_desc->mem;
	if (db_truncate_remap(db_desc->size + size)) {
		printf("Can't add new item to the current plan.\n");
		return NULL;
	}
	/* Restore item. */
	item = db_desc->mem + offset;

	/* Move data. */
	data = (void *)item + sizeof(*item) + item->size;
	memmove(data + size, data,
			((unsigned long long)db_desc->mem + db_desc->size -
			 (unsigned long long)data - size));

	/* Don't forget to update the size field of the item. */
	item->size += size;

	return (item + 1);
}

/* 
 * Remove data from the requested task.
 */
void plandb_task_remove_data(void *task, void *data, int size)
{
	struct plan_item_s *item;

	item = task - sizeof(*item);
	if (item->type != ITEM_TASK) {
		return;
	}

	/* Move data. */
	memmove(data, data + size,
			((unsigned long long)db_desc->mem + db_desc->size -
			 (unsigned long long)data + size));
	/* Don't forget to update the size field of the item. */
	item->size -= size;

	if (db_truncate_remap(db_desc->size - size)) {
		printf("Can't remove data from the task structure.\n");
		return;
	}
}

/*
 * Initialize the database descriptor. Return 0 in case of success,
 * otherwise return 1.
 */
int plandb_open()
{
	char path[MAXNAME_LENGTH * 2];

	if (db_desc) {
		return 1;
	}

	db_desc = malloc(sizeof(*db_desc));
	if (!db_desc) {
		printf("Not enough memory to work with database.\n");
		return 1;
	}

	snprintf(path, MAXNAME_LENGTH * 2, "%s/%s", config_get_planpath(),
			config_get_plandb());
	db_desc->fd = open(path, O_RDWR);
	if (db_desc->fd == -1) {
		printf("Can't open the %s database file.\n", path);
		goto cleanup_plandb_open_mem;
	}
	db_desc->size = lseek(db_desc->fd, 0, SEEK_END);
	if (db_desc->size == (off_t)-1) {
		printf("Can't get the size of the database.\n");
		goto cleanup_plandb_open_fd;
	}
	if (db_desc->size) {
		db_desc->mem = mmap(0, db_desc->size, PROT_READ | PROT_WRITE,
				MAP_SHARED, db_desc->fd, 0);
		if (!db_desc->mem) {
			printf("Can't mmap database to memory.\n");
			goto cleanup_plandb_open_fd;
		}
	}

	return 0;

cleanup_plandb_open_fd:
	close(db_desc->fd);
cleanup_plandb_open_mem:
	free(db_desc);

	return 1;
}

/* Close the plan database. */
void plandb_close()
{
	if (!db_desc) {
		return;
	}
	munmap(db_desc->mem, db_desc->size);
	close(db_desc->fd);
	free(db_desc);
}


/* 
 * Initialize the item structure. Return address to new element in case of
 * success, otherwise return NULL.
 */
void *plandb_item_add(int pid, int type, int size)
{
	struct plan_item_s *parent;
	struct plan_item_s *item;
	unsigned long long offset;
	int ident;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return NULL;
	}

	/* Find the parent element for the required pid. */
	parent = find_item_by_id(pid);
	if (!parent) {
		printf("There is no item with the requested id.\n");
		return NULL;
	}

	/*
	 * Check that the parent type is correct, according to new item
	 * type.
	 */
	if ((type == ITEM_PROJECT) || (type == ITEM_TASK)) {
		/* 
		 * For new project or task item the only possible
		 * parent can be another project item.
		 */
		if (parent->type != ITEM_PROJECT) {
			/* No item is found. */
			parent = NULL;
		}
	} else {
		/* Some item found, but the type is wrong. */
		parent = NULL;
	}
	if (!parent) {
		printf("There is no item with the requested id.\n");
		return NULL;
	}

	/*
	 * Store the parent offset which will be used after munmap/mmap
	 * operation. This is important because the address can differ
	 * after remap call.
	 */
	offset = (unsigned long long)parent - (unsigned long long)db_desc->mem;
	if (db_truncate_remap(db_desc->size + sizeof(*item) + size)) {
		printf("Can't add new item to the current plan.\n");
		return NULL;
	}

	/* Update number of child elements for the parent. */
	parent = db_desc->mem + offset;
	parent->num++;
	projects_update_after_add((void *)(parent + 1), type);
	ident = parent->ident + 1;

	/* Insert new element at the end of the parent child list. */
	parent = find_parent_tail(parent);
	if (!parent) {
		return NULL;
	}

	/* 
	 * The previous item for our new item is found. Copy memory to
	 * insert new element into our plan array.
	 */
	item = (void *)parent + sizeof(*parent) + parent->size;
	item->prev_size = size;
	memmove((void *)item + sizeof(*item) + size, item,
			((unsigned long long)db_desc->mem + db_desc->size -
			 (unsigned long long)item - sizeof(*item) - size));

	/* 
	 * Current parent pointer contains just a previous element,
	 * in general it can be not a parent element.
	 */
	memset(item, 0, sizeof(*item));
	item->type = type;
	item->size = size;
	item->prev_size = parent->size;
	item->num = 0;
	item->ident = ident;

	return (char *)(item) + sizeof(*item);
}


/* Remove all the project and tasks in the current plan. */
void plandb_plan_clean()
{
	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return;
	}

	if (db_truncate_remap(sizeof(struct plan_header_s) +
				sizeof(struct plan_item_s))) {
		printf("Can't clean up the current plan.\n");
		return;
	}

	header_init(db_desc);
	root_item_init(db_desc);
}

/* Remove the item with the required id. Remove all of its subtasks if any. */
void plandb_item_remove(int id, int type)
{
	struct plan_item_s *item;
	struct plan_item_s *tmp;
	struct plan_item_s *parent;
	int i;
	int rm_size;
	int prev_size;
	int is_completed;

	if (!db_desc) {
		printf("There is no database intitialized.\n");
		return;
	}

	item = find_item_by_id(id);
	if (!item) {
		printf("There is no such id to remove.\n");
		return;
	}
	if (item->type != type) {
		printf("There is no such id to remove.\n");
		return;
	}
	prev_size = item->prev_size;
	/* Update parent record. */
	parent = find_parent_by_item(item);
	parent->num--;

	/* 
	 * Don't forget to store the current item pointer, because
	 * it will be used to copy memory.
	 */
	tmp = item;
	/* 
	 * At least one element should be removed. Find all its
	 * subprojects and subtasks.
	 */
	i = 1;
	rm_size = 0;
	is_completed = 1;
	do {
		i += item->num;
		if (is_completed && (item->type == ITEM_TASK)) {
			if (!task_is_completed((void *)(item + 1))) {
				is_completed = 0;
			}
		}
		rm_size += sizeof(*item) + item->size;
		item = (void *)item + sizeof(*item) + item->size;
		i--;
	} while (i > 0);
	projects_update_after_remove((void *)(parent + 1), is_completed);
	memmove(tmp, item, (unsigned long long)db_desc->mem + db_desc->size -
			(unsigned long long)item);
	tmp->prev_size = prev_size;

	if (db_truncate_remap(db_desc->size - rm_size)) {
		printf("Can't clean up the current plan.\n");
		return;
	}
}

/* Find and show items for the required name. */
void plandb_item_show(int id, int type, struct show_format_s *fmt)
{
	struct plan_item_s *item;
	int i;
	int base_ident;

	printf("  #  | Name%*s | S | Prog |   Spent    |  Estm  |  Assign\n",
			OUTPUT_NAME_LENGTH - (int)strlen("Name"), "");
	printf("---------------------------------------------------------------------------------------------\n");
	item = find_item_by_id(id);
	if (!item) {
		printf("There is no such id to show.\n");
		return;
	}
	if (item->type != type) {
		printf("There is no such id to show.\n");
		return;
	}
	base_ident = item->ident;
	if (id == 0) {
		/* 
		 * Because root item isn't shown the ident should be set to
		 * next value.
		 */
		base_ident++;
	}
	for (i = item->num + 1; i > 0; i--) {
		show_item((void *)item + sizeof(*item), item->type,
				item->ident - base_ident, id, fmt);
		item = (void *)item + sizeof(*item) + item->size;
		i += item->num;
		id++;
	}
}

/* Return number of child for the current item. */
int plandb_get_child_num(void *data)
{
	struct plan_item_s *item;

	item = data - sizeof(*item);

	return item->num;
}

/*
 * Add header to the plan database. Return 0 in case of success, otherwise
 * return 1.
 */
int plandb_header_add()
{
	if (plandb_open()) {
		return 1;
	}

	plandb_plan_clean();

	return 0;
}

#if 0
/* TODO */
static void plandb_trace_plan(struct plan_desc_s *desc)
{
	struct plan_item_s *item;
	struct plan_header_s *hdr;
	int size;
	int num;
	char *str_type[] = {"PROJECT", "TASK", "TIMESTAMP"};

	printf("\n===================================================\n");
	printf("size of header: %d\n", (int)sizeof(struct plan_header_s));
	hdr = desc->mem;
	printf("hdr version = %d\n", hdr->version);
	printf("hdr res1:res2:res3 = %d:%d:%d\n", hdr->reserved1,
			hdr->reserved2, hdr->reserved3);

	printf("plan list item size = %d\n", (int)sizeof(*item));
	printf("project size = %d\n", (int)sizeof(struct project_s));
	printf("task size = %d\n", (int)sizeof(struct task_s));
	printf("size overall = %d\n", (int)desc->size);
	size = sizeof(struct plan_header_s);
	num = 0;
	while (size < desc->size) {
		item = desc->mem + size;
		printf("[%d] t/s/prev/num/ident: %s:%d:%d:%d:%d\n",
				num, str_type[item->type], item->size,
				item->prev_size, item->num, item->ident);
		num++;
		size += sizeof(*item);
		size += item->size;
	}
}
#endif
