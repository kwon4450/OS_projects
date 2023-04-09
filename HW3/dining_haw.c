/*
 * Copyright 2020-2022. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었다.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

/*
 * N = 5, 다섯 명의 철학자가 식사와 낮잠을 즐긴다.
 */ 
#define N 5
/*
 * ANSI 컬러 코드: 출력을 쉽게 구분하기 위해서 사용한다.
 * 순서대로 RED, GRN, YEL, BLU, MAG, RESET을 의미한다.
 */
char *color[N+1] = {"\e[0;31m","\e[0;32m","\e[0;33m","\e[0;34m","\e[0;35m","\e[0m"};
/*
 * alive 값이 0이 될 때까지 철학자는 먹고 자기를 반복한다.
 */
int alive = 1;
pthread_mutex_t chopstick[N];

/*
 * 철학자는 식사를 하는 동안에 문자를 출력한다. <P1EAT>11...1<P1DONE> 이런 식이다.
 * 식사가 끝난 철학자는 잠시 낮잠을 잔다. 잠에서 깨어난 철학자는 식사와 낮잠을 반복한다.
 */
void *philosopher(void *arg)
{
    int id;
    /*
     * 철학자의 ID를 읽는다.
     */
    id = *(int *)arg;
    /*
     * 철학자는 식사와 낮잠을 반복한다.
     */
    while (alive) {
        while (true) {
            pthread_mutex_lock(chopstick+id);
            if (pthread_mutex_trylock(chopstick+(id+1)%N)) {
                pthread_mutex_unlock(chopstick+id);
                continue;
            }
            else break;
        }
        
        /*
         * 배고픈 철학자는 식사를 한다.
         */
        printf("%s<P%dEAT>%s", color[id], id, color[N]);
        for (int i = 0; i < 80; ++i)
            printf("%s%d%s", color[id], id, color[N]);
        printf("%s<P%dDONE>%s\n", color[id], id, color[N]);
        /*
         * 식사를 끝낸 철학자는 1 밀리초 낮잠을 잔다.
         */
        pthread_mutex_unlock(chopstick+id);
        pthread_mutex_unlock(chopstick+(id+1)%N);

        usleep(1000);
    }
    pthread_exit(NULL);
}

/*
 * 메인 함수는 N 개의 philosopher 스레드를 생성한다.
 * 철학자가 먹고 잘 동안 20 밀리초 기다렸다가 스레드를 종료한다.
 */
int main(void)
{
    int i, id[N];
    pthread_t tid[N];
 
    for (int i = 0; i < N; i++)
        pthread_mutex_init(chopstick+i, NULL);
    /*
     * N 개의 철학자 스레드를 생성한다.
     */
    for (i = 0; i < N; ++i) {
        id[i] = i;
        pthread_create(tid+i, NULL, philosopher, id+i);
    }
    /*
     * 철학자가 먹고 잘 동안 20 밀리초 기다린다.
     */
    usleep(20000);
    /*
     * 철학자 스레드를 종료한다.
     */
    alive = 0;
    /*
     * 종료된 철학자 스레드를 기다렸다가 조인한다.
     */
    for (i = 0; i < N; ++i)
        pthread_join(tid[i], NULL);
    for (int i = 0; i < N; i++)
        pthread_mutex_destroy(chopstick+i);

    return 0;
}