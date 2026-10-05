#define _GNU_SOURCE

#include "memory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/uio.h>

pid_t getProgramPID(const char *programName) { // TODO: Length check for programName.
	char cmd[256]; // FIXME: Hardcoded.
	char line[16]; // FIXME: Hardcoded.

	snprintf(cmd, sizeof(cmd), "pidof -s %s", programName);

	FILE *file = popen(cmd, "r");
	if (!file) {
	//	printf("!Permissions\n"); // no perms.
		return -1;
	}

	pid_t pid = -1;

	if (fgets(line, sizeof(line), file)) {
		pid = (pid_t)strtoul(line, NULL, 10); // TODO: Check for non empty.
	}
	
	pclose(file);
	return pid;
}

uptr getModuleBase(pid_t pid, const char *moduleName, int region) {
	char mapsPath[256]; // FIXME: Hardcoded

	snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);

	FILE *file = fopen(mapsPath, "r");
	if (!file) {
		// printf("!/maps\n"); // no maps.
		return -1;
	}


	char line[512]; // FIXME: Hardcoded
	uptr baseAddr = 0;
	int currentRegion = -1; // 0 indexed.
	while (fgets(line, sizeof(line), file)) {
		if (strstr(line, moduleName)) {
			currentRegion++;
			if ( currentRegion < region) { continue; }
			sscanf(line, "%llx-", &baseAddr);
			// TODO: Validate success.
			break;
		}
	}

	fclose(file);
	return baseAddr;
}

int readMem(pid_t pid, uptr address, void *buffer, size_t size) {
	struct iovec local = { .iov_base = buffer, .iov_len = size };
	struct iovec remote = { .iov_base = (void *)address, .iov_len = size };

	return process_vm_readv(pid, &local, 1, &remote, 1, 0) == size;
}

int writeMem(pid_t pid, uptr address, void *buffer, size_t size) {
    struct iovec local = { .iov_base = buffer, .iov_len = size };
    struct iovec remote = { .iov_base = (void *)address, .iov_len = size };

    return process_vm_writev(pid, &local, 1, &remote, 1, 0) == size;
}

