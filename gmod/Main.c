#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#define REALPATH_BUF_SIZE 4096
#define LIBRARY_FILENAME "launcher_client.so"
#define LIBRARY_LAUNCHER_FN "LauncherMain"
typedef int (*arbitrary)();

char has_namespace_support = 0x0;

void calc_has_namespace_support()
{
    __pid_t clonedProcessPid;
    __pid_t stoppedProcessPid;
    int unshareSuccessOrFail;
    int statusCode;

    clonedProcessPid = fork();
    if (clonedProcessPid == -1)
    {
        puts("fork failed... assuming unprivileged usernamespaces are disabled");
    }
    else
    {
        if (clonedProcessPid == 0)
        {
            unshareSuccessOrFail = unshare(0x10000000);
            if (unshareSuccessOrFail != -1)
            {
                exit(0);
            }
            fprintf(stderr, "unshare(CLONE_NEWUSER) failed, unprivileged usernamespaces are probably disabled\n");
            exit(1);
        }
        stoppedProcessPid = waitpid(clonedProcessPid, &statusCode, 0);
        if (clonedProcessPid == stoppedProcessPid)
        {
            has_namespace_support = statusCode == '\0';
            return;
        }
        puts("waitpid failed... assuming unprivileged usernamespaces disabled");
    }
    return;
}

int main(int argc, char **argv)
{
    char realPathOut[4104];
    char *unused;
    int returnValue = 0;
    if (getenv("container") || getenv("APPIMAGE") || getenv("SNAP"))
    {
        printf("[GModCefPatch] Overriding \"has_namespace_support\"...\n");
        has_namespace_support = 1;
    }
    else
        calc_has_namespace_support();
    memset(realPathOut, 0, REALPATH_BUF_SIZE);
    realpath("/proc/self/exe", realPathOut);
    unused = strrchr(realPathOut, '/');
    if (unused != (char *)0x0)
    {
        *unused = '\0';
        unused = strrchr(realPathOut, '/');
        if (unused != (char *)0x0)
        {
            *unused = '\0';
            unused = strrchr(realPathOut, '/');
            if (unused != (char *)0x0)
            {
                *unused = '\0';
            }
        }
    }
    chdir(realPathOut);
    void *launcherHandle = dlopen(LIBRARY_FILENAME, RTLD_NOW);
    if (launcherHandle == 0)
    {
        char *errorMsg = dlerror();
        fprintf(stderr, "Failed to load the launcher (%s)\n", errorMsg);
        returnValue = 1;
    }
    else
    {
        arbitrary launcherMainFn;
        *(void **)(&launcherMainFn) = dlsym(launcherHandle, LIBRARY_LAUNCHER_FN);
        if (launcherMainFn == 0x0)
        {
            puts("Failed to load the launcher entry proc");
            returnValue = 1;
        }
        else
        {
            returnValue = ((*launcherMainFn)(argc, argv));
        }
        dlclose(launcherHandle);
    }
    return returnValue;
}
