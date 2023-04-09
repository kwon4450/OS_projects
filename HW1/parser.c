#include "osh.h"

/* args에 line을 delim기준으로 파싱하여 tokenization해주는 함수 */
int parse_command_line(char *line, char *args[MAX_LINE / 2 + 1], char *delim)
{
  int i = 0;
  char *token;
  token = strtok(line, delim);
  while (token != NULL) {
    args[i++] = token;
    token = strtok(NULL, delim);
  }
  args[i] = NULL;
  return i;
}