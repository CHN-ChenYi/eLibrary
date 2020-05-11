﻿#include "view.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gui.h"
#include "list.h"
#include "basictype.h"
#include "exception.h"
#include "model.h"
#include "hash.h"

void DrawUI(Page cur_page, User *cur_user, void *info, char *terminal) {}

typedef struct History {
  Page page;
  State state;
} History;

static List *history_list;
static User user;
static char lib_path[MAX_PATH + 1];
static size_t lib_path_len, username_len;
static DB book_db, user_db, borrowrecord_db;
static FILE *log_file;

inline void Log(char *const msg) {
  time_t cur_time = time(0);
  char *time = asctime(gmtime(&cur_time));
  size_t len = strlen(time);
  while (len && (time[len - 1] == '\r' || time[len - 1] == '\n'))
    time[--len] = '\0';
  // As DST is not always one hour, calculating loacl time zone is expensive
  // So we choose to output UTC time
  fprintf(log_file, "[%s UTC] %s\n", time, msg);
}

void Init() {
  InitConsole();
  InitGraphics();
  history_list = NewList();
  fopen_s(&log_file, ".\\eLibrary.log", "a+");
  Log("[Info] Start");
  DrawUI(kWelcome, &user, NULL, "");
}

static inline History *const HistoryTop() {
  return (History*)history_list->dummy_tail->pre->value;
}

static inline void HistoryPushBack(History *const new_history) {
  InsertList(history_list, history_list->dummy_tail, new_history);
  while (history_list->size > HISTORY_MAX)
    EraseList(history_list, history_list->dummy_head->nxt,
              NULL);  // TODO:(TO/GA) how to free all the states?
}

static inline void ClearHistory() {
  ClearList(history_list, NULL);  // TODO:(TO/GA) how to free all the states?
}

static inline void ReturnHistory(ListNode *go_back_to, const char *const msg) {

}

static void LendAndBorrow_SearchCallback(char *keyword) {}

static void LendAndBorrow_ReturnCallback(ListNode *book) {}

static void BookSearch_SearchCallback(char *keyword) {}

static void BookSearch_BorrowCallback(Book *book) {}

static void UserSearch_SearchCallback(char *keyword) {}

static void UserSearch_InfoCallback(User *user) {}

static void LoginOrRegister_LoginCallback() {
  // TODO:(TO/GA) update username_len
}

static void UserModify_ConfirmCallback() {
  // TODO:(TO/GA) update username_len
  // TODO:(TO/GA) finish it
  // if (user.whoami != ADMINISTRATOR || HistoryTop()->state.user_modify->user->whoami == ADMINISTRATOR) {
  //  uint32_t sha[8];
  //  char *pwd_type = malloc();
  //  sprintf(pwd_type, "%s%s", ) Sha256Sum(sha_db, ) if (memcmp())
  //}
}

static void UserManagement_ApproveCallback(ListNode *user, bool approve) {}

static void UserManagement_DeleteCallback(ListNode *user) {}

static void BookDisplay_AdminCallback() {}

static void BookDisplay_CoverCallback() {
  char image_path[MAX_PATH + 1];
  try {
    SelectFile("JPG image (*.jpg|*.jpeg|*.jpe)\0*.jpg;*.jpeg;*.jpe\0", "jpg",
               FALSE, image_path, MAX_PATH);
    except(ErrorException) {
      char *msg = malloc(sizeof(char) * (34 + username_len));
      sprintf(msg, "[Error] [%s] Fail to open the image", user.username);
      DrawUI(kBookDisplay, &user, HistoryTop()->state.book_display, msg);
      return;
    }
  }
  endtry;
  const int uid = HistoryTop()->state.book_display->book->uid;
  char *command =
      malloc(sizeof(char) * (25 + sizeof(image_path) + lib_path_len + 10));
  sprintf(command, "copy /Y \"%s\" \"%s\\image\\%d.jpg\"", image_path, lib_path,
          uid);

  char *msg = malloc(sizeof(char) * (51 + username_len + 10));
  if (system(command))
    sprintf(msg, "[Error] [%s] Fail to change the book(uid = %d)'s cover", user.username,
            uid);
  else
    sprintf(msg, "[Error] [%s] Change the book(uid = %d)'s cover", user.username, uid);
  Log(msg);
  DrawUI(kBookDisplay, &user, HistoryTop()->state.book_display, msg); // TODO:(TO/GA) 副作用？
  free(command);
}

static void BookDisplay_ConfirmCallback() {}

static void BookDisplay_DeleteCallback() {}

static void BookDisplay_BorrowCallback() {}

static void Library_SortCallback(SortKeyword sort_keyword) {}

// type = 0 => Display
static void BookDisplayOrInit(Book *book, bool type) {
  History *new_history = malloc(sizeof(History));
  if (type) {
    new_history->page = kBookInit;
  } else {
    if (user.whoami == ADMINISTRATOR)
      new_history->page = kBookModify;
    else
      new_history->page = kBookDisplay;
  }
  new_history->state.book_display = malloc(sizeof(BookDisplay));
  new_history->state.book_display->admin_callback = BookDisplay_AdminCallback;
  new_history->state.book_display->cover_callback = BookDisplay_CoverCallback;
  new_history->state.book_display->confirm_callback =
      BookDisplay_ConfirmCallback;
  new_history->state.book_display->delete_callback = BookDisplay_DeleteCallback;
  new_history->state.book_display->borrow_callback = BookDisplay_BorrowCallback;
  new_history->state.book_display->book = book;

  if (!type) {
    char *image_path = malloc(sizeof(char) * (14 + lib_path_len + 10));
    sprintf(image_path, "\"%s\\image\\%d.jpg\"", lib_path, book->uid);
    loadImage(image_path, &new_history->state.book_display->book_cover);
  }
  // TODO:(TO/GA) DrawImage 在没load的时候会挂的

  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (33 + username_len));
  if (type) {
    sprintf(msg, "[Info] [%s] Open book init page", user.username);
  } else {
    if (user.whoami == ADMINISTRATOR)
      sprintf(msg, "[Info] [%s] Open book modify page", user.username);
    else
      sprintf(msg, "[Info] [%s] Open book display page", user.username);
  }
  Log(msg);
  DrawUI(new_history->page, &user, new_history->state.book_display, msg);
}

static void Library_BookCallback(ListNode *book) {
  BookDisplayOrInit(book->value, 0);
}

static void Library_SwitchCallback() {}

static void Statistics_SelectCallback() {}

static inline void Navigation_LendAndBorrow() {
  History *new_history = malloc(sizeof(History));
  new_history->page = kLendAndBorrow;
  new_history->state.lend_and_borrow = malloc(sizeof(LendAndBorrow));
  new_history->state.lend_and_borrow->return_callback = LendAndBorrow_ReturnCallback;
  new_history->state.lend_and_borrow->search_callback = LendAndBorrow_SearchCallback;

  List *borrow_records_list = NewList();
  char *query = malloc(sizeof(char) * (31 + 10));
  sprintf(query, "user_uid=%d&book_status=BORROWED", user.uid);
  Filter(&borrowrecord_db, borrow_records_list, query, USER); // TODO:(TO/GA) error handle
  new_history->state.lend_and_borrow->borrow_records = borrow_records_list;

  List *books = NewList();
  for (ListNode *cur_node = borrow_records_list->dummy_head;
       cur_node != borrow_records_list->dummy_tail; cur_node = cur_node->nxt) {
    Book *book = malloc(sizeof(Book));
    GetById(&book_db, book, ((BorrowRecord*)cur_node->value)->book_uid, BORROWRECORD);
    InsertList(books, books->dummy_tail, book);
  }
  new_history->state.lend_and_borrow->books = books;

  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (34 + username_len));
  sprintf(msg, "[Info] [%s] Open Page LendAndBorrow", user.username);
  Log(msg);
  DrawUI(kLendAndBorrow, &user, new_history->state.lend_and_borrow, msg);
}

static inline void Navigation_BookSearch() {
  BookSearch_SearchCallback(NULL);
}

static inline void Navigation_UserSearch() {
  UserSearch_SearchCallback(NULL);
}

// type = 0 => Manual
static inline void Navigation_ManualOrAbout(bool type) {
  History *new_history = malloc(sizeof(History));
  new_history->state.manual_and_about = malloc(sizeof(ManualAndAbout));
  // TODO:(TO/GA) finish it
  if (type) {
    new_history->page = kAbout;
    new_history->state.manual_and_about->title = "About ...";
    new_history->state.manual_and_about->content = "233 ...";
  } else {
    new_history->page = kManual;
    new_history->state.manual_and_about->title = "Manual ...";
    new_history->state.manual_and_about->content = "233 ...";
  }
  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (27 + username_len));
  if (type)
    sprintf(msg, "[Info] [%s] Open Page About", user.username);
  else
    sprintf(msg, "[Info] [%s] Open Page Manual", user.username);
  Log(msg);
  DrawUI(kAbout, &user, new_history->state.manual_and_about, msg);
}

// type = 0 => LogIn
static inline void Navigation_UserLogInOrRegister(bool type) {
  memset(&user, 0x00, sizeof(user));

  ClearHistory();
  History *new_history = malloc(sizeof(History));
  if (type)
    new_history->page = kUserRegister;
  else
    new_history->page = kUserLogIn;
  new_history->state.login_or_register = malloc(sizeof(LoginOrRegister));
  new_history->state.login_or_register->user = &user;
  new_history->state.login_or_register->login_callback =
      LoginOrRegister_LoginCallback;
  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * 38);
  sprintf(msg, "[Info] Clear history, try to %s", type ? "register" : "log in");
  Log(msg);
  DrawUI(new_history->page, &user, new_history->state.login_or_register, msg);
}

static inline void Navigation_UserLogOut() {
  memset(&user, 0x00, sizeof(user));
  username_len = 0;

  ClearHistory();
  History *new_history = malloc(sizeof(History));
  new_history->page = kWelcome;
  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (36 + username_len));
  sprintf(msg, "[Info] [%s] Clear history and log out", user.username);
  Log(msg);
  DrawUI(kWelcome, &user, new_history->state.login_or_register, msg);
}

static inline void Navigation_UserModify() {
  UserSearch_InfoCallback(&user);
}

static inline void Navigation_UserManagement() {
  if (user.whoami != ADMINISTRATOR) {
    char *msg = malloc(sizeof(char) * (49 + username_len));
    sprintf(msg, "[Error] [%s] Permission denied. Can't manage users",
            user.username);
    Log(msg);
    // 因为指针类型长度固定，所以这里的 HistoryTop()->state.book_display
    // 是随便写的，反正都一样
    DrawUI(HistoryTop()->page, &user, HistoryTop()->state.book_display,
           msg);  // TODO: (TO/GA) 可能因为什么东西没刷新而挂掉
    return;
  }
  History *new_history = malloc(sizeof(History));
  new_history->page = kUserManagement;
  new_history->state.user_management = malloc(sizeof(UserManagement));
  new_history->state.user_management->approve_callback = UserManagement_ApproveCallback;
  new_history->state.user_management->delete_callback = UserManagement_DeleteCallback;
  
  List *to_be_verified = NewList();
  Filter(&user_db, to_be_verified, "verified=FALSE", USER);
  new_history->state.user_management->to_be_verified = to_be_verified;

  List *verified = NewList();
  Filter(&user_db, verified, "verified=TRUE", USER);
  new_history->state.user_management->users = verified;

  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (36 + username_len));
  sprintf(msg, "[Info] [%s] Open Page User Management", user.username);
  Log(msg);
  DrawUI(kUserManagement, &user, new_history->state.user_management, msg);
}

static inline void Navigation_Library() {
  History *new_history = malloc(sizeof(History));
  new_history->page = kLibrary;
  new_history->state.library = malloc(sizeof(Library));
  new_history->state.library->Type = kPicture;
  new_history->state.library->sort_callback = Library_SortCallback;
  new_history->state.library->book_callback = Library_BookCallback;
  new_history->state.library->switch_callback = Library_SwitchCallback;

  List *books = NewList();
  Filter(&book_db, books, "", BOOK);
  new_history->state.library->books = books;

  List *book_covers = NewList();
  const size_t image_path_len = 11 + lib_path_len;
  char *image_path = malloc(sizeof(char) * (image_path_len + 14));
  sprintf(image_path, "\"%s\\image\\", lib_path);
  for (ListNode *cur_node = books->dummy_head->nxt; cur_node != books->dummy_tail; cur_node = cur_node->nxt) {
    LibImage *image = malloc(sizeof(LibImage));
    sprintf(image_path + image_path_len, "%d.jpg", ((Book*)cur_node->value)->uid);
    loadImage(image_path, image);
    InsertList(book_covers, book_covers->dummy_tail, image);
  }
  new_history->state.library->book_covers = book_covers;

  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * (41 + username_len));
  sprintf(msg, "[Info] [%s] Open Page Library (image mode)", user.username);
  Log(msg);
  DrawUI(kLibrary, &user, new_history->state.library, msg);
}

// type == 0 => Open
static inline void Navigation_OpenOrInitLibrary(bool type) {
  if (!user.uid) {
    char *msg = malloc(sizeof(char) * 70);
    if (type)
      sprintf(msg, "[Error] Permission denied. Can't init any library. Please Login first");
    else
      sprintf(msg, "[Error] Permission denied. Can't open any library. Please Login first");
    Log(msg);
    DrawUI(kWelcome, &user, NULL, msg);
    return;
  }

  try {
    SelectFolder("请选择保存图书库的文件夹", lib_path);
    except(ErrorException) {
      char *msg = malloc(sizeof(char) * (56 + username_len));
      if (type)
        sprintf(msg, "[Error] [%s] Fail to init the library. Path doesn't exist", user.username);
      else
        sprintf(msg, "[Error] [%s] Fail to open the library. Path doesn't exist", user.username);
      Log(msg);
      // 因为指针类型长度固定，所以这里的 HistoryTop()->state.book_display
      // 是随便写的，反正都一样
      DrawUI(HistoryTop()->page, &user, HistoryTop()->state.book_display,
             msg);  // TODO: (TO/GA) 有可能因为什么东西没刷新而挂掉
      return;
    }
  }
  endtry;
  lib_path_len = strlen(lib_path);

  CloseDBConnection(&book_db, BOOK);
  CloseDBConnection(&user_db, USER);
  CloseDBConnection(&borrowrecord_db, BORROWRECORD);

  size_t len;
  
  if (type)  {
    len = sizeof(char) * (15 + lib_path_len);
    char *command = malloc(len);
    sprintf(command, "mkdir \"%s\\image\"", lib_path);
    if (system(command)) {
      free(command);
      char *msg = malloc(sizeof(char) * (63 + username_len));
      sprintf(msg, "[Error] [%s] Fail to init the library. Can't create image folder", user.username);
      Log(msg);
      // 因为指针类型长度固定，所以这里的 HistoryTop()->state.book_display
      // 是随便写的，反正都一样
      DrawUI(HistoryTop()->page, &user, HistoryTop()->state.book_display,
             msg);  // TODO: (TO/GA) 有可能因为什么东西没刷新而挂掉
      return;
    }
    free(command);
  }

  len = sizeof(char) * (lib_path_len + strlen("file:\\\\\\book.db") + 2 + 1);
  book_db.filename = malloc(len);
  sprintf(book_db.filename, "\"%s%s%s\"", "file:\\\\", lib_path, "\\book.db");
  OpenDBConnection(&book_db, BOOK);  // TODO:(TO/GA) 异常处理

  user_db.filename = malloc(len);
  sprintf(user_db.filename, "\"%s%s%s\"", "file:\\\\", lib_path, "\\user.db");
  OpenDBConnection(&user_db, USER);

  len += sizeof(char) *
         (strlen("file:\\\\\\borrowrecord.db") - strlen("file:\\\\\\book.db"));
  borrowrecord_db.filename = malloc(len);
  sprintf(borrowrecord_db.filename, "\"%s%s%s\"", "file:\\\\", lib_path,
          "\\borrowrecord.db");
  OpenDBConnection(&borrowrecord_db, BORROWRECORD);

  ClearHistory();
  History *new_history = malloc(sizeof(History));
  new_history->page = kWelcome;
  HistoryPushBack(new_history);

  len = sizeof(char) * (47 + lib_path_len + username_len);
  char *msg = malloc(len);
  if (type)
    sprintf(msg, "[Info] [%s] Clear history and init library from %s",
            user.username, lib_path);
  else
    sprintf(msg, "[Info] [%s] Clear history and open library from %s",
            user.username, lib_path);
  Log(msg);
  DrawUI(kWelcome, &user, NULL, msg);
}

static inline void Navigation_BookInit() {
  Book *book = malloc(sizeof(Book));
  book->uid = GetNextPK(&book_db, BOOK);
  BookDisplayOrInit(book, 1);
}

static bool StrLess(const void *const lhs, const void *rhs) {
  return strcmp(lhs, rhs) <= 0;
}
static bool StrSame(const void *const lhs, const void *rhs) {
  return strcmp(lhs, rhs) == 0;
}
static void StrFree(void *p) {
  free(p);
}

static inline void Navigation_Statistics() {
  History *new_history = malloc(sizeof(History));
  new_history->page = kStatistics;
  new_history->state.statistics = malloc(sizeof(Statistics));
  new_history->state.statistics->select_callback = Statistics_SelectCallback;

  List *book = NewList(), *category = NewList();
  Filter(&book_db, book, "", BOOK);
  for (ListNode *cur_node = book->dummy_head->nxt; cur_node != book->dummy_tail; cur_node = cur_node->nxt) {
    char *str = malloc(sizeof(char) * (strlen(((Book*)cur_node->value)->category) + 1));
    strcpy(str, ((Book *)cur_node->value)->category);
    InsertList(category, category->dummy_tail, str);
  }
  DeleteList(book, NULL);
  SortList(category, StrLess);
  UniqueList(category, StrSame, StrFree);
  new_history->state.statistics->catalogs = category;

  List *borrow_record = NewList();
  Filter(&borrowrecord_db, borrow_record, "", BORROWRECORD);
  new_history->state.statistics->borrow_record = borrow_record;

  HistoryPushBack(new_history);

  char *msg = malloc(sizeof(char) * 31);
  sprintf(msg, "[Info] [%s] Open Page Statistics", user.username);
  Log(msg);
  DrawUI(kStatistics, &user, new_history->state.statistics, msg);
}

static inline void Navigation_Return() {
  char *msg = malloc(sizeof(char) * 18);
  sprintf(msg, "[Info] [%s] Go back", user.username);
  ReturnHistory(history_list->dummy_tail->pre, msg);
}

void NavigationCallback(Page nav_page, User *cur_user) { // TODO:(TO/GA) cur_user 这个参数好像没用？
  switch (nav_page) {
    case kLendAndBorrow:  // 借还书
      Navigation_LendAndBorrow();
      break;
    case kBookSearch:  // 图书搜索
      Navigation_BookSearch();
      break;
    case kUserSearch:  // 用户搜索（管理员）
      Navigation_UserSearch();
      break;
    case kManual:  // 帮助
      Navigation_ManualOrAbout(0);
      break;
    case kAbout:  // 关于
      Navigation_ManualOrAbout(1);
      break;
    case kUserRegister:  // 用户注册
      Navigation_UserLogInOrRegister(1);
      break;
    case kUserLogIn:  // 用户登陆
      Navigation_UserLogInOrRegister(0);
      break;
    case kLogout:  // 用户登出
      Navigation_UserLogOut();
      break;
    case kUserModify:  // 用户信息修改
      Navigation_UserModify();
      break;
    case kUserManagement:  // 用户删除/审核（管理员）
      Navigation_UserManagement();
      break;
    case kLibrary:  // 图书库显示
      Navigation_Library();
      break;
    case kInitLibrary:  // 图书库新建
      Navigation_OpenOrInitLibrary(1);
      break;
    case kOpenLibrary:  // 图书库打开
      Navigation_OpenOrInitLibrary(0);
      break;
    // case kBookDisplay:  // 图书显示
      // Navigation_BookDisplay(nav_page, cur_user);
      // break;
    case kBookInit:  // 图书新增
      Navigation_BookInit(nav_page, cur_user);
      break;
    // case kBookModify:  // 图书修改/删除
      // Navigation_BookModify(nav_page, cur_user);
      // break;
    // case kBorrowDisplay:  // 借还书统计（管理员）
      // Navigation_BorrowDisplay(nav_page, cur_user);
      // break;
    case kStatistics:  // 统计
      Navigation_Statistics();
      break;
    case kReturn:  // 回到上一个界面
      Navigation_Return();
      break;
    default:
      Log("Debug: Unknown nav_page in NavigationCallback");
      Error("Unknown nav_page");
  }
}
