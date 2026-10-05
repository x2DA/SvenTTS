#include "memory.h"
#include "types.h"
#include "offsets.h"

#include <stdio.h>
#include <stdlib.h>

#define maxMessageLength 256
#define maxMessages 10
#define frameDelay 24000 // 40000 // 0.04 secs

int startsWith(const char *substring, const char *string) {
	while (*substring != '\0') {
		if (*string == '\0') { return 0; }
		if (*substring != *string) { return 0; }
		substring++;
		string++;
	}

	return 1;
}

int isBlacklisted(const char *string) {
	char blacklist[3] = "$\'\"";
	while (*string != '\0') { // FIXME: Hardcoded.
		for (int i = 0; i < 3; i++) {
			if (*string == blacklist[i]) { return 1; }
		}
		string++;
	}

	return 0;
}


int main() {
	pid_t pid = getProgramPID("svencoop_linux");
	if (pid < 0) { return -1; }

	uptr client = getModuleBase(pid, "client.so", 0);
	if (client < 0) { return -1; }

	char messageQueue[maxMessages][maxMessageLength];
	char oldLastMessage[maxMessageLength];
	int running = 1;

	int lastMessageIndex = -1;
	int latestMessageIndex = -1;
	char hasButDontInclude[6] = "User: "; // String starts with this, but don't include it in the refined output, zero terminated.
	// FIXME: Hardcoded.


	char commandBuffer[512]; // FIXME: Hardcoded.
	while (running) {
		// Get messages chunk.
		readMem(pid, (client+messageArrayOffset), &messageQueue, sizeof(messageQueue));

		for (latestMessageIndex = 0; latestMessageIndex <= maxMessages; latestMessageIndex++) {
			if (messageQueue[latestMessageIndex][0] == 0) { latestMessageIndex--; break; }
		}

		if (latestMessageIndex < lastMessageIndex ) { usleep(frameDelay); continue;}
		if (latestMessageIndex == lastMessageIndex && startsWith(oldLastMessage, messageQueue[latestMessageIndex])) {
			usleep(frameDelay); continue;
		}

		
		if (!startsWith(hasButDontInclude, messageQueue[latestMessageIndex]+1)) { usleep(frameDelay); continue; }
		if (isBlacklisted(messageQueue[latestMessageIndex]+13)) { usleep(frameDelay); continue; }

		snprintf(commandBuffer, sizeof(commandBuffer), "espeak-ng -s 230 -p 49 -g 2 -k 20 \"%s\"", messageQueue[latestMessageIndex]+13);
		system(commandBuffer);

		lastMessageIndex = latestMessageIndex;
		snprintf(oldLastMessage, sizeof(oldLastMessage), "%s", messageQueue[latestMessageIndex]);
		usleep(frameDelay);
	}

	return 0;
}

