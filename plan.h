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

#if !defined(PLANT_PLAN_H)
#define PLANT_PLAN_H

int plan_add(int argc, char *argv[]);
int plan_current(int argc, char *argv[]);
int plan_switch(int argc, char *argv[]);
int plan_show(int argc, char *argv[]);
int plan_edit(int argc, char *argv[]);

#endif /* PLANT_PLAN_H */
