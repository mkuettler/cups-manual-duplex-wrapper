/* cmdw-options.h - CUPS Manual Duplex Wrapper, option handling header
   Copyright (C) 2014 Martin Kuettler

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUPS_MANUAL_DUPLEX_WRAPPER_OPTIONS_H
#define CUPS_MANUAL_DUPLEX_WRAPPER_OPTIONS_H

#ifdef CMDW_OPTIONS
# define EXTERN
#else
# define EXTERN extern
#endif

EXTERN int duplex;
EXTERN int manual_copies;
EXTERN char *printer_name;

#undef EXTERN

int parse_and_assemble_options(char **oargv, char ***argv);

int prepare_odd_pages();
int prepare_even_pages();
int prepare_all_pages();

void cleanup_options();

#endif // CUPS_MANUAL_DUPLEX_WRAPPER_OPTIONS_H
