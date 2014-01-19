/* cmdw-options.h - CUPS Manual Duplex Wrapper, option handling header
   Copyright (C) 2014 Martin Kuettler

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.
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
