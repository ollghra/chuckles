#ifndef _SYS_IO_H
#define _SYS_IO_H 1

	static inline unsigned char
inb (unsigned short int port)
{
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

	static inline unsigned short int
inw (unsigned short int port)
{
	unsigned short _v;
	__asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

	static inline unsigned int
inl (unsigned short int port)
{
	unsigned int _v;
	__asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

	static inline void
outb (unsigned short int port, unsigned char value)
{
	__asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (port));
}

	static inline void
outw (unsigned short int port, unsigned short int value)
{
	__asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd" (port));
}

	static inline void
outl (unsigned short int port, unsigned int value)
{
	__asm__ __volatile__ ("outl %0,%w1": :"a" (value), "Nd" (port));
}

#endif
