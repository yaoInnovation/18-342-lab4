/** @file hello.c
 *
 * @brief Prints out Hello world using the syscall interface.
 *
 * Links to libc.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-10-29
 */
#include <unistd.h>


const char hello[] = "Hello World\r\n";

int main()
{
	char buffer[255] = {0};
	int size = read(STDIN_FILENO, buffer, 255);
	write(STDOUT_FILENO, buffer, size);
	return 0;
}
