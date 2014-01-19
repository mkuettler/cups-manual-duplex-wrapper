/* cmdw.c - CUPS Manual Duplex Wrapper
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

/*
 * This is a cups backend program that allowes for manuel duplex printing
 * even when the printing driver does not support that option.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "cmdw-options.h"
#include "cmdw.h"

static char const *msg_name[] = {"", "Warning: ", "Error: ", "Debug: "};
static char const* logfilename = "/tmp/cups-mdw";
static FILE *logfile;

int open_log()
{
    logfile = fopen(logfilename, "a");
    if (!logfile)
        return 0;
    return 1;
}

void close_log()
{
    fclose(logfile);
}

void write_log(int mode, char const *msg, ...)
{
    time_t t = time(NULL);
    char *tstr = ctime(&t);
    char *p;
    char buf[1024];
    va_list arg_list;
#ifndef DEBUG
    if (mode == DBG)
        return;
#endif
    va_start(arg_list, msg);

    for (p = tstr; *p && *p != '\n'; ++p)
        ;
    *p = '\0';
    vsnprintf(buf, 1024, msg, arg_list);
    va_end(arg_list);
    fprintf(logfile, "[%s] %s%s\n", tstr, msg_name[mode], buf);
    fflush(logfile);
}

int call_lp(char** argv)
{
    int status;
    pid_t pid = fork();
    if (!pid) {
        if (execvp(PRINT_CMD, argv)) {
            write_log(ERR, "Call to lp failed: %s", strerror(errno));
            return -1;
        }
        return 0;
    }
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
        write_log(ERR, "lp exited abnormally.");
        return -1;
    }
    return 0;
}

#define ERROR(c, msg, ...)                      \
    do {                                        \
        write_log(ERR, msg, ##__VA_ARGS__);     \
        retval = c;                             \
        goto close;                             \
    } while (0);

#define CLOSE(c)                                \
    do {                                        \
        retval = c;                             \
        goto close;                             \
    } while (0);

int main(int argc, char **argv)
{
    struct passwd *passwd;
    char const *user = 0;
    gid_t *groups = 0;
    int ngroups;
    int retval = 0;
    char infilename[20];
    char *nargv[7];
    char buf[256];
    int fd;
    int n;
    int copies;
    char **lp_argv;

    infilename[0] = '\0';

    /* Check for valid call */
    if (argc == 1) {
        printf("file cdw:/ \"Unknown\" \"Cups Duplex Wrapper (cdw)\"\n");
        return 0;
    }
    if (setuid(0)) {
        fprintf(stderr, "cdw must be run with root privileges.\n");
        return 1;
    }

    if (argc < 6 || argc > 7) {
        fprintf(stderr, "Usage: %s job-id user title copies options "
                "[file]\n", argv[0]);
        return 1;
    }

    if (!open_log()) {
        fprintf(stderr, "Failed to open log file %s.\n", logfilename);
        return 4;
    }

    write_log(MSG, "Processing job %s.", argv[1]);

    /* Fetch user information */
    user = argv[2];
    passwd = getpwnam(user);
    if (!passwd)
        ERROR(2, "Failed to get user information for user %s.\n", user);

    ngroups = 16;
    groups = calloc(ngroups, sizeof(gid_t));
    if (groups == NULL) ERROR(3, "Failed to allocate memory");
    if (getgrouplist(user, passwd->pw_gid, groups, &ngroups) == -1) {
        free(groups);
        groups = calloc(ngroups, sizeof(gid_t));
        if (groups == NULL) ERROR(3, "Failed to allocate memory");
    }
    if (getgrouplist(user, passwd->pw_gid, groups, &ngroups) == -1)
        ERROR(2, "Failed to get groupd list");

    /* Change to normal user */
    if (setgid(passwd->pw_gid))
        ERROR(2, "Failed to change group id");

    if (setgroups(ngroups, groups))
        ERROR(2, "Failed to set groups");

    if (setuid(passwd->pw_uid))
        ERROR(2, "Failed to set user id");

    write_log(MSG, "Switched to user %s", user);
    umask(0077);

    /* temporary: write out the options */
    fd = open("/home/martin/cups-duplex-out", O_WRONLY | O_APPEND);
    for (n = 0; n < argc; ++n) {
        if (write(fd, argv[n], strlen(argv[n])) != (int)strlen(argv[n]))
            write_log(WRN, "debug write %i failed", n);
        if (write(fd, "\n", 1) != 1)
            write_log(WRN, "debug write %i.1 failed", n);
    }
    close(fd);

    fd = -1;
    /* If input is stdin, write that data into a file, because
       it will be needed twice. */
    if (argc == 6) {
        memcpy(nargv, argv, 6*sizeof(char*));
        nargv[6] = infilename;
        strcpy(infilename, "/tmp/cmdw-XXXXXX");
        fd = mkstemp(infilename);
        if (fd < 0)
            ERROR(2, "Failed to create temporary file.");
        while ((n = read(fileno(stdin), buf, 256)) > 0) {
            if (write(fd, buf, n) != n) {
                close(fd);
                ERROR(2, "Failed writing to temporary file.");
            }
        }
        close(fd);
        write_log(DBG, "Created temporary file %s", infilename);
    }

    n = parse_and_assemble_options(argc == 6 ? nargv : argv, &lp_argv);
    write_log(DBG, "parse_options returned %i", n);
    if (n < 0)
        CLOSE(4);

    if (!duplex) {
        write_log(MSG, "Printing all pages");
        prepare_all_pages();
        if (call_lp(lp_argv))
            CLOSE(5);
        CLOSE(0);
    }

    if (manual_copies)
        copies = atoi(argv[4]);
    else
        copies = 1;

    for (; copies; --copies) {
        prepare_even_pages();
        write_log(MSG, "Printing even pages");
        if (call_lp(lp_argv))
            CLOSE(5);


        n = system("DISPLAY=:0 "
                   "kdialog --yesno \"When the printer is done, please put "
                   "the pages back into \nthe input tray and click Continue "
                   "to print the odd pages.\""
                   " --yes-label Continue --no-label Cancel");
        if (!WIFEXITED(n) || WEXITSTATUS(n)) {
            if (WIFEXITED(n))
                write_log(DBG, "Exit code %i", WEXITSTATUS(n));
            write_log(MSG, "Stop printing: user aborted.");
            CLOSE(0);
        }

        prepare_odd_pages();
        write_log(MSG, "Printing odd pages");
        if (call_lp(lp_argv))
            CLOSE(5);
    }

    write_log(DBG, "work done");
    CLOSE(0);

 close:
    if (groups) free(groups);
    if (infilename[0]) unlink(infilename);
    close_log();

    return retval;
}
