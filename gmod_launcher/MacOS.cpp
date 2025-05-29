#include <mach-o/dyld.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

#define REALPATH_BUF_SIZE 4096 // NOTE: Original has 1024
typedef int (*LauncherMain_t)(int, char**);

int main(int argc, char** argv)
{
	char realPathOut[REALPATH_BUF_SIZE];
	unsigned int realPathSizeUInt = sizeof(realPathOut);
	int returnCode;
	char *unused;

	memset(realPathOut, 0, REALPATH_BUF_SIZE);
	returnCode = _NSGetExecutablePath(realPathOut, &realPathSizeUInt);
	if (returnCode != 0) {
		puts("_NSGetExecutablePath failed");
	}
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

	void *launcherHandle = dlopen("GarrysMod_Signed.app/Contents/MacOS/launcher.dylib", RTLD_NOW);
	if (!launcherHandle) {
		launcherHandle = dlopen("GarrysMod.app/Contents/MacOS/launcher.dylib", RTLD_NOW);
	}

	if (!launcherHandle) {
		char *errorMsg = dlerror();
		fprintf(stderr, "Failed to load the launcher (%s)\n", errorMsg);
		return 1;
	} else {
		LauncherMain_t launcherMainFn = (LauncherMain_t)dlsym(launcherHandle, "LauncherMain");

		if (!launcherMainFn) {
			puts("Failed to load the launcher entry proc\n");
			return 1;
		} else {
			return launcherMainFn(argc, argv);
		}

		//dlclose(launcherHandle);
	}
}
