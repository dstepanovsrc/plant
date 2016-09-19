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

#if !defined(PLANT_TIMESTAMP_H)
#define PLANT_TIMESTAMP_H

#include <time.h>

#define MAXTIME_STRING 32

struct timestamp_s {
	time_t start;
	time_t stop;
};

void timestamp_start(struct timestamp_s *ts);
unsigned int timestamp_stop(struct timestamp_s *ts);
void timestamp_show(struct timestamp_s *ts, int num);
int timestamp_running(struct timestamp_s *ts);

#endif /* PLANT_TIMESTAMP_H */
