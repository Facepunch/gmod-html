#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>

typedef int (*LauncherMain_t)(int, char**);

char has_namespace_support = 0x0;

void calc_has_namespace_support()
{
	__pid_t clonedProcessPid;
	__pid_t stoppedProcessPid;
	int unshareSuccessOrFail;
	int statusCode;

	clonedProcessPid = fork();
	if (clonedProcessPid == -1) {
		puts("fork failed... assuming unprivileged usernamespaces are disabled");
	} else {
		if (clonedProcessPid == 0) {
			unshareSuccessOrFail = unshare(0x10000000);

			if (unshareSuccessOrFail != -1) {
				exit(0);
			}

			fprintf(stderr, "unshare(CLONE_NEWUSER) failed, unprivileged usernamespaces are probably disabled\n");
			exit(1);
		}

		stoppedProcessPid = waitpid(clonedProcessPid, &statusCode, 0);
		if (clonedProcessPid == stoppedProcessPid) {
			has_namespace_support = statusCode == '\0';
			return;
		}

		puts("waitpid failed... assuming unprivileged usernamespaces disabled");
	}

	return;
}

int main(int argc, char** argv)
{
	char realPathOut[PATH_MAX];
	char *unused;

	if (getenv("container") || getenv("APPIMAGE") || getenv("SNAP")) {
		printf("Container detected, overriding \"has_namespace_support\"...\n");
		has_namespace_support = 1;
	} else {
		calc_has_namespace_support();
	}

	memset(realPathOut, 0, PATH_MAX);
	realpath("/proc/self/exe", realPathOut);
	unused = strrchr(realPathOut, '/');

	if (unused != (char *)0x0) {
		*unused = '\0';
		unused = strrchr(realPathOut, '/');

		if (unused != (char *)0x0) {
			*unused = '\0';
			unused = strrchr(realPathOut, '/');

			if (unused != (char *)0x0) {
				*unused = '\0';
			}
		}
	}

	chdir(realPathOut);

	void *launcherHandle = dlopen("launcher_client.so", RTLD_NOW);
	if (!launcherHandle) {
		char *errorMsg = dlerror();
		fprintf(stderr, "Failed to load the launcher (%s)\n", errorMsg);
		return 1;
	} else {
		LauncherMain_t launcherMainFn = (LauncherMain_t)dlsym(launcherHandle, "LauncherMain");

		if (!launcherMainFn) {
			fprintf(stderr, "Failed to load the launcher entry proc\n");
			return 1;
		} else {
			return launcherMainFn(argc, argv);
		}

		//dlclose(launcherHandle);
	}
}
