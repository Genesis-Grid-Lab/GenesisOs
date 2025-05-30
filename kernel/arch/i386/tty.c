#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/keyboard.h>
#include <kernel/cursor.h>
#include <kernel/fs.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_init(void) {
  terminal_buffer = VGA_MEMORY;

  terminal_clear();
}

void terminal_clear(void){
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }

  // Reset cursor
  terminal_set_cursor(0);
}

// Scrolls everything up one line
void terminal_scroll(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      
      if (index + 80 <= VGA_WIDTH * VGA_HEIGHT) {
	terminal_buffer[index] = terminal_buffer[index + 80];
      } else {
	terminal_buffer[index] = vga_entry(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
      }
    }
  }

  terminal_row--;
}

void terminal_set_color(uint8_t color) { terminal_color = color; }

void terminal_set_cursor(uint16_t pos) {
  // Save old color
  const uint8_t old_color = terminal_color;

  // Reset color
  terminal_set_color(VGA_COLOR_WHITE);

  fb_move_cursor(pos);

  // Restore color
  terminal_set_color(old_color);
}

void terminal_put_entry_at(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_put_char(char c) {
  unsigned char uc = c;
  if (uc == '\n') {
    terminal_column = 0;
    terminal_row++;

  }
  else if (uc == '\b'){
    terminal_put_entry_at(' ', terminal_color, --terminal_column, terminal_row);

    if (terminal_column == 0) {
      terminal_row--;
      terminal_column = VGA_WIDTH;
    }
  }
  else {
    terminal_put_entry_at(uc, terminal_color, terminal_column, terminal_row);

    if (++terminal_column == VGA_WIDTH) {
      terminal_column = 0;
      terminal_row++;
    }

  }

  if (terminal_row == VGA_HEIGHT) {
    terminal_scroll();
  }

  // Set cursor to character after
  terminal_set_cursor(terminal_row * VGA_WIDTH + terminal_column);

}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_put_char(data[i]);
}

void terminal_write_string(const char* data) {
	terminal_write(data, strlen(data));
}


// little shell

// Read a full command from keyboard
void read_command(char *buffer, int max_length) {
    int index = 0;
    while (1) {
      char key = (char)get_key();
      if (key) {
	if (key == '\n') {
	  buffer[index] = '\0';
	  return;
	} else if (key == '\b' && index > 0) {
	  buffer[--index] = '\0';
	} else if (key >= 32 && index < max_length - 1) {	  
	  buffer[index++] = key;
	}
      }
    }
    buffer[index] = '\0'; // Null-terminate string
}

// Custom string comparison for checking command prefixes
int starts_with(const char* str, const char* prefix) {
    while (*prefix && *str) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return *prefix == '\0';
}

// Custom function to find first space in a string (for argument parsing)
char* find_space(char* str) {
    while (*str) {
        if (*str == ' ') return str;
        str++;
    }
    return NULL;
}

int atoi(char *s)
{    
    int c=1, a=0, sign, start, end, base=1;
//Determine if the number is negative or positive 
    if (s[0] == '-')
        sign = -1;
    else if (s[0] <= '9' && s[0] >= '0')
        sign = 1;
    else if (s[0] == '+')
        sign = 2;
//No further processing if it starts with a letter 
    else 
        return 0;
//Scanning the string to find the position of the last consecutive number
    while (s[c] != '\n' && s[c] <= '9' && s[c] >= '0')
        c++;
//Index of the last consecutive number from beginning
    start = c - 1;
//Based on sign, index of the 1st number is set
    if (sign==-1)       
        end = 1;
    else if (sign==1)
        end = 0;
//When it starts with +, it is actually positive but with a different index 
//for the 1st number
    else
    { 
        end = 1;
        sign = 1;
    }
//This the main loop of algorithm which generates the absolute value of the 
//number from consecutive numerical characters.  
    for (int i = start; i >=end ; i--)
    {
        a += (s[i]-'0') * base;
        base *= 10;
    }
//The correct sign of generated absolute value is applied
    return sign*a;
}

void start_little_shell(void){
  char command[64];

  bool running = true;
  while(running){
    output_prompt();
    read_command(command, 64);

    if (command[0] == '\0') {
      continue;
     }else if (!strcmp(command, "hello")) {
      terminal_write_string("Hello, world!");
    } else if (!strcmp(command, "clear")) {
      terminal_clear();
    } else if (!strcmp(command, "halt")) {
      terminal_write_string("Halting...");
      while (1) { __asm__("hlt"); }  
    } else if (!strcmp(command, "help")){
      show_help();
    }else if (strncmp(command, "ls", 3) == 0) {
      char* dir = strtok(command + 3, " ");
      printf("dir %s\n", dir);
      if(dir){
	fs_ls_node(dir);
      }
      else{
	fs_ls();	
      }
    } else if (strncmp(command, "cd ", 3) == 0) {
      fs_cd(command + 3);
    } else if (strncmp(command, "mkdir ", 6) == 0) {
      fs_create(command + 6, DIR_TYPE);
    } else if (strncmp(command, "touch ", 6) == 0) {
      fs_create(command + 6, FILE_TYPE);
    } else if(strncmp(command, "edit", 6) == 0){
      printf("edit: %s\n", command + 6);
      edit_file(command + 6);
    } else if (strncmp(command, "write ", 6) == 0) {
      char* filename = command + 6;
      char* space = strchr(filename, ' ');
      if (space) {
	*space = '\0';
	fs_write(filename, space + 1);
      }
    } else if (strncmp(command, "read ", 5) == 0) {
      printf("%s\n", fs_read(command + 5));
    } else if (strncmp(command, "./", 2) == 0) {
      char* args[10] = {command + 2, NULL};
      fs_exec(command + 2, 1, args);
    }else if (strncmp(command, "rm ", 3) == 0) {
      fs_delete(command + 3);
    } else if (strncmp(command, "mv ", 3) == 0) {
      char* old_name = strtok(command + 3, " ");
      char* new_name = strtok(NULL, "\0");
      fs_rename(old_name, new_name);
    } else if (strncmp(command, "cp ", 3) == 0) {
      char* src = strtok(command + 3, " ");
      char* dest = strtok(NULL, "\0");
      fs_copy(src, dest);
    } else if (strncmp(command, "chmod ", 6) == 0) {
      char* filename = strtok(command + 6, " ");
      char* perm_str = strtok(NULL, "\0");
      uint8_t perm = atoi(perm_str);
      fs_chmod(filename, perm);
    }else if (strncmp(command, "shutdown",8) == 0){
      running = false;
    }
    else {
      printf("%s\n", command);
    }
   
  }
}

void output_prompt(void)
{
    output_line(">");
}

void output_line(char* line)
{
    printf("\n%s ", line);
}

void show_help(void){
  terminal_write_string("Commands: \n");
  terminal_write_string("Help\n"
                        "hello\n"
			"clear\n"
			"halt\n"
			"touch\n"
			"write\n"
			"read\n"
			"ls\n"
			"mv\n"
			"cp\n"
			"chmod\n"
			"mkdir\n"
			"shutdown\n");
}

void edit_file(const char* filename){
  printf("Editing %s. Type `:wq` to save and exit.\n", filename);
  char buffer[MAX_FILE_SIZE];
  int index = 0;
  uint8_t done = false;
  
  while (!done) {
    char c = (char)get_key();
    if (c == '\n') {
      buffer[index++] = '\n';
    } else if (c == ':') {  // Command mode
      buffer[index] = '\0';      
      if (strcmp(buffer, ":wq") == 0) {
	fs_write(filename, buffer);
	printf("File saved.\n");
	done = true;
	return;
      }
    } else {
      buffer[index++] = c;
    }
  }
}
