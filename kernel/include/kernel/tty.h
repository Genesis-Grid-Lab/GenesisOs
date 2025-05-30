#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

void terminal_init(void);
void terminal_clear(void);
void terminal_set_color(uint8_t color);
void terminal_put_char(char c);
void terminal_set_cursor(uint16_t pos);
void terminal_write(const char* data, size_t size);
void terminal_write_string(const char *data);

// Little Shell
void start_little_shell(void);
void output_prompt(void);
void output_line(char *line);
void show_help(void);
void edit_file(const char* filename);
/* static int process_start(char *command); */
/* static char *input_line(void); */
/* static int process_executing(int proc); */
/* static int input_line_waiting(void); */
/* static void process_send_input_line(int proc, char *line); */
/* static int process_output_line_waiting(int proc); */
/* static char* process_get_output_line(int proc); */

#endif
