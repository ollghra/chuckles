#include <stdio.h>

#include <sys/shell.h>
#include <sys/kb.h>

int shell_key(struct key_event e)
{
	//  if(!(e.flags & 1) && e.down) 
	//   printf("%c F:[%c,%c,%c]", e.utf8, 
	//   (e.flags & (1<<1)) == (1<<1)? 'C':'0', (
	//   e.flags & (1<<2)) == (1<<2) ? 'S': '0', 
	//   (e.flags & (1<<3)) == (1<<3) ? 'A': '0');
	if(~e.flags & 1 && e.flags & 0x80)
	{
		if(e.flags & (1 << 2))
			e.utf8 -= 0x20;
		printf("%c", e.utf8);
	}
	return -1;
}
