/* cmdw-options.c - CUPS Manual Duplex Wrapper, option handling code
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMDW_OPTIONS
#include "cmdw-options.h"
#include "cmdw.h"

#define MIN(a,b) (a < b ? a : b)

#define IF_OPT(o) if (strncasecmp(o, opts, p-opts) == 0)
#define ELIF_OPT(o) else IF_OPT(o)
#define ELIF_OPT_BEGIN(o)                                               \
    else if (strncasecmp(o, opts, MIN(strlen(o), (size_t)(p-opts))) == 0)

#define SKIP_TO_NEXT_WORD                       \
    for (; *p == ' '; ++p)                      \
        ;                                       \
    opts = p;

#define SKIP_TO_END_OF_WORD                     \
    for (; *p && *p != ' '; ++p)                \
        ;

#define CHECK_FOR_ARGUMENT                                      \
    if (*p == ' ') {                                            \
        write_log(WRN, "Option %.*s needs an argument. "        \
                  "Ignored.", p-opts, opts);                    \
        continue;                                               \
    }                                                           \
    opts = ++p;

#define CHECK_FOR_NO_ARGUMENT                                   \
    if (*p == '=') {                                            \
        write_log(WRN, "Invalid argument to option %.*s. "      \
                  "Ignored.", p-opts, opts);                    \
        SKIP_TO_END_OF_WORD;                                    \
    }
/* TODO: Quotes! */

static char *opt_buf;
static char **argv;
static char **outputorder;
static char **page_set;
static char **orientation_req;
static int orientation;
static int initialized = 0;

int parse_and_assemble_options(char **oargv, char ***argvp)
{
    int nopts = 1;
    int argc;
    char *p, *buf_p;
    char *opts = oargv[5];
    int i;
    int collate;

    write_log(DBG, "Beginning");
    /* count number of options in opts */
    for (p = opts; *p;) {
        i = 0;
        if (*p == ' ') ++nopts;
        while (*p == ' ') ++p;
        if (!*p) break;

        if (*p == '\'') {
            i = 1;
            for(; *p && *p != '\''; ++p)
                if (*p == '\\' && *(p+1)) ++p;
        } else if (*p == '"') {
            i = 1;
            for(; *p && *p != '"'; ++p)
                if (*p == '\\' && *(p+1)) ++p;
        } else {
            for(; *p && *p != ' '; ++p)
                ;
        }

        if (i && !*p) {
            write_log(ERR, "Failed parsing options: "
                      "Opening string doesn't close");
            return -1;
        }
    }

    write_log(DBG, "Counted %i opts", nopts);

    opt_buf = malloc((strlen(opts)+1) * sizeof(char));
    buf_p = opt_buf;
    argv = *argvp = calloc(2*nopts + 17, sizeof(char*));
    argc = 0;
    argv[argc++] = PRINT_CMD;
    argv[argc++] = "-t";
    argv[argc++] = oargv[3];
    argv[argc++] = "-o";
    argv[argc++] = "sides=one-sided";
    argv[argc++] = "-o";
    outputorder = &argv[argc++];
    argv[argc++] = "-o";
    page_set = &argv[argc++];
    argv[argc++] = "-o";

    argv[argc] = malloc(24 * sizeof(char));
    orientation_req = &argv[argc++];

    /* set default values */
    duplex = 0;
    collate = 0;
    orientation = 3;
    printer_name = NULL;

    write_log(DBG, "Filled first %i args", argc);

    /* parse and copy options */
    for(p = opts; *p; ) {
        SKIP_TO_NEXT_WORD;

        for (; *p && *p != '=' && *p != ' '; ++p)
            ;
        if (!*p) break;

        write_log(DBG, "Found option %.*s", p-opts, opts);

        IF_OPT("sides") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;

            if (strncmp("two-sided-long-edge", opts, p-opts) == 0) {
                duplex = 1;
            } else if (strncmp("two-sided-short-edge", opts, p-opts) == 0) {
                duplex = 2;
            } else if (strncmp("one-sided", opts, p-opts) == 0) {
                duplex = 0;
            } else {
                write_log(WRN, "Unknown option sides=%.*s. Ignored.",
                          p-opts, opts);
            }
            continue;
        } ELIF_OPT("outputorder") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;

            write_log(MSG, "Ignoring option outputorder=%.*s.",
                      p-opts, opts);
            continue;
        } ELIF_OPT("portrait") {
            CHECK_FOR_NO_ARGUMENT;
            orientation = 3;
            continue;
        } ELIF_OPT("landscape") {
            CHECK_FOR_NO_ARGUMENT;
            orientation = 4;
            continue;
        } ELIF_OPT("orientation-requested") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;

            i = atoi(opts);
            if (i < 3 || i > 6) {
                write_log(WRN, "Invalid value in option "
                          "orientation-requested=%i. Ignored.", i);
            } else {
                orientation = i;
            }
            continue;
        } ELIF_OPT("page-set") {
            write_log(WRN, "The page-set options is currently not supported. "
                      "Ignored.");
            SKIP_TO_END_OF_WORD;
            continue;
        } ELIF_OPT("cmdw-target-printer") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;
            printer_name = malloc((p-opts + 1) * sizeof(char));
            strncpy(printer_name, opts, p-opts);
            printer_name[p-opts] = '\0';
            continue;
        } ELIF_OPT("job-uuid") {
            SKIP_TO_END_OF_WORD;
            write_log(MSG, "Ignoring option %.*s.", p-opts, opts);
            continue;
        } ELIF_OPT("job-originating-host-name") {
            SKIP_TO_END_OF_WORD;
            write_log(MSG, "Ignoring option %.*s.", p-opts, opts);
            continue;
        } ELIF_OPT("collate") {
            if (*p == '=') {
                opts = ++p;
                SKIP_TO_END_OF_WORD;
                if (strncasecmp("true", opts, p-opts) == 0)
                    collate = 1;
                else if (strncasecmp("false", opts, p-opts) == 0)
                    collate = 0;
                else
                    write_log(WRN, "Unknown option duplex=%.*s. Ignored.",
                              p-opts, opts);
            } else {
                collate = 1;
            }
        } ELIF_OPT("nocollate") {
            CHECK_FOR_NO_ARGUMENT;
            collate = 0;
        } ELIF_OPT("duplex") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;
            if (strncasecmp("duplexnotumble", opts, p-opts) == 0) {
                duplex = 1;
            } else if (strncmp("duplextumble", opts, p-opts) == 0) {
                duplex = 2;
            } else if (strncmp("none", opts, p-opts) == 0) {
                duplex = 0;
            } else {
                write_log(WRN, "Unknown option duplex=%.*s. Ignored.",
                          p-opts, opts);
            }
        } ELIF_OPT_BEGIN("time-") {
            SKIP_TO_END_OF_WORD;
            write_log(MSG, "Ignoring option %.*s.", p-opts, opts);
            continue;
        }
        SKIP_TO_END_OF_WORD;
        write_log(DBG, "Copy option %.*s", p-opts, opts);
        argv[argc++] = "-o";
        strncpy(buf_p, opts, p-opts);
        argv[argc++] = buf_p;
        buf_p += p-opts;
        *(buf_p++) = '\0';
    }

    initialized = 1;
    if (!printer_name) {
#ifdef DEFAULT_PRINTER
        printer_name = DEFAULT_PRINTER;
#else
        cleanup_options();
        write_log(ERR, "Missing printer name (option cmdw-target-printer).");
        return -1;
#endif
    }

    if (!collate || duplex == 0) {
        argv[argc++] = "-n";
        argv[argc++] = oargv[4];
        manual_copies = 0;
    } else {
        manual_copies = 1;
    }

    argv[argc++] = "-d";
    argv[argc++] = printer_name;

    argv[argc++] = oargv[6];
    argv[argc] = NULL;

    write_log(DBG, "options done: %i", argc);

    return argc;
}

int prepare_odd_pages()
{
    if (!initialized)
        return -1;
    *outputorder = "outputorder=normal";
    *page_set = "page-set=odd";
    snprintf(*orientation_req, 24, "orientation-requested=%i", orientation);
    return 0;
}

int prepare_even_pages()
{
    if (!initialized)
        return -1;
    *outputorder = "outputorder=reverse";
    *page_set = "page-set=even";
    snprintf(*orientation_req, 24, "orientation-requested=%i",
             9-orientation);
    return 0;
}

int prepare_all_pages()
{
    if (!initialized)
        return -1;
    *outputorder = "outputorder=normal";
    *page_set = "page-set=all";
    snprintf(*orientation_req, 24, "orientation-requested=%i", orientation);
    return 0;
}

void cleanup_options()
{
    if (!initialized)
        return;
    free(*orientation_req);
    free(opt_buf);
    free(argv);
    initialized = 0;
}

