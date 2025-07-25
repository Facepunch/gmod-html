#include <sys/syslimits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

typedef int (*LauncherMain_t)(int, char**);

int main(int argc, char** argv)
{
	char executablePath[PATH_MAX];
	unsigned int realPathSizeUInt = sizeof(executablePath);
	int returnCode;

	memset(executablePath, 0, PATH_MAX);
	returnCode = _NSGetExecutablePath(executablePath, &realPathSizeUInt);
	if (returnCode != 0) {
		puts("_NSGetExecutablePath failed");
		return 1;
	}

	for (int i = 1; i <= 4; ++i) {
		char *lastSlash = strrchr(executablePath, '/');
		if (lastSlash != (char *)0) {
			*lastSlash = (char)0;
		}
	}

	returnCode = chdir(executablePath);
	if (returnCode != 0) {
		fprintf(stderr, "Failed to change directory (%s)\n", executablePath);
		return 1;
	}

	void *launcherHandle = dlopen("GarrysMod_Signed.app/Contents/MacOS/launcher.dylib", RTLD_NOW);
	if (!launcherHandle) {
		launcherHandle = dlopen("GarrysMod.app/Contents/MacOS/launcher.dylib", RTLD_NOW);
	}

	if (!launcherHandle) {
		char *errorMsg = dlerror();
		fprintf(stderr, "Failed to load the launcher (%s)\n", errorMsg);
		return 1;
	}

	LauncherMain_t launcherMainFn = (LauncherMain_t)dlsym(launcherHandle, "LauncherMain");

	if (!launcherMainFn) {
		puts("Failed to load the launcher entry proc\n");
		return 1;
	}

	return launcherMainFn(argc, argv);
}
