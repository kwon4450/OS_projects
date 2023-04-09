#ifndef __OSH_H__
#define __OSH_H__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */
#define MAX_TOKEN_LEN 10

typedef enum {
  WORD,
  IN_RE,
  OUT_RE,
  APPEND_RE,
  PIPE,
  BACKGROUND,
  EOL
} TokenType;

typedef struct Node {
	struct Node *next;
	char *cmd;
  pid_t zpid;
  int index;
} Node;

typedef struct {
  Node *head;
  Node *tail;
} List;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

/* zombie.c */
Node *newNode(char *cmd, pid_t zpid, int index);
List *newList(void);
void appendNode(List * list, Node *node);
void deleteNode(List *list, int index);
void handle_zombies(List *zombies);

/* input.c */
int command_line(char *line);

/* parser.c */
int parse_command_line(char *line, char *args[MAX_LINE / 2 + 1], char *delim);

/* command.c */
void handle_command(char *line, List *zombies);
void handle_pipe(char *cmds[MAX_LINE / 2 + 1], int fd_cnt, int is_back, List *zombies);
void handle_input_redirection(char *args[MAX_LINE / 2 + 1], char *target, int is_back, List *zombies);
void handle_output_redirection(char *args[MAX_LINE / 2 + 1], char *target, int is_back, List *zombies);

#endif