﻿#ifndef UTILITY_H_
#define UTILITY_H_

#include "list.h"

void InitUtility();
void UninitUtility();

void Log(char *const msg);
// direction = 0 => forward
char *MoveInList(ListNode **const node, List *list, int max_size,
                 bool direction, const char *const list_name,
                 const char *const page_name);
// FALSE for success，num 是可变参数的个数，可变参数是可以接受的报错
bool ErrorHandle(int errno_, int num, ...);
// FALSE for success, no_user 为 true 表示可以接受未登录用户
bool InitCheck(bool no_user);

bool CmpGreaterBorrowRecordByReturnTime(const void *const lhs,
                                        const void *const rhs);
bool CmpLessBorrowRecordByReturnTime(const void *const lhs,
                                     const void *const rhs);
bool CmpLessBookById(const void *const lhs, const void *const rhs);
bool CmpLessBookByTitle(const void *const lhs, const void *const rhs);
bool CmpLessBookByAuthor(const void *const lhs, const void *const rhs);
bool CmpLessUserById(const void *const lhs, const void *const rhs);
bool CmpLessUserByName(const void *const lhs, const void *const rhs);
bool CmpLessUserByDepartment(const void *const lhs, const void *const rhs);

void *const StrCpy(void *const str);
bool StrLess(const void *const lhs, const void *rhs);
bool StrSame(const void *const lhs, const void *rhs);

// in format of YYYYMMDD
// 返回的字符串在静态储存区，请不要 free
char *GetTime(time_t dst_tm);
// 传入的链表均为归还时间的降序
int GetBorrowRecordNumberAfter(List *borrow_record, time_t dst_tm);

#endif  // !UTILITY_H_
