/* cmdw.h - CUPS Manual Duplex Wrapper header
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

#ifndef CUPS_MANUAL_DUPLEX_WRAPPER_HEADER
#define CUPS_MANUAL_DUPLEX_WRAPPER_HEADER

#define PRINT_CMD "lp"

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
