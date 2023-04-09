/*
 * Copyright 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

int alive = 1;
/*
 * 흡연자 문제에서 사용하는 네 가지 세마포
 */
pthread_mutex_t tabacco = PTHREAD_MUTEX_INITIALIZER, paper = PTHREAD_MUTEX_INITIALIZER, matches = PTHREAD_MUTEX_INITIALIZER, done = PTHREAD_MUTEX_INITIALIZER;
// sem_t *tabacco, *paper, *matches, *done;

/*
 * 말린 담배잎을 잘게 썰어 놓은 일명 봉초만 가지고 있는 흡연자
 */
void *tabacco_smoker(void *arg)
{
    while (alive) {
      if (!alive) pthread_exit(NULL);
        /*
         * 종이와 성냥을 얻는다.
         */
        pthread_mutex_lock(&tabacco);
        /*
         * 담배를 피운다.
         */
        printf("tabacco       )\n""             (\n"" _ ___________ )\n""[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        pthread_mutex_unlock(&done);
    }
    pthread_exit(NULL);
}

/*
 * 봉초를 말 수 있는 종이만 가지고 있는 흡연자
 */
void *paper_smoker(void *arg)
{
    while (alive) {
      if (!alive) pthread_exit(NULL);
        /*
         * 봉초와 성냥을 얻는다.
         */
        pthread_mutex_lock(&paper);
        /*
         * 담배를 피운다.
         */
        printf("paper         )\n""             (\n"" _ ___________ )\n""[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        pthread_mutex_unlock(&done);
    }
    pthread_exit(NULL);
}

/*
 * 성냥만 가지고 있는 흡연자
 */
void *matches_smoker(void *arg)
{
    while (alive) {
      if (!alive) pthread_exit(NULL);
        /*
         * 봉초와 종이를 얻는다.
         */
        pthread_mutex_lock(&matches);
        /*
         * 담배를 피운다.
         */
        printf("matches       )\n""             (\n"" _ ___________ )\n""[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        pthread_mutex_unlock(&done);
    }
    pthread_exit(NULL);
}

/*
 * 봉초, 종이, 성냥을 무한히 생산할 수 있는 에이전트
 * 임의로 두 가지만 생산해서 나머지를 가지고 있는 흡연자가 담배를 만들어 피울 수 있게 한다.
 * 이 과정을 무한 반복한다.
 */
void *agent(void *arg)
{
    int turn;
    
    srand(202106);
    while (alive) {
        if (!alive) pthread_exit(NULL);
        /*
         * 임의로 두 가지만 생산해서 나머지를 가진 흡연자가 피울 수 있게 한다.
         */
        turn = rand() % 3;
        switch (turn) {
            case 0:
                pthread_mutex_unlock(&tabacco);
                break;
            case 1:
                pthread_mutex_unlock(&paper);
                break;
            case 2:
                pthread_mutex_unlock(&matches);
                break;
            default:
                break;
        }
        pthread_mutex_lock(&done);
    }
    pthread_exit(NULL);
}

/*
 * 메인 함수는 세마포를 초기화하고 세 개의 흡연자 스레드와 에이전트 스레드를 생성한다.
 * 생성된 스레드가 일을 할 동안 10초 동안 기다렸다가 모든 스레드를 철회하고 종료한다.
 */
int main(void)
{
    pthread_t tabacco_id, paper_id, matches_id, agent_id;

    /*
     * 세마포를 초기화 한다. 다만 오류 검사는 생략한다.
     */
    // tabacco = sem_open("tabacco", O_CREAT, 0600, 0);
    // paper = sem_open("paper", O_CREAT, 0600, 0);
    // matches = sem_open("matches", O_CREAT, 0600, 0);
    // done = sem_open("done", O_CREAT, 0600, 0);
    /*
     * 스레드를 생성한다. 다만 오류 검사는 생략한다.
     */
    printf("start\n");
    pthread_create(&tabacco_id, NULL, tabacco_smoker, NULL);
    pthread_create(&paper_id, NULL, paper_smoker, NULL);
    pthread_create(&matches_id, NULL, matches_smoker, NULL);
    pthread_create(&agent_id, NULL, agent, NULL);
    /*
     * 스레드가 실행할 동안 10초 기다린다.
     */
    sleep(1);
    alive = 0;
    pthread_mutex_unlock(&tabacco);
    pthread_mutex_unlock(&paper);
    pthread_mutex_unlock(&matches);
    // pthread_mutex_unlock(&done);
    /*
     * 스레드가 조인될 때까지 기다린다.
     */
    pthread_join(tabacco_id, NULL);
    pthread_join(paper_id, NULL);
    pthread_join(matches_id, NULL);
    pthread_join(agent_id, NULL);
    /*
     * 세마포를 모두 지우고 정리한다.
     */
    pthread_mutex_destroy(&tabacco);
    pthread_mutex_destroy(&paper);
    pthread_mutex_destroy(&matches);
    pthread_mutex_destroy(&done);

    return 0;
}