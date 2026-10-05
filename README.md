# SvenTTS
An external Text to Speech program for the linux version of Sven Co-Op.

## Dependencies
`espeak-ng`

## Compiling
`./build.sh`
Alternatively,
`gcc -o SvenTTS src/*.c`

## Disclaimer
This is a really shabby prototype which was made in under an hour ("reversing" included).
Even though it has a legitimate use-case, anti cheat software may still flag this as a malicious program, as it reads memory the same way cheat software would.

## Explanation
The chatbox acts more or less like a queue:
`messages[0]: Hey!(1)`
Another user sends another message, the queue becomes:
`messages[0]: Hey! (1)`
`messages[1]: Hello! (2)`
The initial message expires:
`messages[0]: Hello! (2)`

It can hold 10 zero-terminated messages of length 255.
