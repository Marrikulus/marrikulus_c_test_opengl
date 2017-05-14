#include <stdio.h>
#include <stdlib.h>

#ifndef MARLIB_H
# define MARLIB_H

char* ReadFile(char* filename)
{
	char* buffer = NULL;
	int stringSize, readSize;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
		fseek(handler, 0, SEEK_END);
		stringSize = ftell(handler);
		rewind(handler);

		buffer = (char*)malloc(sizeof(char) * (stringSize + 1));

		readSize = fread(buffer, sizeof(char), stringSize, handler);

		buffer[stringSize] = '\0';

		if (stringSize != readSize)
		{
			free(buffer);
			buffer = NULL;
		}

		fclose(handler);
	}

	return buffer;
}

#endif
