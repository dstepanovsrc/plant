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
#include "plant_limits.h"
#include "timestamp.h"
#include "database.h"

/* Initialize the start timestamp for the required structure. */
void timestamp_start(struct timestamp_s *ts)
{
	memset(ts, 0, sizeof(*ts));
	ts->start = time(NULL);
}

/* Stop the time and return the difference between start and stop time. */
unsigned int timestamp_stop(struct timestamp_s *ts)
{
	if (ts->stop) {
		printf("Can't stop the task, because it wasn't started.\n");
		return 0;
	}
		
	ts->stop = time(NULL);

	return ts->stop - ts->start;
}

/* Show task item on screen. */
void timestamp_show(struct timestamp_s *ts, int num)
{
	char strt[MAXTIME_STRING];
	struct tm *tm;
	char buf[OUTPUT_NAME_LENGTH];

	tm = localtime(&ts->start);
	strftime(strt, MAXTIME_STRING, "%F %H:%M:%S", tm);
	snprintf(buf, OUTPUT_NAME_LENGTH, "[%02d] start time : %s",
			num, strt);
	printf("     |   %s  |   |      |            |        |\n", buf);
	if (ts->stop) {
		tm = localtime(&ts->stop);
		strftime(strt, MAXTIME_STRING, "%F %H:%M:%S", tm);
		snprintf(buf, OUTPUT_NAME_LENGTH, "end time   : %s", strt);
		printf("     |        %s  |   |      |            |        |\n", buf);
	} else {
		snprintf(buf, OUTPUT_NAME_LENGTH,
				"end time   : in progress");
		printf("     |        %s          |   |      |            |        |\n", buf);
	}
}

/* 
 * Check the current state of the task timer. Return 1 if it is running,
 * otherwise return 0.
 */
int timestamp_running(struct timestamp_s *ts)
{
	if (ts->start && (!ts->stop)) {
		return 1;
	}

	return 0;
}
