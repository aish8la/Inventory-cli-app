#include "utilities.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*This function will read the input into a buffer, replace \n with \0 if present
and clear the input buffer of chars if buffer overflows and if input buffer has
any leftover due to it*/
void read_input(char *str, int size)
{

  fgets(str, size, stdin);

  char *str_ptr = strchr(str, '\n');

  if (str_ptr != NULL)
  {
    *str_ptr = '\0';
  }
  else
  {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
      ;
  }
}

// A macro to set the clear_console function based on operating system
#include <stdlib.h>

void clear_console(void)
{
#ifdef _WIN32
  system("cls"); // Windows
#else
  system("clear"); // Unix/Linux/Mac
#endif
}

/*A customer function to wait for input even if input buffer has unconsumed
input this will consume input with getchar.*/
void wait_for_enter(void)
{
  int ch;

  printf("\nPress Enter to continue...");
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

void clear_input_buffer(void)
{
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

int get_user_confirmation(const char *prompt_msg, const char *cancel_msg)
{

  char answer;

  printf("%s [Y]es / [N]o => ", prompt_msg);
  scanf("%c", &answer);
  clear_input_buffer();

  if (tolower(answer) != 'y')
  {
    printf("%s", cancel_msg);
    return 1;
  }

  return 0;
}