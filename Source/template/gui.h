#include "graphics.h"
#include "basictype.h"
#include "list.h"

#ifndef GUI_H_
#define GUI_H_

/*
 * 界面绘制函数
 * cur_page: 当前界面（当前的用户
 * cur_user: 当前登陆的用户（若为NULL，则未登录）
 *     info: 一个结构体指针，其类型由cur_page决定
 * terminal: 终端输出
 */
void DrawUI (Page cur_page, User* cur_user, void* info, char* terminal);

/*
 * 顶部菜单点击
 * nav_page: 导航到哪个界面
 * cur_user: 当前登陆的界面
 * terminal: 终端输出
 */
void NavigationCallback (Page nav_page, User* cur_user, char* terminal);

typedef enum {
  kLendAndBorrow,       // 借还书
  kSearch,              // 搜索
  kManual,              // 帮助
  kAbout,               // 关于
  kUserRegister,        // 用户注册
  kUserLogIn,           // 用户登陆
  kLogout,              // 用户登出
  kUserModify,          // 用户信息修改
  kUserManagement,      // 用户删除/审核（管理员）
  kLibrary,             // 图书库显示
  kInitLibrary,         // 图书库新建
  kOpenLibrary,         // 图书库打开
  kBookDisplay,         // 图书显示
  kBookInit,            // 图书新增
  kBookModify,          // 图书修改/删除
  kBorrowDisplay,       // 借还书统计（管理员）
  kStatistics,          // 统计
  kReturn               // 回到上一个界面
} Page;

/* 借书还书界面 */
struct LendAndBorrow {
  List *book_list;                            // 待还书列表
  void (*search_callback) (char* keyword);    // 搜索按钮
  void (*return_callback) (ListNode* book);   // 还书按钮
};

/* 搜索界面 */
struct Search {
  char *keyword;                              // 搜索关键词
  List *search_result;                        // 结果链表
  void (*search_callback) (char* keyword);    // 搜索按钮
  void (*borrow_callback) (Book* book);       // 借书按钮
};

/* 用户手册/关于界面 */
struct ManualAndAbout {
  enum {kManual, kAbout} Type;    // 用户手册/关于
  char *title;
  char *content;
};

/* 用户登陆/注册界面 */
struct LoginOrRegister {
  enum {kLogin, kRegister} Type;  // 登录还是注册
  User *user;                     // 当前正在登陆/注册的这个用户
  char password[50];              // 注册/登陆的密码
  char repeat_password[50];       // 重复新密码
  void (*login_callback) ();      // 登陆/注册按钮
};

/* 用户信息修改 */
struct UserModify {
  User *user;                   // 用户信息
  List *books;                  // 借书 
  char old_password[50];        // 旧密码
  char new_password[50];        // 新密码
  char repeat_password[50];     // 重复新密码
  void (*confirm_callback) ();  // 确认按钮
  void (*admin_callback) ();    // 管理员修改他人账户按钮
};


/* 审核、修改、删除用户 */
struct UserManagement {
  List *to_be_verified;                                     // 待审核用户列表
  List *users;                                              // 已添加用户列表
  void (*approve_callback) (ListNode* user, bool approve);  // 审核通过或者拒绝
  void (*delete_callback) (ListNode* user);                 // 删除
};

/* 图书库界面 */
struct Library {
  enum {kPicture, kList} Type;              // 图片模式还是列表模式
  List *books;                              // 图书库的图书
  List *book_covers;                        // 图书库的书的封面
  void (*sort_callback) ();                 // 排序按钮
  void (*book_callback) (ListNode* book);   // 图书详细信息按钮
  void (*switch_callback) ();               // 切换模式
};

/* 图书显示、新建、修改 */
struct BookDisplay {
  enum {kDisplay, kModify, kInit} Type;   // 当前状态：显示或修改/新建
  Book *book;                             // 当前书籍
  LibImage book_cover;                    // 当前书籍封面
  void (*admin_callback) ();              // 查看图书借阅次数按钮（管理员）
  void (*cover_callback) ();              // 修改图书封面
  void (*delete_callback) (Book *book);   // 删除
  void (*borrow_callback) (Book *book);   // 借书按钮
};

/* 图书借还界面显示 */
struct BorrowDisplay {
  char *book_name;                    // 当前书籍
  List *borrow_record;                // 当前书籍的借还记录
};

/* 统计界面 */
struct Statistics {
  List *catalogs;                     // 图书分类
  List *borrow_record;                // 借还次数统计
  void (*select_callback) ();         // 选中某图书分类
};

#endif  // GUI_H_