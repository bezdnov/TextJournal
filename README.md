# Text Journal App
Following project consists of 2 parts: dynamic library and a console application which runs it.

## Build
`make all`

## Run
`LD_LIBRARY_PATH=. ./text_journal`

## Clean temporary files
`make clean`

# Internals
## Journal structure and message structure in text file
Journal is just a text file

The message consists of 3 lines. First is importance level (number from 0 to 2), second is time, third is message.
The messages are separated by empty line, and 
This is example journal:

```
1
2026-08-13T08:57:27Z
This is a important message

2
2026-08-13T08:57:27Z
This is a special message
```

## Usage
