#include <stdio.h>

#include <sys/shell.h>
#include <sys/kb.h>

int shell_key(struct key_event e)
{
    if(!(e.flags & KB_FLAG_SPECIAL) && e.flags & KB_FLAG_DOWN)
    {
        printf("%c", e.utf8);
    }
    else if (e.flags & KB_FLAG_DOWN){
        //printf("%c%c%c%c+", e.flags & KB_FLAG_SHIFT ? 's':' ',e.flags & KB_FLAG_CONTROL ? 'c':' ',  e.flags & KB_FLAG_ALT ? 'a':' ', e.flags & KB_FLAG_MOD ? 'm':' ');
        switch (e.code)
        {
            case KEY_UP_ARROW:
                printf("<UP>");
                break;
            case KEY_DOWN_ARROW:
                printf("<DOWN>");
                break;
            case KEY_LEFT_ARROW:
                printf("<LEFT>");
                break;
            case KEY_RIGHT_ARROW:
                printf("<RIGHT>");
                break;
            case KEY_PAGE_UP:
                printf("<PG UP>");
                break;
            case KEY_PAGE_DOWN:
                printf("<PG DOWN>");
                break;
            default:
                printf("<0x%x>",e.code);
                break;
        }
    }
    return -1;
}
