// TODO:(TO/GA) Add Comment and test again
#include "list.h"

#include <stdlib.h>

#include "exception.h"

static inline void CombineListNode(ListNode *const left,
                                   ListNode *const right) {
  left->nxt = right;
  right->pre = left;
}

List *NewList(const size_t size_of_value) {
  List *list = (List *)malloc(sizeof(List));
  if (!list) Error("Malloc failed in NewList");
  list->size = 0;
  list->size_of_value = size_of_value;
  list->dummy_head = (ListNode *)malloc(sizeof(ListNode));
  if (!list->dummy_head) Error("Malloc failed in NewList");
  list->dummy_tail = (ListNode *)malloc(sizeof(ListNode));
  if (!list->dummy_tail) Error("Malloc failed in NewList");
  CombineListNode(list->dummy_head, list->dummy_tail);
  list->dummy_head->pre = list->dummy_head->value = NULL;
  list->dummy_tail->nxt = list->dummy_tail->value = NULL;
  return list;
}

void DeleteList(const List *const list) {
  ClearList((List *const)list);
  free(list->dummy_head);
  free(list->dummy_tail);
  free((void*)list);
}

void ClearList(List *const list) {
  const ListNode *now = list->dummy_head->nxt;
  while (now != list->dummy_tail) now = EraseList(list, now);
  list->size = 0;
}

const ListNode *InsertList(List *const list, ListNode *const pos,
                           void *const value) {
  if (pos == list->dummy_head) Error("Can't insert a node before dummy head");
  ListNode *new_node = (ListNode *)malloc(sizeof(ListNode));
  if (!new_node) Error("Malloc failed in NewList");
  new_node->value = value;
  new_node->pre = pos->pre;
  new_node->pre->nxt = new_node;
  new_node->nxt = pos;
  new_node->nxt->pre = new_node;
  pos->pre = new_node;
  list->size++;
  return new_node;
}

const ListNode *EraseList(List *const list, const ListNode *const node) {
  if (node == list->dummy_head) Error("Can't erase dummy_head");
  if (node == list->dummy_tail) Error("Can't erase dummy_tail");
  const ListNode *ret = node->pre->nxt = node->nxt;
  node->nxt->pre = node->pre;
  list->size--;
  free(node->value);
  free((void*)node);
  return ret;
}

static inline ListNode *MoveListNode(const List *const list,
                                     const ListNode *const node, int step) {
  ListNode *ret = (ListNode *)node;
  while (ret != list->dummy_tail && step--) ret = ret->nxt;
  return ret;
}

void SortList(const List *const list,
              bool (*cmp)(const void *const lhs, const void *const rhs)) {
  for (int l = 1; l < list->size; l <<= 1) {
    ListNode *now, *left_now, *left_end, *right_now, *right_end;
    now = list->dummy_head;
    do {
      left_now = now->nxt;
      right_now = left_end = MoveListNode(list, left_now, l);
      right_end = MoveListNode(list, right_now, l);
      while (left_now != left_end && right_now != right_end) {
        if (cmp(left_now->value, right_now->value)) {
          CombineListNode(now, left_now);
          now = now->nxt;
          left_now = left_now->nxt;
        } else {
          CombineListNode(now, right_now);
          now = now->nxt;
          right_now = right_now->nxt;
        }
      }
      while (left_now != left_end) {
        CombineListNode(now, left_now);
        now = now->nxt;
        left_now = left_now->nxt;
      }
      while (right_now != right_end) {
        CombineListNode(now, right_now);
        now = now->nxt;
        right_now = right_now->nxt;
      }
      CombineListNode(now, right_now);
    } while (right_now != list->dummy_tail);
  }
}