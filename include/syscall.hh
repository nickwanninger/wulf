/*
 * A compiler for the wulf language
 * Copyright (C) 2018  Nick Wanninger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define SYS_EXIT   1

#define SYS_EVAL   2
#define SYS_MACROEXPAND 3

#define SYS_PRINT  7
#define SYS_LOAD   8
#define SYS_TYPE   9
#define SYS_SHELL  10

#define SYS_AND    16
#define SYS_OR     17
#define SYS_NOT    18

#define SYS_CAR    19
#define SYS_CDR    20

#define SYS_RNG    22

#define SYS_GC_COLLECT 23

#define SYS_CONS 24


#define SYS_EQUAL 25
#define SYS_LT    26

#define SYS_STRLEN 27
#define SYS_STRREF 28
#define SYS_STRSET 29
#define SYS_STRCAT 30
#define SYS_STRCONV 31
