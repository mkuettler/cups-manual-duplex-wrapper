/* cdw.cc

   ...
 */

#include <stdio.h>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    pid_t pid;

    struct passwd *passwd;
    char *user = 0;
    git_t *groups = 0;
    int ngroups;
    int retval = 0;
    char* args[8];
    int i;

    if (argc == 1) {
        printf("file cdw:/ \"Unknown\" \"Cups Duplex Wrapper (cdw)\"");
        return 0;
    }

    if (setuid(0)) {
        fprintf(stderr, "cdw must be run with root privileges.\n");
        return 1;
    }

    if (argc < 6 || argc > 7) {
        fprintf(stderr, "Usage: cdw job-id user title copies options "
                "[file]\n");
        return 1;
    }

    user = argv[2];
    passwd = getpwnam(user);
    if (!passwd) {
        fprintf(stderr, "Failed to get user information for user %s.\n",
                user);
        retval = 2;
        goto close;
    }

    ngroups = 16;
    groups = calloc(ngroups, sizeof(gid_t));
    if (groups == NULL) goto alloc_failed;
    if (getgrouplist(user, passwd->pw_gid, groups, &ngroups) == -1) {
        free(groups);
        groups = calloc(ngroups, sizeof(gid_t));
        if (groups == NULL) goto alloc_failed;
        if (getgrouplist(user, passwd->pw_gid, groups, &ngroups) == -1) {
            fprintf(stderr, "Failed to get groupd list.\n");
            retval = 2;
            goto close;
        }
    }

    if (setgid(passwd->pw_gid)) {
        fprintf(stderr, "Failed to change groud id.\n");
        retval = 2;
        goto close;
    }
    if (setgroups(ngroups, groups)) {
        fprintf(stderr, "Failedto set groups.\n");
        retval = 2;
        goto close;
    }
    if (setuid(passwd->pw_uid)) {
        fprintf(stderr, "Failed to set user id.\n");
        retval = 2;
        goto close;
    }

    free(groups);
    args[0] = "cups-duplex-wrapper-frontend";
    for (i = 1; i < argc; ++i)
        args[i] = argv[i];
    args[argc] = NULL;
    execvp(args[0], args);

 alloc_failed:
    fprintf(stderr, "Failed to allocate memory.\n");
    retval = 3;
    goto close;

 close:
    if (groups) free(groups);

    return retval;
}
