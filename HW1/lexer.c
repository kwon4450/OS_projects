#include "osh.h"

char** tokenizer(char *cmd)
{
    char *argv[MAX_LINE/2+1];   /* 명령어 인자를 저장하기 위한 배열 */
    int argc = 0;               /* 인자의 개수 */
    char *p, *q;                /* 명령어를 파싱하기 위한 변수 */

    /*
     * 명령어 앞부분 공백문자를 제거하고 인자를 하나씩 꺼내서 argv에 차례로 저장한다.
     * 작은 따옴표나 큰 따옴표로 이루어진 문자열을 하나의 인자로 처리한다.
     */
    p = cmd; p += strspn(p, " \t");
    do {
        /*
         * 공백문자, 큰 따옴표, 작은 따옴표가 있는지 검사한다.
         */ 
        q = strpbrk(p, " \t\'\"");
        /*
         * 공백문자가 있거나 아무 것도 없으면 공백문자까지 또는 전체를 하나의 인자로 처리한다.
         */
        if (q == NULL || *q == ' ' || *q == '\t') {
            q = strsep(&p, " \t");
            if (*q) argv[argc++] = q;
        }
        /*
         * 작은 따옴표가 있으면 그 위치까지 하나의 인자로 처리하고, 
         * 작은 따옴표 위치에서 두 번째 작은 따옴표 위치까지 다음 인자로 처리한다.
         * 두 번째 작은 따옴표가 없으면 나머지 전체를 인자로 처리한다.
         */
        else if (*q == '\'') {
            q = strsep(&p, "\'");
            if (*q) argv[argc++] = q;
            q = strsep(&p, "\'");
            if (*q) argv[argc++] = q;
        }
        /*
         * 큰 따옴표가 있으면 그 위치까지 하나의 인자로 처리하고, 
         * 큰 따옴표 위치에서 두 번째 큰 따옴표 위치까지 다음 인자로 처리한다.
         * 두 번째 큰 따옴표가 없으면 나머지 전체를 인자로 처리한다.
         */
        else {
            q = strsep(&p, "\"");
            if (*q) argv[argc++] = q;
            q = strsep(&p, "\"");
            if (*q) argv[argc++] = q;
        }        
    } while (p);
    argv[argc] = NULL;
    for (int i = 0; i < argc; i++)
        printf("argv[%d] = %s, len = %lu\n", i, argv[i], strlen(argv[i]));
    return argv;
}

Token *lexer (char *cmd) {
    char **argv = tokenizer(cmd);
    Token tokens[MAX_LINE/2+1];
    int len = strlen(argv);
    for (int i = 0; i < len; i++) {
        if (strcmp(argv[i], "<") == 0) {
            tokens[i].type = IN_RE;
        } else if (strcmp(argv[i], ">") == 0) {
            tokens[i].type = OUT_RE;
        } else if (strcmp(argv[i], ">>")) {
            tokens[i].type = APPEND_RE;
        } else if (strcmp(argv[i], "|") == 0) {
            tokens[i].type = PIPE;
        } else if (strcmp(argv[i], "&") == 0) {
            tokens[i].type = BACKGROUND;
        } else {
            tokens[i].type = WORD;
        }
        tokens[i].value = argv[i];
    }
}