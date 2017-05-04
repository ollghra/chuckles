#include <stdlib.h>
#include <string.h>

// Not working until malloc() implemented
char *strdup(const char *string)
{
      char *new;
      /*
      if (NULL != (new = malloc(strlen(string) + 1)))
            strcpy(new, string);
      */
      
      return new;
}
