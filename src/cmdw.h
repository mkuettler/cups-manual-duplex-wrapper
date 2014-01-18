/* cmdw.h - CUPS Manual Duplex Wrapper header
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

#ifndef CUPS_MANUAL_DUPLEX_WRAPPER_HEADER
#define CUPS_MANUAL_DUPLEX_WRAPPER_HEADER

#ifdef CONFIG
# define EXTERN extern
#else
# define EXTERN
#endif

#define MSG 0
#define WRN 1
#define ERR 2
#define DBG 3

EXTERN void write_log(int mode, char const *msg, ...);

#undef EXTERN

#endif // CUPS_MANUAL_DUPLEX_WRAPPER_HEADER
