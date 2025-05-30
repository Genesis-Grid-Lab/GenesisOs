#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (put_char(bytes[i]) == EOF)
			return false;
	return true;
}

// Convert unsigned int to a decimal string
static void itoa_dec(unsigned int num, char* buffer) {
    char temp[11];  // Max length for 32-bit unsigned int
    int i = 0;

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Reverse string
    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

// Convert unsigned int to hex string
static void itoa_hex(unsigned int num, char* buffer) {
    const char* hex_digits = "0123456789ABCDEF";
    char temp[9];  // Max 8 hex digits + null terminator
    int i = 0;

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (num > 0) {
        temp[i++] = hex_digits[num % 16];
        num /= 16;
    }

    // Reverse the string
    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

int printf(const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

  int written = 0;

  while (*format != '\0') {
    size_t maxrem = INT_MAX - written;
    
    if (format[0] != '%' || format[1] == '%') {
      if (format[0] == '%')
	format++;
      size_t amount = 1;
      while (format[amount] && format[amount] != '%')
	amount++;
      if (maxrem < amount) {
	// TODO: Set errno to EOVERFLOW.
	return -1;
      }
      if (!print(format, amount))
	return -1;
      format += amount;
      written += amount;
      continue;
    }
    
    const char* format_begun_at = format++;
    
    if (*format == 'c') {
      format++;
      char c = (char) va_arg(parameters, int /* char promotes to int */);
      if (!maxrem) {
	// TODO: Set errno to EOVERFLOW.
	return -1;
      }
      if (!print(&c, sizeof(c)))
	return -1;
      written++;
    } else if (*format == 's') {
      format++;
      const char* str = va_arg(parameters, const char*);
      size_t len = strlen(str);
      if (maxrem < len) {
	// TODO: Set errno to EOVERFLOW.
	return -1;
      }
      if (!print(str, len))
	return -1;
      written += len;
    } else if (*format == 'd') {
      format++;
      int u = (int)va_arg(parameters, int);
      char buffer[11];
      /* for(int i = 0; i <= u; i++){ */
      /* 	buffer[i] = int_to_char(i); */
      /* } */
      buffer[0] = (char)u;
      size_t len = strlen(buffer);
      if(!print(buffer, len)){
	return -1;
      }
      written += len;
    }else if (*format == 'u'){
      format++;
      unsigned int u = (unsigned int) va_arg(parameters, unsigned int);
      char buffer[11];  // Max length for 32-bit unsigned int
      itoa_dec(u, buffer);
      size_t len = strlen(buffer);
      if (!print(buffer, len)) return -1;
      written += len;
    }else if (*format == 'x') {  // Handle %x for hex
      format++;
      unsigned int x = va_arg(parameters, unsigned int);
      char buffer[9];  // Max length for 32-bit hex (8 digits)
      itoa_hex(x, buffer);
      size_t len = strlen(buffer);
      if (!print(buffer, len)) return -1;
      written += len;
    } else {
      format = format_begun_at;
      size_t len = strlen(format);
      if (maxrem < len) {
	// TODO: Set errno to EOVERFLOW.
	return -1;
      }
      if (!print(format, len))
	return -1;
      written += len;
      format += len;
    }
  }

  va_end(parameters);
  return written;
}
