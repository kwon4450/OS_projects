#include "osh.h"

Node *newNode(char *cmd, pid_t zpid, int index) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->cmd = cmd;
  node->zpid = zpid;
  node->index = index;
  node->next = NULL;
  return node;
}

List *newList() {
  List *list = (List *)malloc(sizeof(List));
  list->head = NULL;
  list->tail = NULL;
  return list; 
}

void appendNode(List * list, Node *node) {
  if (list->head == NULL) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }
}

void deleteNode(List *list, int index) {
  Node *now = list->head;
  Node *prev = NULL;
  while (now->index != index) {
    prev = now;
    now = now->next;
  }
  if (prev == NULL) {  /* 삭제해야할 부분이 head인 경우 */
    if (now == list->tail) { /* 리스트에 한개의 노드만 들어있을 경우 */
      list->head = NULL;
      list->tail = NULL;
    } else list->head = now->next;
  } else if (now->next == NULL) {  /* 삭제해야할 부분이 tail인 경우 */
    list->tail = prev;
    prev->next = NULL;
  } else {
    prev->next = now->next;
  }
  free(now->cmd);
  free(now);
}

void handle_zombies(List *zombies) {
	Node *now = zombies->head;
  if (now) {
    while (now != NULL) {
      int status;
      pid_t end = waitpid(now->zpid, &status, WNOHANG);
      if (end) {
        if (status) {
          printf("[%d]  Exit %d\t\t\t%s\n", now->index, status/256, now->cmd);
        }
        else {
          printf("[%d]  Done\t\t\t%s\n", now->index, now->cmd);
        }
        deleteNode(zombies, now->index);
      }
      now = now->next;
    }
  }
}