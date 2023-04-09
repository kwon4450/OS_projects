#include "osh.h"

int main(void) {
  char line[MAX_LINE]; /* 명령어 입력 */
  int should_run = 1;  /* 프로그램 종료 혹은 입력 예외처리를 위한 flag {0: 종료, 1: 실행, -1: 입력 예외} */
  List* zombies = newList();     /* 좀비 프로세스 정보 리스트 */
  while ((should_run = command_line(line))) {
    if (should_run < 0) {
      printf("Wrong input format...\n");
    }
    else handle_command(line, zombies);
    handle_zombies(zombies);
  }
  return 0;
}