#include "osh.h"
/* 명령어를 처리해주는 함수 */
void handle_command(char *line, List *zombies) {
  int is_back = strchr(line, '&') ? 1 : 0;
  if (is_back) {
    line = strtok(line, "&"); // 첫 &까지만 입력받은걸로 간주.
  }
  
  int is_pipe = strchr(line, '|') ? 1 : 0;
  int is_in_re = strchr(line, '<') ? 1 : 0;
  int is_out_re = strchr(line, '>') ? 1 : 0;

  if (is_pipe) {
    char *cmds[MAX_LINE / 2 + 1];
    int fd_cnt = parse_command_line(line, cmds, "|") - 1;
    handle_pipe(cmds, fd_cnt, is_back, zombies);
  }
  /*
      "grep .c<input", "grep .c < input", "grep .c< input", "grep .c <input",
      "<input grep .c", "< input cat osh" => 전부 고려해야함.
      공통점 < 와 targetfile인 input은 연속하였음.
      또한 targetfile은 < 기준 파싱 이후 마지막 문자열에 속해있음.
      1. "<" 기준 파싱
          {"grep .c", "input"}, {"grep .c ", " input"}, {"grep .c", " input"}, {"grep .c" , "input"},
          {"input grep .c"}, {" input grep .c"}
      2. 파싱된 char* 배열에서 마지막 문자열을 " " 기준으로 파싱 후 첫 문자열이 target file임.
          {"input"}, {"input"}, {"input"}, {"input"},
          {"input"}, {"input"}
  */
  else if (is_in_re) {
    char *target;
    char *args[MAX_LINE / 2 + 1];
    int len = parse_command_line(line, args, "<");
    if (len > 1) {
      target = strtok(args[1], " \t");
      parse_command_line(args[0], args, " \t");
      handle_input_redirection(args, target, is_back, zombies);
    } else { 
      len = parse_command_line(args[0], args, " \t");
      target = args[0];
      for (int i = 1; i < len; i++)
        args[i - 1] = args[i];
      args[len - 1] = NULL;
      handle_input_redirection(args, target, is_back, zombies);
    }
  }
  /*
      "cat osh.c>output", "cat osh.c > output", "cat osh.c> output", "cat osh.c >output",
      ">output cat osh.c", "> output cat osh" => 전부 고려해야함.
      공통점 > 와 targetfile인 output은 연속하였음.
      또한 targetfile은 > 기준 파싱 이후 마지막 문자열에 속해있음.
      1. ">" 기준 파싱
          {"cat osh.c", "output"}, {"cat osh.c ", " output"}, {"cat osh.c", " output"}, {"cat osh.c" , "output"},
          {"output cat osh.c"}, {" output cat osh.c"}
      2. 파싱된 char* 배열에서 마지막 문자열 " " 기준으로 파싱 후 첫 문자열이 target file임.
          {"output"}, {"output"}, {"output"}, {"output"},
          {"output"}, {"output"}
  */
  else if (is_out_re) {
    char *target;
    char *args[MAX_LINE / 2 + 1];
    int len = parse_command_line(line, args, ">");
    if (len > 1) {
      target = strtok(args[1], " \t");
      parse_command_line(args[0], args, " \t");
      handle_output_redirection(args, target, is_back, zombies);
    } else { 
      len = parse_command_line(args[0], args, " \t");
      target = args[0];
      for (int i = 1; i < len; i++)
        args[i - 1] = args[i];
      args[len - 1] = NULL;
      handle_output_redirection(args, target, is_back, zombies);
    }
  } else {
    pid_t pid = fork();
    if (pid == -1) {
      fprintf(stderr, "Fork error.\n");
      exit(1);
    } else if (pid == 0) {
      char *args[MAX_LINE / 2 + 1];
      parse_command_line(line, args, " \t");
      execvp(args[0], args);
      fprintf(stderr, "osh: %s: command not found\n", args[0]);
      exit(127);
    } else {
      if (is_back) {
        char *cmd = (char *)malloc(sizeof(char) * (MAX_LINE * 5 / 3 - 1));
        strcpy(cmd, line);
        int index = (zombies->tail) ? zombies->tail->index + 1 : 1;
        printf("[%d] %d\n", index, pid);
        Node *appended = newNode(cmd, pid, index);
        appendNode(zombies, appended);
      } else waitpid(pid, NULL, 0);
    }
  }
}

/* 입력 리다이렉션을 처리해주는 함수 */
void handle_input_redirection(char *args[MAX_LINE / 2 + 1], char *target, int is_back, List *zombies) {
  int fd = open(target, O_RDONLY);
  pid_t pid = fork();
  if (pid == -1) {
    fprintf(stderr, "Fork error.\n");
    exit(1);
  } else if (pid == 0) {
    if (fd == -1) {
      fprintf(stderr, "osh: %s: No such file or directory\n", target);
      exit(1);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
    execvp(args[0], args);
    fprintf(stderr, "osh: %s: command not found\n", args[0]);
    exit(127);
  } else {
    if (is_back) {
      char *cmd = (char *)malloc(sizeof(char) * (MAX_LINE * 5 / 3 - 1));
      for (int i = 0; args[i] != NULL; i++) {
        strcat(cmd, args[i]);
        strcat(cmd, " ");
      }
      strcat(cmd, "< ");
      strcat(cmd, target);
      int index = (zombies->tail) ? zombies->tail->index + 1 : 1;      
      printf("[%d] %d\n", index, pid);
      Node *appended = newNode(cmd, pid, index);
      appendNode(zombies, appended);
    } else waitpid(pid, NULL, 0);
  }
}

/* 출력 리다이렉션을 처리해주는 함수 */
void handle_output_redirection(char *args[MAX_LINE / 2 + 1], char *target, int is_back, List *zombies) {
  int fd = open(target, O_RDWR | O_CREAT | O_TRUNC, 0644);

  pid_t pid = fork();

  if (pid == -1) {
    fprintf(stderr, "Fork error.\n");
    exit(1);
  } else if (pid == 0) {
    dup2(fd, STDOUT_FILENO);
    close(fd);
    execvp(args[0], args);
    fprintf(stderr, "osh: %s: command not found\n", args[0]);
    exit(127);
  } else {
    if (is_back) {
      char *cmd = (char *)malloc(sizeof(char) * (MAX_LINE * 5 / 3 - 1));
      for (int i = 0; args[i] != NULL; i++) {
        strcat(cmd, args[i]);
        strcat(cmd, " ");
      }
      strcat(cmd, "> ");
      strcat(cmd, target);
      int index = (zombies->tail) ? zombies->tail->index + 1 : 1;
      printf("[%d] %d\n", index, pid);
      Node *appended = newNode(cmd, pid, index);
      appendNode(zombies, appended);
    } else waitpid(pid, NULL, 0);
  }
}

/* 다중 파이프을 처리해주는 함수 */
void handle_pipe(char *cmds[MAX_LINE / 2 + 1], int fd_cnt, int is_back, List *zombies) {
  int fd[fd_cnt][2];

  for (int i = 0; i <= fd_cnt; i++) {
    if (i < fd_cnt) {
      if (pipe(fd[i]) == -1) {
        fprintf(stderr, "Fork failed...\n");
        exit(1);
      }
    }
    pid_t pid = fork();
    if (pid == -1) {
      fprintf(stderr, "Fork failed...\n");
      exit(1);
    } else if (pid == 0) {
      if (i > 0) {
        dup2(fd[i - 1][0], STDIN_FILENO);
        close(fd[i - 1][1]);
        close(fd[i - 1][0]);
      }
      if (i < fd_cnt) {
        dup2(fd[i][1], STDOUT_FILENO);
        close(fd[i][0]);
        close(fd[i][1]);
      }
      char *args[MAX_LINE / 2 + 1];
      parse_command_line(cmds[i], args, " \t");
      execvp(args[0], args);
      fprintf(stderr, "osh: %s: command not found\n", args[0]);
      exit(127);
    } else {
      if (i > 0) {
        close(fd[i - 1][1]);
        close(fd[i - 1][0]);
      }
      if (i < fd_cnt) {
        waitpid(pid, NULL, 0);
      } else {
        if (is_back) {
          char *cmd = (char *)malloc(sizeof(char) * (MAX_LINE * 5 / 3 - 1));
          for (int i = 0; i <= fd_cnt; i++) {
            char *head, *tail;
            head = cmds[i];
            while (*head != '\0') {
              if (*head == ' ' || *head == '\t') head++;
              else {
                cmds[i] = head;
                break;
              }
            }
            char t[MAX_LINE];
            strcpy(t, cmds[i]);
            tail = t + strlen(cmds[i]) - 1;
            while (tail != t) {
              if (*tail == ' ' || *tail == '\t') {
                tail--;
              } else break;
            }
            *(tail+1) = '\0';
            cmds[i] = t;
            printf("%s\n", cmds[i]); 
            strcat(cmd, cmds[i]);
            if (i < fd_cnt) strcat(cmd, " | ");
          }
          int index = (zombies->tail) ? zombies->tail->index + 1 : 1;
          printf("[%d] %d\n", index, pid);
          Node *appended = newNode(cmd, pid, index);  
          appendNode(zombies, appended);
        } else waitpid(pid, NULL, 0);
      }
    }
  }
}