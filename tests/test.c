#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DELIM ":"

int main()
{

    char str[89] = "12:off"; 
    char delimiter[1] = ":";
    char *token = strtok(str, DELIM);

    char logBuffer[10];
    int first = 1;

    while (token != NULL)
    {
        if (first)
        {
            printf("%s\n", logBuffer);  
            first = 0;
            
            strcat(logBuffer, token);
            printf("%s\n", logBuffer);  
            strcat(logBuffer, delimiter);
            printf("%s\n", logBuffer);  
        }
        else
        {
            if (strcmp(token, "1") == 0) {
                strcat(logBuffer, "on");
            } else {
                strcat(logBuffer, "off");
            }
            printf("%s\n", logBuffer);  
        }
        token = strtok(NULL, DELIM);
    }
}