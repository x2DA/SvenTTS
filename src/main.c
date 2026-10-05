#include "memory.h"
#include "types.h"
#include "offsets.h"

#include <stdio.h>
#include <stdlib.h>

#define maxMessageLength 256 // Length of each message in the queue (+1).
#define maxMessages 10 // Amount of messages in the queue.

#define maxCmdLen 512 // The length of the buffer that holds the espeak-ng call.
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

// Skips system messages. I don't really like this.
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
	if (pid < 0) {
		printf("Could not get pid.");
		return -1;
	}

	uptr client = getModuleBase(pid, "client.so", 0);
	if (client < 0) {
		printf("Could not get module.");
		return -1;
	}

	char messageQueue[maxMessages][maxMessageLength];
	char oldLastMessage[maxMessageLength];
	int running = 1;

	int lastMessageIndex = -1;
	int latestMessageIndex = -1;

	char commandBuffer[maxCmdLen];

	while (running) {
		// Get messages chunk.
		readMem(pid, (client+messageArrayOffset), &messageQueue, sizeof(messageQueue));

		for (latestMessageIndex = 0; latestMessageIndex < maxMessages; latestMessageIndex++) {
			// Update latest message index.
			if (messageQueue[latestMessageIndex][0] == 0) {
				latestMessageIndex--;
				break;
			}
		}


		// Messages expiring.
		if (latestMessageIndex < lastMessageIndex) {
			goto end_frame;
		}

		// Deal with repeats.
		if (latestMessageIndex == lastMessageIndex && startsWith(oldLastMessage, messageQueue[latestMessageIndex])) {
			goto end_frame;
		}
		
		// Skips system messages.
		if (isBlacklisted(messageQueue[latestMessageIndex])) {
			goto end_frame;
		}

		// Log & call espeak.
		printf("%s\n", messageQueue[latestMessageIndex]);
		snprintf(commandBuffer, sizeof(commandBuffer), "espeak-ng -s 230 -p 49 -g 2 -k 20 \"%s\"", messageQueue[latestMessageIndex]);
		system(commandBuffer);

		lastMessageIndex = latestMessageIndex;
		snprintf(oldLastMessage, sizeof(oldLastMessage), "%s", messageQueue[latestMessageIndex]);

		end_frame:
		usleep(frameDelay);
	}

	return 0;
}

