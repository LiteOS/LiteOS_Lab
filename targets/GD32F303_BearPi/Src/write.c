/* See LICENSE of license details. */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "stub.h"
#include "gd32f30x.h"
#include <stdarg.h>

typedef unsigned int size_t;

extern int _put_char(int ch) __attribute__((weak));


#define LOG_BUF_SIZE  256  ///< you could modify it

int printf(const char *format, ...)
{
    int ret;
    char str_buf[LOG_BUF_SIZE] = {0};
    va_list list;

    memset(str_buf, 0, LOG_BUF_SIZE);
    va_start(list, format);
    ret = vsnprintf(str_buf, LOG_BUF_SIZE, format, list);
    va_end(list);

    puts(str_buf);
    return ret;

}

//ssize_t _write(int fd, const void* ptr, size_t len) {
//	const uint8_t * current = (const uint8_t *) ptr;
//
////	if (isatty(fd))
//	{
//		for (size_t jj = 0; jj < len; jj++) {
//			_put_char(current[jj]);
//
//		}
//		return len;
//	}
//
//	return _stub(EBADF);
//}
//
//int _put_char(int ch)
//{
//    usart_data_transmit(USART0, (uint8_t) ch );
//    while (usart_flag_get(USART0, USART_FLAG_TBE)== RESET){
//    }
//
//    return ch;
//}

