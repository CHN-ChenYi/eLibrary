﻿#ifndef GUI_H_
#define GUI_H_

#include "basictype.h"
#include "graphics.h"
#include "list.h"

typedef enum Page {
  kWelcome,         // 欢迎界面
  kLendAndBorrow,   // 借还书
  kBookSearch,      // 图书搜索
  kUserSearch,      // 用户搜索（管理员）
  kManual,          // 帮助
  kAbout,           // 关于
  kUserRegister,    // 用户注册
  kUserLogIn,       // 用户登陆
  kLogout,          // 用户登出
  kUserModify,      // 用户信息修改
  kUserManagement,  // 用户删除/审核（管理员）
  kLibrary,         // 图书库显示
  kInitLibrary,     // 图书库新建
  kOpenLibrary,     // 图书库打开
  kSaveLibrary,     // 图书库保存
  kBookDisplay,     // 图书显示
  kBookInit,        // 图书新增
  kBookModify,      // 图书修改/删除
  kBorrowDisplay,   // 借还书统计（管理员）
  kStatistics,      // 统计
  kReturn,          // 回到上一个界面
  kExit             // 关闭程序
} Page;

/*
 * 界面绘制函数
 * cur_page: 当前界面（当前的用户
 * cur_user: 当前登陆的用户（若为NULL，则未登录）
 *     info: 一个结构体指针，其类型由cur_page决定
 * terminal: 终端输出
 */
void DrawUI(Page cur_page, User *cur_user, void *info, char *terminal);

/*
 * 顶部菜单点击
 * nav_page: 导航到哪个界面
 * cur_user: 当前登陆的界面
 * terminal: 终端输出
 */
extern void NavigationCallback(Page nav_page);

/* 借书还书界面 */
#define kLendAndBorrowMax 10  // 单页最大显示数目
typedef struct LendAndBorrow {
  List *books;                             // 待还书列表
  ListNode *books_start;                   // 第一本要显示的待还书
  List *borrow_records;                    // 记录还书日期
  ListNode *borrow_records_start;          // 第一个要显示的还书日期
  void (*search_callback)(char *keyword);  // 搜索按钮
  void (*return_callback)(ListNode *book, ListNode *borrow_record);  // 还书按钮
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} LendAndBorrow;

/* 图书搜索界面 */
#define kBookSearchMax 10  // 单页最大显示数目
typedef struct BookSearch {
  char *keyword;                           // 搜索关键词
  List *book_result;                       // 结果链表
  ListNode *book_result_start;             // 第一个要显示的结果
  void (*search_callback)(char *keyword);  // 搜索按钮
  void (*borrow_callback)(Book *book);     // 借书按钮
  void (*book_callback)(Book *book);       // 图书详细信息按钮
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} BookSearch;

/* 用户搜索界面 */
#define kUserSearchMax 10  // 单页最大显示数目
typedef struct UserSearch {
  char *keyword;                           // 搜索关键词
  List *user_result;                       // 结果链表
  ListNode *user_result_start;             // 第一个要显示的结果
  void (*search_callback)(char *keyword);  // 搜索按钮
  void (*info_callback)(User *user);       // 用户详细信息按钮
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} UserSearch;

/* 用户登陆/注册界面 */
typedef struct LoginOrRegister {
  User *user;                // 当前正在登陆/注册的这个用户
  char password[50];         // 注册/登陆的密码
  char repeat_password[50];  // 重复新密码
  void (*login_callback)();  // 登陆/注册按钮
} LoginOrRegister;

/* 用户信息修改 */
#define kUserModifyMax 10  // 单页最大显示数目
typedef struct UserModify {
  User *user;                     // 用户信息
  int frequency;                  // 最近一个月借书次数
  List *borrowrecords;            // 借书
  ListNode *borrowrecords_start;  // 第一本要显示的借书
  char old_password[50];          // 旧密码
  char new_password[50];          // 新密码
  char repeat_password[50];       // 重复新密码
  void (*confirm_callback)();     // 确认按钮
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} UserModify;

typedef enum SortKeyword {
  kId,      // 用于 Library::sort_callback 和 UserManagement::sort_callback
  kTitle,   // 用于 Library::sort_callback
  kAuthor,  // 用于 Library::sort_callback
  kName,    // 用于 UserManagement::sort_callback
  kDepartment   // 用于 UserManagement::sort_callback
} SortKeyword;  // 关键字列表

/* 审核、修改、删除用户 */
#define kUserManagementToBeVerifiedMax 10  // 单页待审核最大显示数目
#define kUserManagementUsersMax 10         // 单页已添加最大显示数目
typedef struct UserManagement {
  List *to_be_verified;            // 待审核用户列表
  ListNode *to_be_verified_start;  // 第一个显示的待审核用户
  List *users;                     // 已添加用户列表
  ListNode *users_start;           // 第一个显示的已添加用户
  void (*approve_callback)(ListNode *user, bool approve);  // 审核通过或者拒绝
  void (*delete_callback)(ListNode *user);                 // 删除
  void (*info_callback)(User *user);                       // 用户详情
  void (*sort_callback)(SortKeyword sort_keyword);         // 排序按钮
  // direction = 0 上一页, direction = 1 下一页
  // type = 0 待审核用户列表, type = 1 已添加用户列表
  void (*turn_page)(bool direction, bool type);
} UserManagement;

/* 图书库界面 */
#define kLibraryMax 10  // 单页最大显示数目
typedef struct Library {
  enum { kPicture, kList } type;  // 图片模式还是列表模式
  List *books;                    // 图书库的图书
  ListNode *books_start;          // 第一本要显示的图书
  List *book_covers;              // 图书库的书的封面
  ListNode *books_covers_start;   // 第一本要显示的图书
  void (*sort_callback)(SortKeyword sort_keyword);  // 排序按钮
  void (*book_callback)(ListNode *book);            // 图书详细信息按钮
  void (*switch_callback)();                        // 切换模式
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} Library;

/* 图书显示、新建、修改 */
typedef struct BookDisplay {
  Book *book;                  // 当前书籍
  LibImage book_cover;         // 当前书籍封面
  void (*admin_callback)();    // 查看图书借阅次数按钮（管理员）
  void (*cover_callback)();    // 修改图书封面
  void (*confirm_callback)();  // 确认按钮
  void (*delete_callback)();   // 删除
  void (*borrow_callback)();   // 借书按钮
  void (*copy_paste_callback)();  // 复制粘贴按钮
} BookDisplay;

/* 图书借还界面显示 */
#define kBorrowDisplayMax 10  // 单页最大显示数目
typedef struct BorrowDisplay {
  char *book_id;                  // 当前书籍
  int frequency;                  // 最近一个月借书次数
  List *borrow_record;            // 当前书籍的借还记录
  ListNode *borrow_record_start;  // 要显示的第一条借还记录
  // direction = 0 上一页, direction = 1 下一页
  void (*turn_page)(bool direction);
} BorrowDisplay;

/* 统计界面 */
#define kStatisticsCatalogsMax 10      // 单页分类最大显示数目
#define kStatisticsBorrowRecordMax 10  // 单页借还次数最大显示数目
typedef struct Statistics {
  List *catalogs;                 // 图书分类
  ListNode *catalogs_start;       // 第一个要显示的图书分类
  int frequency;                  // 该分类下最近一个月借书次数
  List *borrow_record;            // 借还次数统计
  ListNode *borrow_record_start;  // 第一个要显示的借还记录
  void (*select_callback)(ListNode *catalog);  // 选中某图书分类
  // direction = 0 上一页, direction = 1 下一页
  // type = 0 图书分类, type = 1 借还记录
  void (*turn_page)(bool direction, bool type);
} Statistics;

// 联合体是为了提高代码可读性，用 Union 而不是 struct 是为了节省空间
typedef union State {
  LendAndBorrow *lend_and_borrow;
  BookSearch *book_search;
  UserSearch *user_search;
  LoginOrRegister *login_or_register;
  UserModify *user_modify;
  UserManagement *user_management;
  Library *library;
  BookDisplay *book_display;
  BorrowDisplay *borrow_display;
  Statistics *statistics;
} State;

#endif  // GUI_H_
