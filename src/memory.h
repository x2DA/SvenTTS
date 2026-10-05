#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <unistd.h>

#define MAXCMDSIZE 256 // for stuff like pidof -s NAME
#define MAXLINESIZE 16 // output, for the above it'd be the pid


// returns program's pid
pid_t getProgramPID(const char *programName);

// returns the start address of modulename's nth region.
// example usage: getModuleBase(1234, client.so, 0); // equivalent to client.so[0] or client.so
uptr getModuleBase(pid_t pid, const char *moduleName, int region);

// reads size bytes from address into buffer
// returns 1 on success
int readMem(pid_t pid, uptr address, void *buffer, size_t size);


// writes size bytes from buffer into address
// returns 1 on success
int writeMem(pid_t pid, uptr address, void *buffer, size_t size);

#endif

