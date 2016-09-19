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

#if !defined(PLANT_CONFIG_H)
#define PLANT_CONFIG_H

#define CONFIG_DIR ".plantrc"
#define CONFIG_NAME "config"
#define CONFIG_PATH_LENGTH 128
#define CONFIG_MAXSTR_LENGTH 128
#define CONFIG_PRINT 1
#define CONFIG_NOPRINT 0

int config_init(int show);
int config_set_plandb(char *name);
char *config_get_plandb();
char *config_get_planpath();

#endif /* PLANT_CONFIG_H */
