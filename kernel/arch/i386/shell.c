#include <stdio.h>

#include <sys/shell.h>
#include <sys/kb.h>

int shell_key(struct key_event e)
{
    if(~e.flags & KB_FLAG_SPECIAL && e.flags & KB_FLAG_DOWN)
    {
        printf("%c", e.utf8);
    }
    return -1;
}
