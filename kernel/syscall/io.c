/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 *
 * @author Yao Zhou <yaozhou@andrew.cmu.edu>
 *         Congshan Lv <congshal@andrew.cmu.edu>
 * @date   Fri, 21 Nov 2014 23:18:17
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>

#define EOT_CHAR 0x04
#define DEL_CHAR 0x7f


/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read_syscall(int fd __attribute__((unused)), void *buf __attribute__((unused)), size_t count __attribute__((unused)))
{
	// a variable to record how many byte read
	unsigned read_byte = 0;
	// loaded char
	char ch = 0;
	// check file descriptor
	if(fd!=STDIN_FILENO) {
		return -EBADF;
	}

	// check read memory range
	if( (unsigned)&buf < 0xa0000000 || 
		(unsigned)&buf > 0xa3ffffff) {
		// invalid address!
		return -EFAULT;
	}

	// reading from stdin into buffer and output it to stdout
	while(read_byte < count) {
		ch = getc();

		// return if EOT occurred
		if( ch == EOT) {
			return read_byte;
		}

		switch(ch) {
			case backspace: {
				read_byte--;
				buf[read_byte] = 0;
				puts("\b \b");
				break;
			}
			case delete: {
				read_byte--;
				buf[read_byte] = 0;
				puts("\b \b");
				break;
			}
			case '\r': {
				buf[read_byte] = ch;
				read_byte++;
				putc('\n');
				return read_byte;
			}
			case '\n': {
				buf[read_byte] = ch;
				read_byte++;
				putc(ch);
				return read_byte;
			}
			default: {
				buf[read_byte] = ch;
				read_byte++;
				putc(ch);
			}
		}
	}
	return read_byte;
}

/* Write count bytes to fd from the buffer buf. */
ssize_t write_syscall(int fd  __attribute__((unused)), const void *buf  __attribute__((unused)), size_t count  __attribute__((unused)))
{
	// a variable to record how many byte already wrote
	unsigned byte_write = 0;
	// check file descriptor
	if(fd != STDOUT_FILENO) {
		return -EBADF;
	}
	// check dest memory range
	// the address should on be either inside RAM or ROM
	if(((unsigned)&buf > 0xa0000000 && (unsigned)&buf < 0xa3ffffff) || 
		((unsigned)&buf > 0x00000000 && (unsigned)&buf < 0x00ffffff) ) {
		//valid address
		while(byte_write < count) {
			//printf("byte_write:%d\n",byte_write);
			if(buf[byte_write] == '\0') {
				// the end of string
				return byte_write;
			}
			else {
				putc(buf[byte_write]);
				byte_write++;
			}
		}
	}else {
		// invalid address, return error!
		return -EFAULT;
	}
	return byte_write;
}

