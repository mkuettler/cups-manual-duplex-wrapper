/* cmdw-options.c - CUPS Manual Duplex Wrapper, option handling code
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMDW_OPTIONS
#include "cmdw-options.h"
#include "cmdw.h"

#define MIN(a,b) (a < b ? a : b)

#define IF_OPT(o) if (strncmp(o, opts, opts-p) == 0)
#define ELIF_OPT(o) else IF_OPT(o)
#define ELIF_OPT_BEGIN(o)                                               \
    else if (strncmp(o, opts, MIN(strlen(o), (size_t)(opts-p))) == 0)

#define SKIP_TO_NEXT_WORD                       \
    for (; p && *p == ' '; ++p)                 \
        ;                                       \
    opts = p;

#define SKIP_TO_END_OF_WORD                     \
    for (; p && *p != ' '; ++p)                 \
        ;

#define CHECK_FOR_ARGUMENT                                      \
    if (*p == ' ') {                                            \
        write_log(WRN, "Option %.*s needs an argument. "        \
                  "Ignored.", opts, p-opts);                    \
        continue;                                               \
    }                                                           \
    opts = ++p;

#define CHECK_FOR_NO_ARGUMENT                                   \
    if (*p == '=') {                                            \
        write_log(WRN, "Invalid argument to option %.*s. "      \
                  "Ignored.", opts, opts-p);                    \
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

    /* count number of options in opts */
    for (p = opts; p; ++p) {
        if (*p == ' ') ++nopts;
        while (*p == ' ') ++p;
        if (!p) break;

        if (*p == '\'')
            for(; p && *p != '\''; ++p)
                if (*p == '\\') ++p;
        else if (*p == '"')
            for(; p && *p != '"'; ++p)
                if (*p == '\\') ++p;
        if (!p) {
            write_log(ERR, "Failed parsing options: "
                      "Opening string doesn't close");
            return -1;
        }
    }

    opt_buf = malloc((strlen(opts)+1) * sizeof(char));
    buf_p = opt_buf;
    argv = *argvp = calloc(2*nopts + 13, sizeof(char*));
    argc = 0;
    argv[argc++] = "echo";
    /* Do not pass number of copies - we handle that manually (for now) */
    argv[argc++] = "-t";
    argv[argc++] = oargv[3];
    argv[argc++] = "-o";
    argv[argc++] = "sides=one-sided";
    argv[argc++] = "-o";
    *outputorder = argv[argc++];
    argv[argc++] = "-o";
    *page_set = argv[argc++];
    argv[argc++] = "-o";
    *orientation_req = argv[argc++] = malloc(24 * sizeof(char));

    /* set default values */
    duplex = 0;
    orientation = 3;
    printer_name = NULL;

    /* parse and copy options */
    for(p = opts; p; ) {
        SKIP_TO_NEXT_WORD;

        for (; p && *p != '=' && *p != ' '; ++p)
            ;
        if (!p) break;

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
                          opts, p-opts);
            }
            continue;
        } ELIF_OPT("outputorder") {
            CHECK_FOR_ARGUMENT;
            SKIP_TO_END_OF_WORD;

            write_log(MSG, "Option outputorder=%.*s ignored.",
                      opts, p-opts);
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
            printer_name = malloc((opts-p + 1) * sizeof(char));
            strncpy(printer_name, opts, opts-p);
            printer_name[opts-p] = '\0';
            continue;
        } ELIF_OPT_BEGIN("job-") {
            write_log(MSG, "Ignoring option %.*s.", opts, opts-p);
            SKIP_TO_END_OF_WORD;
            continue;
        } ELIF_OPT_BEGIN("time-") {
            write_log(MSG, "Ignoring option %.*s.", opts, opts-p);
            SKIP_TO_END_OF_WORD;
            continue;
        }
        SKIP_TO_END_OF_WORD;
        argv[argc++] = "-o";
        strncpy(buf_p, opts, opts-p);
        argv[argc++] = buf_p;
        buf_p += opts-p;
        *(buf_p++) = '\0';
    }

    argv[argc++] = oargv[6];
    argv[argc] = NULL;

    initialized = 1;
    if (!printer_name) {
        write_log(ERR, "Missing printer name (option cmdw-target-printer).");
        cleanup_options();
        return -1;
    }

    return argc;
}

int prepare_odd_pages()
{
    if (!initialized)
        return -1;
    *outputorder = "outputorder=normal";
    *page_set = "page-set=odd";
    snprintf(*orientation_req, 24, "orientation_requested=%i", orientation);
    return 0;
}

int prepare_even_pages()
{
    if (!initialized)
        return -1;
    *outputorder = "outputorder=reverse";
    *page_set = "page-set=even";
    snprintf(*orientation_req, 24, "orientation_requested=%i",
             3+(orientation-1)%4);
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

