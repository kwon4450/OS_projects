#include "osh.h"
/* 사용자 입력을 line에 저장하고 입력을 체크하여 flag를 리턴해주는 함수 {0: 종료, 1: 정상 입력, -1: 입력 예외처리} */
int command_line(char *line) {
  printf("osh> ");
  fflush(stdout);
  char c;
  int in_re_cnt = 0, out_re_cnt = 0, pipe_cnt = 0, i;
  for (i = 0;; i++) {
    c = getchar();
    /* 입력 명령어의 길이가 최댜 길이보다 길 경우 */
    if (i > MAX_LINE) return -1;
    if (c == '\n') {
      line[i] = '\0';
      break;
    } else if (c == '<') {
      in_re_cnt++;
    } else if (c == '>') {
      out_re_cnt++;
    } else if (c == '|') {
      pipe_cnt++;
    } else if (c == '&') {
      line[i] = '&';
    } else if (c == EOF) return 0; /* ctrl + d 입력 시 종료 */
    line[i] = c;
  }

  /* 명령어가 입출력 리다이렉션 두개 이상 들어온 경우 예외처리. cat < test > test2 */
  if (in_re_cnt + out_re_cnt > 1)
    return -1;
  /* 명령어가 파이프와 입출력 리다이렉션과 같이 들어온 경우 예외처리. */
  if (pipe_cnt) {
    if (in_re_cnt) return -1;
    if (out_re_cnt) return -1;
  }

  if (strcmp(line, "exit")) return 1;
  else return 0;
}