﻿#include "page.h"

#include "extgraph.h"
#include "graphics.h"
#include "gui.h"
#include "list.h"
#include "ui.h"

typedef enum { kUnclicked, kFile, kBook, kUser, kHelp, kSearch } HeadBarStatus;

HeadBarStatus hb_status;

// Current Page
static Page cur_page;
// Current state
static void* cur_info;
static User* cur_user;
static char cur_terminal[500];

/* HeadBar */

// id for head bar items
#define FILE_ID -1
#define BOOK_ID -2
#define LEND_ID -3
#define USER_ID -4
#define SEARCH_ID -5
#define HELP_ID -6
#define ST_ID -7
#define RTN_ID -8
#define NULL_ID 0  // will not triger any callback

#define MENU_COLOR "BDBDBD"
#define CONFIRM_COLOR "757575"
#define SEARCH_COLOR "1976D2"
#define PANEL_COLOR "E0E0E0"
#define TURN_COLOR "1976D2"

/* Headbar menu */
static Link *HelpButton, *FileButton, *BookButton, *LendAndBorrowButton,
    *UserButton, *SearchButton, *Quit, *Statistic;
static Button* return_button;
static Label* bottom_output;
static InputBox* bottom_info;

/* Background for head bar and foot bar */
static Frame *hb_frame, *fb_frame;

// 数组作用：将callback id和对应的组件结合起来
// 暂存当前页面正在输入/修改的内容，回调的时候将对应数据拷贝
#define MAX_ON_PAGE 20
static Book* books_to_borrow_on_page[MAX_ON_PAGE];
static ListNode* borrow_records_on_page[MAX_ON_PAGE];
static ListNode* books_on_page[MAX_ON_PAGE];
static User* user_on_page[MAX_ON_PAGE];
static ListNode* tbv_user_on_page[MAX_ON_PAGE];
static ListNode* v_user_on_page[MAX_ON_PAGE];
static ListNode* catalog_on_page[MAX_ON_PAGE];
static char* keyword_on_page;
static char* user_name_on_page;
static char* id_on_page;
static char* old_pwd_on_page;
static char* pwd_on_page;
static char* rep_pwd_on_page;
static char* gender_on_page;
static char* dpt_on_page;
static char* whoami_on_page;
static char* book_id_on_page;
static char* book_name_on_page;
static char* book_press_on_page;
static char* book_public_date_on_page;
static char* book_authors_on_page[3];
static char* book_keywords_on_page[5];
static char* catagory_on_page;
static char* days_to_borrow_on_page;
static char* number_of_books_on_page;

/* Submenu */

// submenu for File
static Button *init_lib, *open_lib, *save_lib, *quit;
// submenu for Book
static Button *new_book, *display_book;
// submenu for User
static Button *login, *new_user, *varify, *logout, *modify;
// submenu for Help
static Button *about, *manual;
// submenu for Search
static Button *search_user, *search_book;

/*
 * Callback id:
 * 1 文件新建
 * 2 文件打开
 * 3 文件保存
 * 4 退出
 * 5 图书新建
 * 6 图书显示
 * 7 登录
 * 8 用户新建
 * 9 用户审核
 * 10 用户登出
 * 11 关于
 * 12 用户手册
 * 13 用户搜索
 * 14 图书搜索
 * 15 用户修改
 * 101 用户搜索
 * 102 用户搜索上一页
 * 103 用户搜索下一页
 * 151 - ? 用户详细信息
 * 201 用户注册确认
 * 301 用户修改确认
 * 302 用户修改上一页
 * 303 用户修改下一页
 * 401 登陆
 * 501 按照ID排序
 * 502 按照title排序
 * 503 按照author排序
 * 504 显示上一页书库
 * 505 显示下一页书库
 * 551 - ? 书籍详细信息
 * 601 书籍搜索
 * 602 书籍搜索上一页
 * 603 书籍搜索下一页
 * 651 - ？ 书籍详细信息
 * 701 用户管理-未审核上一页
 * 702 用户管理-未审核下一页
 * 703 用户管理-已存在上一页
 * 704 用户管理-已存在下一页
 * 740 - ? 用户管理-同意（奇数）/拒绝（偶数）第k个申请
 * 770 - ？用户管理-删除第k
 * 801 确认新建/修改
 * 802 新建/修改图片
 * 803 删除图书按钮
 * 804 借书按钮
 * 901 借还书统计界面上一页
 * 902 借还书统计界面下一页
 * 1001 分类统计标签上一页
 * 1002 分类统计标签下一页
 * 1003 分类统计结果上一页
 * 1004 分类统计结果上一页
 * 1050 - ? 分类标签
 * 1101 借书界面上一页
 * 1102 借书界面下一页
 * 1103 借书界面搜索
 * 1150 - ? 还书
 */

// 测试用：生成书的链表
List* GenBook() {
  List* list = NewList();
  for (int i = 0; i < 10; i++) {
    Book* book = malloc(sizeof(Book));
    for (int j = 0; j < 3; j++) sprintf(book->authors[j], "author%d|%d", j, i);
    for (int j = 0; j < 5; j++)
      sprintf(book->keywords[j], "keyword%d|%d", j, i);
    sprintf(book->category, "category%d", i);
    sprintf(book->id, "id%d", i);
    sprintf(book->press, "press%d", i);
    sprintf(book->title, "title%d", i);
    InsertList(list, list->dummy_tail, book);
  }
  return list;
}
// 测试用：生成用户链表
List* GenUser() {
  List* list = NewList();
  for (int i = 0; i < 10; i++) {
    User* user = malloc(sizeof(User));
    sprintf(user->department, "department%d", i);
    user->gender = i % 2 ? MALE : FEMALE;
    user->whoami = i % 2 ? ADMINISTRATOR : NORMAL_USER;
    user->verified = i % 2 ? TRUE : FALSE;
    sprintf(user->id, "id%d", i);
    InsertList(list, list->dummy_tail, user);
  }
  return list;
}
// 测试用：生成借还记录链表
List* GenBorrowRecord() {
  List* list = NewList();
  for (int i = 0; i < 10; i++) {
    BorrowRecord* record = malloc(sizeof(BorrowRecord));
    sprintf(record->book_id, "book%d", i);
    record->book_status = i % 2 ? BORROWED : RETURNED;
    sprintf(record->borrowed_date, "202001%02d", i);
    sprintf(record->returned_date, "202010%02d", i);
    sprintf(record->user_id, "user%d", i);
    InsertList(list, list->dummy_tail, record);
  }
  return list;
}
// 测试用：生成分类链表
List* GenCatalogs() {
  List* list = NewList();
  for (int i = 0; i < 10; i++) {
    char* str = malloc(sizeof(char) * 20);
    sprintf(str, "catalog%d", i);
    InsertList(list, list->dummy_tail, str);
  }
  return list;
}

// 清除表层元件（退出子菜单栏）
void ExitSurface() {
  hb_status = kUnclicked;
  InitSurface();
}

// 绘制子菜单
void AddSubmenuFile() {
  init_lib = CreateButton((Rect){FileButton->position.left,
                                 FileButton->position.left + 150, 70, 110},
                          "新建", MENU_COLOR, 1, kWhite, 1);
  open_lib = CreateButton((Rect){FileButton->position.left,
                                 FileButton->position.left + 150, 110, 150},
                          "打开", MENU_COLOR, 1, kWhite, 2);
  save_lib = CreateButton((Rect){FileButton->position.left,
                                 FileButton->position.left + 150, 150, 190},
                          "保存", MENU_COLOR, 1, kWhite, 3);
  quit = CreateButton((Rect){FileButton->position.left,
                             FileButton->position.left + 150, 190, 230},
                      "退出", MENU_COLOR, 1, kWhite, 4);
  InsertSurface(init_lib, kButton);
  InsertSurface(open_lib, kButton);
  InsertSurface(save_lib, kButton);
  InsertSurface(quit, kButton);
}

// 绘制书籍相关子菜单
void AddSubmenuBook() {
  new_book = CreateButton((Rect){BookButton->position.left,
                                 BookButton->position.left + 150, 70, 110},
                          "新建", MENU_COLOR, 1, kWhite, 5);
  display_book = CreateButton((Rect){BookButton->position.left,
                                     BookButton->position.left + 150, 110, 150},
                              "显示", MENU_COLOR, 1, kWhite, 6);
  InsertSurface(new_book, kButton);
  InsertSurface(display_book, kButton);
}

// 绘制用户相关子菜单
void AddSubmenuUser() {
  login = CreateButton((Rect){UserButton->position.left,
                              UserButton->position.left + 150, 70, 110},
                       "登录", MENU_COLOR, 1, kWhite, 7);
  new_user = CreateButton((Rect){UserButton->position.left,
                                 UserButton->position.left + 150, 110, 150},
                          "新建", MENU_COLOR, 1, kWhite, 8);
  varify = CreateButton((Rect){UserButton->position.left,
                               UserButton->position.left + 150, 150, 190},
                        "审核", MENU_COLOR, 1, kWhite, 9);
  logout = CreateButton((Rect){UserButton->position.left,
                               UserButton->position.left + 150, 190, 230},
                        "登出", MENU_COLOR, 1, kWhite, 10);
  modify = CreateButton((Rect){UserButton->position.left,
                               UserButton->position.left + 150, 230, 270},
                        "修改", MENU_COLOR, 1, kWhite, 15);
  InsertSurface(modify, kButton);
  InsertSurface(login, kButton);
  InsertSurface(new_user, kButton);
  InsertSurface(varify, kButton);
  InsertSurface(logout, kButton);
}

// 绘制帮助相关子菜单
void AddSubmenuHelp() {
  about = CreateButton(
      (Rect){GetWindowWidthPx() - 150, GetWindowWidthPx(), 70, 110}, "关于",
      MENU_COLOR, 1, kWhite, 11);
  manual = CreateButton(
      (Rect){GetWindowWidthPx() - 150, GetWindowWidthPx(), 110, 150},
      "用户手册", MENU_COLOR, 1, kWhite, 12);
  InsertSurface(about, kButton);
  InsertSurface(manual, kButton);
}

// 绘制搜索相关子菜单
void AddSubmenuSearch() {
  Button* user_search = CreateButton((Rect){240, 390, 70, 110}, "用户搜索",
                                     MENU_COLOR, 1, kWhite, 13);
  Button* book_search = CreateButton((Rect){240, 390, 110, 150}, "图书搜索",
                                     MENU_COLOR, 1, kWhite, 14);
  InsertSurface(user_search, kButton);
  InsertSurface(book_search, kButton);
}

// 绘制子菜单
void AddSubmenu(int status) {
  switch (status) {
    case FILE_ID:
      AddSubmenuFile();
      hb_status = kFile;
      break;
    case BOOK_ID:
      AddSubmenuBook();
      hb_status = kBook;
      break;
    case USER_ID:
      AddSubmenuUser();
      hb_status = kUser;
      break;
    case HELP_ID:
      AddSubmenuHelp();
      hb_status = kHelp;
      break;
    case SEARCH_ID:
      AddSubmenuSearch();
      hb_status = kSearch;
      break;
  }
}

LibImage folder_icon_image, book_icon_image, lend_icon_image, user_icon_image,
    search_icon_image, statistics_icon_image, about_image, manual_image,
    welcome_image;

// 绘制顶部菜单
void AddHeadBar() {
  // Head bar
  FileButton = CreateLink((Rect){15, 0, 0, 45}, "文件", kWhite, FILE_ID);
  Image* folder_icon =
      CreateImage((Rect){FileButton->position.right,
                         FileButton->position.right + 25, 25, 50},
                  folder_icon_image, FILE_ID);
  InsertComp(folder_icon, kImage);
  BookButton = CreateLink((Rect){folder_icon->position.right + 10, 0, 0, 45},
                          "图书", kWhite, BOOK_ID);
  Image* book_icon =
      CreateImage((Rect){BookButton->position.right,
                         BookButton->position.right + 25, 25, 50},
                  book_icon_image, BOOK_ID);
  InsertComp(book_icon, kImage);
  LendAndBorrowButton =
      CreateLink((Rect){book_icon->position.right + 10, 0, 0, 45}, "借还",
                 kWhite, LEND_ID);
  Image* lend_icon =
      CreateImage((Rect){LendAndBorrowButton->position.right,
                         LendAndBorrowButton->position.right + 25, 25, 50},
                  lend_icon_image, LEND_ID);
  InsertComp(lend_icon, kImage);
  UserButton = CreateLink((Rect){lend_icon->position.right + 10, 0, 0, 45},
                          "用户", kWhite, USER_ID);
  Image* user_icon =
      CreateImage((Rect){UserButton->position.right,
                         UserButton->position.right + 25, 25, 50},
                  user_icon_image, USER_ID);
  InsertComp(user_icon, kImage);
  Label* user_id = NULL;
  if (cur_user == NULL || cur_user->id[0] == '\0') {
    user_id = CreateLabel((Rect){user_icon->position.right + 10, 0, 0, 45},
                          "未登录", kBlack, NULL_ID);
  } else {
    user_id = CreateLabel((Rect){user_icon->position.right + 10, 0, 0, 45},
                          cur_user->id, kBlack, NULL_ID);
  }
  SearchButton = CreateLink((Rect){user_id->position.right + 10, 0, 0, 45},
                            "搜索", kWhite, SEARCH_ID);
  Image* search_icon =
      CreateImage((Rect){SearchButton->position.right,
                         SearchButton->position.right + 25, 25, 50},
                  search_icon_image, SEARCH_ID);
  InsertComp(search_icon, kImage);
  Statistic = CreateLink((Rect){search_icon->position.right + 10, 0, 0, 45},
                         "统计", kWhite, ST_ID);
  Image* statistics_icon = CreateImage(
      (Rect){Statistic->position.right, Statistic->position.right + 25, 25, 50},
      statistics_icon_image, ST_ID);
  InsertComp(statistics_icon, kImage);
  return_button =
      CreateButton((Rect){statistics_icon->position.right + 10,
                          statistics_icon->position.right + 80, 15, 55},
                   "返回", "757575", 1, kWhite, RTN_ID);
  HelpButton = CreateLink((Rect){GetWindowWidthPx() - 60, 0, 5, 45}, "帮助",
                          kWhite, HELP_ID);
  hb_frame = CreateFrame((Rect){0, GetWindowWidthPx(), 0, 70}, "2979FF", 0.5);
  InsertFrame(hb_frame);
  InsertComp(user_id, kLabel);
  InsertComp(HelpButton, kLink);
  InsertComp(return_button, kButton);
  InsertComp(Statistic, kLink);
  InsertComp(SearchButton, kLink);
  InsertComp(UserButton, kLink);
  InsertComp(LendAndBorrowButton, kLink);
  InsertComp(BookButton, kLink);
  InsertComp(FileButton, kLink);
}

// 绘制底部状态信息栏
void AddFooBar() {
  // foo bar
  bottom_output =
      CreateLabel((Rect){5, GetWindowWidthPx(), GetWindowHeightPx() - 50,
                         GetWindowHeightPx() - 15},
                  "状态栏输出：", kWhite, NULL_ID);
  fb_frame = CreateFrame((Rect){0, GetWindowWidthPx(), GetWindowHeightPx() - 50,
                                GetWindowHeightPx()},
                         "212121", 0.5);
  bottom_info = CreateInputBox(
      (Rect){TextStringWidth("状态栏输出：") + 5, GetWindowWidthPx() - 30, 0,
             GetWindowHeightPx() - 15},
      cur_terminal, NULL_ID, 1);
  InsertFrame(fb_frame);
  InsertComp(bottom_output, kLabel);
  InsertComp(bottom_info, kInputBox);
}

void AddManual() {
  if (cur_page == kManual) {
    Image* img = CreateImage(
        (Rect){0, GetWindowWidthPx(), 100, GetWindowHeightPx() - 100},
        manual_image, 0);
    InsertComp(img, kImage);
  } else if (cur_page == kAbout) {
    Image* img = CreateImage(
        (Rect){0, GetWindowWidthPx(), 100, GetWindowHeightPx() - 100},
        about_image, 0);
    InsertComp(img, kImage);
  }
}

void AddWelcome() {
  Image* img =
      CreateImage((Rect){0, GetWindowWidthPx(), 100, GetWindowHeightPx() - 100},
                  welcome_image, 0);
  InsertComp(img, kImage);
}

// 绘制借还书界面
void AddLendAndBorrow() {
  int left_border = 30;
  int right_border = GetWindowWidthPx() - 30;
  int length_input_box = 120;
  int search_button_top = 90;
  int search_button_bottom = 110;
  Frame* frame =
      CreateFrame((Rect){left_border - 10, right_border + 10,
                         search_button_bottom + 20, GetWindowHeightPx() - 70},
                  PANEL_COLOR, 1);
  InsertFrame(frame);
  Label* borrow_title = CreateLabel(
      (Rect){left_border, 100, search_button_top, search_button_bottom},
      "借书：", kBlack, NULL_ID);
  InputBox* input_box = CreateInputBox(
      (Rect){left_border + TextStringWidth("借书："),
             length_input_box + left_border + TextStringWidth("借书："),
             search_button_top, search_button_bottom},
      "", NULL_ID, 0);
  keyword_on_page = input_box->context;
  Button* search_button = CreateButton(
      (Rect){length_input_box + left_border + TextStringWidth("借书：") + 10,
             length_input_box + left_border + TextStringWidth("借书：") + 80,
             search_button_top - 5, search_button_bottom + 10},
      "搜索", SEARCH_COLOR, 1, kWhite, 1103);
  Label* return_title =
      CreateLabel((Rect){left_border, 0, 0, 150}, "还书：（右侧为到期时间）",
                  kBlack, NULL_ID);
  LendAndBorrow* cur_state = cur_info;
  int count = 1;
  left_border += TextStringWidth("还书");
  int cur_y = 150;
  int delta_y = 50;
  for (ListNode* p = cur_state->books_start;
       p != cur_state->books->dummy_tail && count <= kLendAndBorrowMax;
       p = p->nxt, count++) {
    books_on_page[count] = p;
    Book* cur_book = p->value;
    Label* label = CreateLabel((Rect){left_border, 0, 0, cur_y += delta_y},
                               cur_book->title, kBlack, NULL_ID);
    InsertComp(label, kLabel);
    Link* link = CreateLink((Rect){right_border - 50, 0, 0, cur_y}, "还书",
                            kBlack, 1150 + count);
    InsertComp(link, kLink);
  }
  cur_y = 150;
  count = 1;
  for (ListNode* p = cur_state->borrow_records_start;
       p != cur_state->borrow_records->dummy_tail && count <= kLendAndBorrowMax;
       p = p->nxt, count++) {
    borrow_records_on_page[count] = p;
    BorrowRecord* cur_borrow = p->value;
    Label* label =
        CreateLabel((Rect){left_border + 500, 0, 0, cur_y += delta_y},
                    cur_borrow->returned_date, kBlack, NULL_ID);
    InsertComp(label, kLabel);
  }
  Button* pre_page_button = CreateButton(
      (Rect){20, 100, GetWindowHeightPx() - 125, GetWindowHeightPx() - 70},
      "上一页", TURN_COLOR, 1, kWhite, 1101);
  Button* next_page_button =
      CreateButton((Rect){GetWindowWidthPx() - 100, GetWindowWidthPx() - 20,
                          GetWindowHeightPx() - 125, GetWindowHeightPx() - 70},
                   "下一页", TURN_COLOR, 1, kWhite, 1102);
  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(search_button, kButton);
  InsertComp(input_box, kInputBox);
  InsertComp(return_title, kLabel);
  InsertComp(borrow_title, kLabel);
}

/*
 * 处理借还书界面的点击
 * 1101 借书界面上一页
 * 1102 借书界面下一页
 * 1103 借书界面搜索
 * 1150 - ? 还书
 */
void HandleLendAndBorrowCallback(int id) {
  LendAndBorrow* cur_state = cur_info;
  switch (id) {
    case 1:
      cur_state->turn_page(0);
      break;
    case 2:
      cur_state->turn_page(1);
      break;
    case 3:
      cur_state->search_callback(keyword_on_page);
      break;
    default:
      cur_state->return_callback(books_on_page[id - 50],
                                 borrow_records_on_page[id - 50]);
      break;
  }
}

// 绘制图书搜索界面
void AddBookSearch() {
  BookSearch* book_search = cur_info;

  Label* search_title =
      CreateLabel((Rect){30, 0, 0, 120}, "搜索结果：", kBlack, NULL_ID);
  Label* search_info =
      CreateLabel((Rect){30 + TextStringWidth("搜索结果："), 0, 0, 120},
                  book_search->keyword, kBlack, NULL_ID);
  InputBox* input_box = CreateInputBox((Rect){50, 350, 0, 150}, "", NULL_ID, 0);
  keyword_on_page = input_box->context;
  Button* button = CreateButton((Rect){400, 500, 120, 160}, "搜索",
                                SEARCH_COLOR, 1, kBlack, 601);

  int left_border = 20;
  int right_border = GetWindowWidthPx() - 20;
  int top = 180;
  int bottom = GetWindowHeightPx() - 75;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int cur_y = 160;
  int delta_y = (bottom - top - 50) / 10;
  int delta_x = (right_border - left_border) / 5;
  int count = 1;

  for (ListNode* p = book_search->book_result_start;
       p != book_search->book_result->dummy_tail && count <= 10;
       p = p->nxt, count++) {
    Book* book = p->value;
    books_to_borrow_on_page[count] = book;
    Link* title = CreateLink((Rect){left_border + 10, 0, 0, cur_y += delta_y},
                             book->title, kBlack, 670 + count);
    InsertComp(title, kLink);
    Label* press = CreateLabel((Rect){left_border + delta_x, 0, 0, cur_y},
                               book->press, kBlack, NULL_ID);
    InsertComp(press, kLabel);
    Label* catalog = CreateLabel((Rect){left_border + 2 * delta_x, 0, 0, cur_y},
                                 book->category, kBlack, NULL_ID);
    InsertComp(catalog, kLabel);
    Label* author = CreateLabel((Rect){left_border + 3 * delta_x, 0, 0, cur_y},
                                book->authors[0], kBlack, NULL_ID);
    InsertComp(author, kLabel);
    Link* borrow = CreateLink((Rect){right_border - 100, 0, 0, cur_y}, "借书",
                              kBlack, 650 + count);
    InsertComp(borrow, kLink);
  }

  Button* pre_page_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 602);
  Button* next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 603);
  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(button, kButton);
  InsertComp(search_title, kLabel);
  InsertComp(search_info, kLabel);
  InsertComp(input_box, kInputBox);
}

/*
 * 处理图书搜索界面的回调
 * 601 书籍搜索
 * 602 书籍搜索上一页
 * 603 书籍搜索下一页
 * 651 - ? 借书
 * 671 - ? 图书详细信息
 */
void HandleBookSearchCallback(int id) {
  State cur_state;
  cur_state.book_search = cur_info;
  switch (id) {
    case 1:
      cur_state.book_search->search_callback(keyword_on_page);
      break;
    case 2:
      cur_state.book_search->turn_page(0);
      break;
    case 3:
      cur_state.book_search->turn_page(1);
      break;
    default:
      if (id <= 70) {
        cur_state.book_search->borrow_callback(
            books_to_borrow_on_page[id - 50]);
      } else {
        cur_state.book_search->book_callback(books_to_borrow_on_page[id - 70]);
      }
      break;
  }
}

// 绘制用户搜索界面
void AddUserSearch() {
  UserSearch* user_search = cur_info;

  Label* search_title =
      CreateLabel((Rect){30, 0, 0, 120}, "搜索结果：", kBlack, NULL_ID);
  Label* search_info =
      CreateLabel((Rect){30 + TextStringWidth("搜索结果："), 0, 0, 120},
                  user_search->keyword, kBlack, NULL_ID);
  InputBox* input_box = CreateInputBox((Rect){50, 350, 0, 150}, "", NULL_ID, 0);
  keyword_on_page = input_box->context;
  Button* button = CreateButton((Rect){400, 500, 120, 160}, "搜索",
                                SEARCH_COLOR, 1, kBlack, 101);

  int left_border = 20;
  int right_border = GetWindowWidthPx() - 20;
  int top = 180;
  int bottom = GetWindowHeightPx() - 75;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int cur_y = 160;
  int delta_y = (bottom - top - 50) / 10;
  int count = 1;

  for (ListNode* p = user_search->user_result_start;
       p != user_search->user_result->dummy_tail && count <= kUserSearchMax;
       p = p->nxt, count++) {
    User* user = p->value;
    user_on_page[count] = user;
    Label* name = CreateLabel((Rect){left_border + 10, 0, 0, cur_y += delta_y},
                              user->id, kBlack, NULL_ID);
    Label* department =
        CreateLabel((Rect){name->position.right + 10, 0, 0, cur_y},
                    user->department, kBlack, NULL_ID);
    Label* gender =
        CreateLabel((Rect){department->position.right + 10, 0, 0, cur_y},
                    user->gender == MALE ? "男" : "女", kBlack, NULL_ID);
    Link* more_info = CreateLink(
        (Rect){right_border - TextStringWidth("详细信息") - 10, 0, 0, cur_y},
        "详细信息", kBlack, 150 + count);
    InsertComp(more_info, kLink);
    InsertComp(gender, kLabel);
    InsertComp(department, kLabel);
    InsertComp(name, kLabel);
  }

  Button* pre_page_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 102);
  Button* next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 103);
  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(button, kButton);
  InsertComp(search_title, kLabel);
  InsertComp(search_info, kLabel);
  InsertComp(input_box, kInputBox);
}

/*
 * 处理用户搜索界面的回调
 * 101 用户搜索
 * 102 用户搜索上一页
 * 103 用户搜索下一页
 * 151 - ? 用户详细信息
 */
void HandleUserSearchCallback(int id) {
  State cur_state;
  cur_state.user_search = cur_info;
  switch (id) {
    case 1:
      cur_state.user_search->search_callback(keyword_on_page);
      break;
    case 2:
      cur_state.user_search->turn_page(0);
      break;
    case 3:
      cur_state.user_search->turn_page(1);
      break;
    default:
      cur_state.user_search->info_callback(user_on_page[id - 50]);
      break;
  }
}

// 绘制用户注册界面
void AddUserRegister() {
  int left_border = GetWindowWidthPx() / 2 - 200;
  int right_border = GetWindowWidthPx() / 2 + 200;
  int top = GetWindowHeightPx() / 2 - 250;
  int bottom = GetWindowHeightPx() / 2 + 250;
  int left_x = left_border + 10;

  int cur_y = top;
  int delta_y = (bottom - top) / 10;
  Frame* center_frame = CreateFrame(
      (Rect){left_border, right_border, top, bottom}, PANEL_COLOR, 1);
  InsertFrame(center_frame);
  Label* register_title = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                      "用户注册：", kBlack, NULL_ID);
  Label* id_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                "用户号：", kBlack, NULL_ID);
  InputBox* id_input = CreateInputBox(
      (Rect){id_label->position.right + 10, right_border - 10, 0, cur_y}, "",
      NULL_ID, 0);
  id_on_page = id_input->context;
  Label* name_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                  "姓名：", kBlack, NULL_ID);
  InputBox* name_input = CreateInputBox(
      (Rect){name_label->position.right + 10, right_border - 10, 0, cur_y}, "",
      NULL_ID, 0);
  user_name_on_page = name_input->context;
  Label* first_pw_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                      "密码：", kBlack, NULL_ID);
  InputBox* first_pw_input = CreateInputBox(
      (Rect){first_pw_label->position.right + 10, right_border - 10, 0, cur_y},
      "", NULL_ID, 0);
  pwd_on_page = first_pw_input->context;
  Label* second_pw_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                       "重复密码：", kBlack, NULL_ID);
  InputBox* second_pw_input = CreateInputBox(
      (Rect){second_pw_label->position.right + 10, right_border - 10, 0, cur_y},
      "", NULL_ID, 0);
  rep_pwd_on_page = second_pw_input->context;
  Label* dpt_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                 "部门：", kBlack, NULL_ID);
  InputBox* dpt_input = CreateInputBox(
      (Rect){dpt_label->position.right, right_border - 10, 0, cur_y}, "",
      NULL_ID, 0);
  dpt_on_page = dpt_input->context;
  Label* sex_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                 "性别（M/F）", kBlack, NULL_ID);
  InputBox* sex_input = CreateInputBox(
      (Rect){sex_label->position.right + 10, right_border - 10, 0, cur_y}, "",
      NULL_ID, 0);
  gender_on_page = sex_input->context;
  Label* admin_label =
      CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                  "是否申请管理员账号？(Y/N)", kBlack, NULL_ID);
  InputBox* admin_input = CreateInputBox(
      (Rect){admin_label->position.right + 10, right_border - 10, 0, cur_y}, "",
      NULL_ID, 0);
  whoami_on_page = admin_input->context;
  Button* confirm_button = CreateButton(
      (Rect){left_border + right_border - 80 >> 1,
             left_border + right_border + 80 >> 1, bottom - 50, bottom},
      "确认", CONFIRM_COLOR, 1, kBlack, 201);
  InsertComp(confirm_button, kButton);
  InsertComp(admin_input, kInputBox);
  InsertComp(sex_input, kInputBox);
  InsertComp(dpt_input, kInputBox);
  InsertComp(second_pw_input, kInputBox);
  InsertComp(first_pw_input, kInputBox);
  InsertComp(name_input, kInputBox);
  InsertComp(id_input, kInputBox);
  InsertComp(admin_label, kLabel);
  InsertComp(sex_label, kLabel);
  InsertComp(dpt_label, kLabel);
  InsertComp(second_pw_label, kLabel);
  InsertComp(first_pw_label, kLabel);
  InsertComp(name_label, kLabel);
  InsertComp(id_label, kLabel);
  InsertComp(register_title, kLabel);
}

// 处理用户注册界面回调
void HandleUserRegisterCallback(int id) {
  State cur_state;
  cur_state.login_or_register = cur_info;
  switch (id) {
    case 1:
      strcpy(cur_state.login_or_register->password, pwd_on_page);
      strcpy(cur_state.login_or_register->repeat_password, rep_pwd_on_page);
      strcpy(cur_state.login_or_register->user->id, id_on_page);
      strcpy(cur_state.login_or_register->user->department, dpt_on_page);
      strcpy(cur_state.login_or_register->user->name, user_name_on_page);
      if (strcmp(gender_on_page, "F") == 0) {
        cur_state.login_or_register->user->gender = FEMALE;
      } else {
        // 不合法性别默认为男性
        cur_state.login_or_register->user->gender = MALE;
      }
      if (strcmp(whoami_on_page, "Y") == 0) {
        cur_state.login_or_register->user->whoami = ADMINISTRATOR;
      } else {
        // 不合法默认不申请
        cur_state.login_or_register->user->whoami = NORMAL_USER;
      }
      cur_state.login_or_register->login_callback();
      break;
  }
}

// 绘制用户登录界面
void AddUserLogIn() {
  int pos_x = GetWindowWidthPx() / 2 - 200;
  int pos_y = GetWindowHeightPx() / 2 - 100;
  Frame* center_frame =
      CreateFrame((Rect){pos_x, pos_x + 400, pos_y, pos_y + 250}, "E0E0E0", 1);
  InsertFrame(center_frame);
  Label* register_title = CreateLabel((Rect){pos_x + 5, 0, 0, pos_y + 25},
                                      "用户登陆：", kBlack, NULL_ID);
  Label* id_label = CreateLabel((Rect){pos_x + 15, 0, 0, pos_y + 70},
                                "用户号：", kBlack, NULL_ID);
  InputBox* id_input = CreateInputBox(
      (Rect){pos_x + 150, pos_x + 350, 0, pos_y + 70}, "", NULL_ID, 0);
  id_on_page = id_input->context;
  Label* first_pw_label = CreateLabel((Rect){pos_x + 15, 0, 0, pos_y + 130},
                                      "密码：", kBlack, NULL_ID);
  InputBox* first_pw_input = CreateInputBox(
      (Rect){pos_x + 150, pos_x + 350, 0, pos_y + 130}, "", NULL_ID, 0);
  pwd_on_page = first_pw_input->context;
  Button* confirm_button =
      CreateButton((Rect){pos_x + 100, pos_x + 300, pos_y + 160, pos_y + 220},
                   "登录", CONFIRM_COLOR, 1, kBlack, 401);
  InsertComp(confirm_button, kButton);
  InsertComp(first_pw_input, kInputBox);
  InsertComp(id_input, kInputBox);
  InsertComp(first_pw_label, kLabel);
  InsertComp(id_label, kLabel);
  InsertComp(register_title, kLabel);
}

// 处理用户登录界面回调
void HandleUserLoginCallback(int id) {
  State cur_state;
  cur_state.login_or_register = cur_info;
  switch (id) {
    case 1:
      strcpy(cur_state.login_or_register->password, pwd_on_page);
      strcpy(cur_state.login_or_register->user->id, id_on_page);
      cur_state.login_or_register->login_callback();
      break;
  }
}

// 绘制用户修改界面
void AddUserModify() {
  UserModify* user_modify = cur_info;

  User* user = user_modify->user;

  int left_border = 100;
  int right_border = GetWindowWidthPx() - 100;
  int middle = GetWindowWidthPx() / 2;
  int top = 100;
  int bottom = GetWindowHeightPx() - 70;

  Frame* left_frame = CreateFrame((Rect){left_border, middle - 10, top, bottom},
                                  PANEL_COLOR, 1);
  Frame* right_frame = CreateFrame(
      (Rect){middle + 10, right_border, top, bottom}, PANEL_COLOR, 1);
  InsertFrame(left_frame);
  InsertFrame(right_frame);

  int left_x = left_border + 10;
  int cur_y = top;
  int delta_y = (bottom - top - 100) / 9;
  Label* register_title = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                      "用户修改：", kBlack, NULL_ID);
  Label* id_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                "用户号：", kBlack, NULL_ID);
  InputBox* id_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("用户号："), middle - 20, 0, cur_y},
      user->id, NULL_ID, 0);
  id_on_page = id_input->context;

  Label* name_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                  "姓名：", kBlack, NULL_ID);
  InputBox* name_input = CreateInputBox(
      (Rect){name_label->position.right + 10, middle - 20, 0, cur_y},
      user->name, kBlack, NULL_ID);
  user_name_on_page = name_input->context;

  Label* first_pw_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                      "原密码：", kBlack, NULL_ID);
  InputBox* first_pw_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("原密码："), middle - 20, 0, cur_y}, "",
      NULL_ID, 0);
  old_pwd_on_page = first_pw_input->context;
  Label* second_pw_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                       "新密码：", kBlack, NULL_ID);
  InputBox* second_pw_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("新密码："), middle - 20, 0, cur_y}, "",
      NULL_ID, 0);
  pwd_on_page = second_pw_input->context;
  Label* dpt_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                 "重复新密码：", kBlack, NULL_ID);
  InputBox* dpt_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("重复新密码："), middle - 20, 0, cur_y},
      "", NULL_ID, 0);
  rep_pwd_on_page = dpt_input->context;
  Label* sex_label =
      CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                  "性别（M/F）：（真的要改吗）", kBlack, NULL_ID);
  InputBox* sex_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("性别（M/F）：（真的要改吗）"),
             middle - 20, 0, cur_y},
      user->gender == MALE ? "M" : "F", NULL_ID, 0);
  gender_on_page = sex_input->context;
  Label* admin_label = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                   "部门：", kBlack, NULL_ID);
  InputBox* admin_input = CreateInputBox(
      (Rect){left_x + TextStringWidth("部门："), middle - 20, 0, cur_y},
      user->department, NULL_ID, 0);
  dpt_on_page = admin_input->context;
  Button* confirm_button =
      CreateButton((Rect){left_border, middle - 10, bottom - 50, bottom},
                   "确认", CONFIRM_COLOR, 1, kBlack, 301);

  cur_y = top;
  delta_y = (bottom - top - 100) / (kUserModifyMax + 1);
  int right_x = middle + 20;

  Label* borrow_title = CreateLabel((Rect){right_x, 0, 0, cur_y += delta_y},
                                    "借还书记录", kBlack, NULL_ID);
  InsertComp(borrow_title, kLabel);

  Label* borrow_frequency =
      CreateLabel((Rect){borrow_title->position.right + 10, 0, 0, cur_y},
                  "近一个月借书次数：", kBlack, NULL_ID);

  Label* borrow_times =
      CreateLabel((Rect){borrow_frequency->position.right + 10, 0, 0, cur_y},
                  "", kBlack, NULL_ID);
  sprintf(borrow_times->caption, "%d", user_modify->frequency);

  InsertComp(borrow_frequency, kLabel);
  InsertComp(borrow_times, kLabel);

  int count = 1;
  for (ListNode* p = user_modify->borrowrecords_start;
       p != user_modify->borrowrecords->dummy_tail && count <= kUserModifyMax;
       p = p->nxt, count++) {
    BorrowRecord* borrow_record = p->value;
    Label* user_id = CreateLabel((Rect){right_x, 0, 0, cur_y += delta_y},
                                 borrow_record->user_id, kBlack, NULL_ID);
    Label* info_1 =
        CreateLabel((Rect){user_id->position.right + 10, 0, 0, cur_y}, "在",
                    kBlack, NULL_ID);
    Label* borrow_time =
        CreateLabel((Rect){info_1->position.right + 10, 0, 0, cur_y},
                    borrow_record->borrowed_date, kBlack, NULL_ID);
    Label* info_2 = CreateLabel(
        (Rect){borrow_time->position.right + 10, 0, 0, cur_y},
        borrow_record->book_status == BORROWED ? "借出，应于" : "借出，于",
        kBlack, NULL_ID);
    Label* return_time =
        CreateLabel((Rect){info_2->position.right + 10, 0, 0, cur_y},
                    borrow_record->returned_date, kBlack, NULL_ID);
    Label* info_3 =
        CreateLabel((Rect){return_time->position.right + 10, 0, 0, cur_y},
                    "归还", kBlack, NULL_ID);
    InsertComp(user_id, kLabel);
    InsertComp(info_1, kLabel);
    InsertComp(borrow_time, kLabel);
    InsertComp(info_2, kLabel);
    InsertComp(return_time, kLabel);
    InsertComp(info_3, kLabel);
  }
  Button* pre_page_button =
      CreateButton((Rect){middle + 10, middle + 90, bottom - 50, bottom},
                   "上一页", SEARCH_COLOR, 1, kWhite, 302);
  Button* next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", SEARCH_COLOR, 1, kWhite, 303);

  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(confirm_button, kButton);
  InsertComp(admin_input, kInputBox);
  InsertComp(sex_input, kInputBox);
  InsertComp(dpt_input, kInputBox);
  InsertComp(second_pw_input, kInputBox);
  InsertComp(first_pw_input, kInputBox);
  InsertComp(name_input, kInputBox);
  InsertComp(id_input, kInputBox);
  InsertComp(admin_label, kLabel);
  InsertComp(sex_label, kLabel);
  InsertComp(dpt_label, kLabel);
  InsertComp(second_pw_label, kLabel);
  InsertComp(first_pw_label, kLabel);
  InsertComp(name_label, kLabel);
  InsertComp(id_label, kLabel);
  InsertComp(register_title, kLabel);
}

/*
 * 处理用户修改界面回调
 * 301 用户修改确认
 * 302 上一页
 * 303 下一页
 */
void HandleUserModifyCallback(int id) {
  State cur_state;
  cur_state.user_modify = cur_info;
  switch (id) {
    case 1:
      strcpy(cur_state.user_modify->old_password, old_pwd_on_page);
      strcpy(cur_state.user_modify->new_password, pwd_on_page);
      strcpy(cur_state.user_modify->repeat_password, rep_pwd_on_page);
      strcpy(cur_state.user_modify->user->department, dpt_on_page);
      strcpy(cur_state.user_modify->user->id, id_on_page);
      strcpy(cur_state.user_modify->user->name, user_name_on_page);
      if (strcmp(gender_on_page, "F") == 0) {
        cur_state.user_modify->user->gender = FEMALE;
      } else {
        cur_state.user_modify->user->gender = MALE;
        // 不合法性别默认为男性
      }
      cur_state.user_modify->confirm_callback();
      break;
    case 2:
      cur_state.user_modify->turn_page(0);
      break;
    case 3:
      cur_state.user_modify->turn_page(1);
      break;
  }
}

// 绘制用户管理界面
void AddUserManagement() {
  UserManagement* user_management = cur_info;

  int left_border = 20;
  int right_border = GetWindowWidthPx() - 20;
  int middle = GetWindowWidthPx() / 2;
  int top = 100;
  int bottom = GetWindowHeightPx() - 70;
  int delta_y = (bottom - top - 100) / (kLibraryMax + 1);
  int left_x = left_border + 10, left_cur_y = 100;
  int right_x = middle + 20, right_cur_y = 100;
  Frame* left_frame = CreateFrame((Rect){left_x - 10, middle - 10, top, bottom},
                                  PANEL_COLOR, 1);
  Frame* right_frame = CreateFrame(
      (Rect){middle + 10, right_border, top, bottom}, PANEL_COLOR, 1);
  InsertFrame(left_frame);
  InsertFrame(right_frame);
  Label* to_be_varified_label =
      CreateLabel((Rect){left_x, 0, 0, left_cur_y += delta_y}, "待审核用户：",
                  kBlack, NULL_ID);

  Label* user_list_label =
      CreateLabel((Rect){right_x, 0, 0, right_cur_y += delta_y}, "已存在用户：",
                  kBlack, NULL_ID);

  int count = 1;
  for (ListNode* p = user_management->to_be_verified_start;
       p != user_management->to_be_verified->dummy_tail &&
       count <= kUserManagementUsersMax;
       p = p->nxt, count++) {
    User* user = p->value;
    tbv_user_on_page[count] = p;
    Link* name = CreateLink((Rect){left_x, 0, 0, left_cur_y += delta_y},
                            user->id, kBlack, 710 + count);
    InsertComp(name, kLink);
    Label* dpt =
        CreateLabel((Rect){name->position.right + 10, 0, 0, left_cur_y},
                    user->department, kBlack, NULL_ID);
    InsertComp(dpt, kLabel);
    if (user->whoami == ADMINISTRATOR) {
      Label* whoami =
          CreateLabel((Rect){dpt->position.right + 10, 0, 0, left_cur_y},
                      "申请管理员", kBlack, NULL_ID);
      InsertComp(whoami, kLabel);
    }
    Link* reject = CreateLink(
        (Rect){middle - 10 - TextStringWidth("拒绝") - 10, 0, 0, left_cur_y},
        "拒绝", kBlack, 740 + 2 * count);
    Link* approve =
        CreateLink((Rect){reject->position.left - 10 - TextStringWidth("通过"),
                          0, 0, left_cur_y},
                   "通过", kBlack, 740 + 2 * count - 1);
    InsertComp(approve, kLink);
    InsertComp(reject, kLink);
  }

  count = 1;
  for (ListNode* p = user_management->users_start;
       p != user_management->users->dummy_tail &&
       count <= kUserManagementUsersMax;
       p = p->nxt, count++) {
    User* user = p->value;
    v_user_on_page[count] = p;
    Link* label = CreateLink((Rect){right_x, 0, 0, right_cur_y += delta_y},
                             user->id, kBlack, 720 + count);
    InsertComp(label, kLink);
    Link* delete_button =
        CreateLink((Rect){right_border - 10 - TextStringWidth("删除") - 10, 0,
                          0, right_cur_y},
                   "删除", kBlack, 770 + count);
    InsertComp(delete_button, kLink);
  }

  Button* pre_page_left_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 701);
  Button* next_page_left_button =
      CreateButton((Rect){middle - 90, middle - 10, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 702);
  Button* pre_page_right_button =
      CreateButton((Rect){middle + 10, middle + 90, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 703);
  Button* next_page_right_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 704);
  InsertComp(next_page_right_button, kButton);
  InsertComp(pre_page_right_button, kButton);
  InsertComp(next_page_left_button, kButton);
  InsertComp(pre_page_left_button, kButton);
  InsertComp(to_be_varified_label, kLabel);
  InsertComp(user_list_label, kLabel);
}

/*
 * 处理用户管理界面的回调
 * 701 用户管理 - 未审核上一页
 * 702 用户管理 - 未审核下一页
 * 703 用户管理 - 已存在上一页
 * 704 用户管理 - 已存在下一页
 * 710 - ? 用户详细信息（待审核）
 * 720 - ? 用户详细信息（已存在）
 * 740 - ? 用户管理-同意（奇数）/拒绝（偶数）第k个申请
 * 770 - ？用户管理-删除第k
 */
void HandleUserManagementCallback(int id) {
  State cur_state;
  cur_state.user_management = cur_info;
  switch (id) {
    case 1:
      cur_state.user_management->turn_page(0, 0);
      break;
    case 2:
      cur_state.user_management->turn_page(1, 0);
      break;
    case 3:
      cur_state.user_management->turn_page(0, 1);
      break;
    case 4:
      cur_state.user_management->turn_page(1, 1);
      break;
    default:
      if (id > 70) {
        cur_state.user_management->delete_callback(v_user_on_page[id - 70]);
      } else if (id > 40) {
        cur_state.user_management->approve_callback(
            tbv_user_on_page[(id - 40) + 1 >> 1], id & 1);
      } else if (id > 20) {
        cur_state.user_management->info_callback(
            v_user_on_page[id - 20]->value);
      } else {
        cur_state.user_management->info_callback(
            tbv_user_on_page[id - 10]->value);
      }
  }
}

// 绘制图书显示界面
void AddLibrary() {
  Library* library = cur_info;

  int left_border = 10;
  int right_border = GetWindowWidthPx() - 10;
  int top = 130;
  int bottom = GetWindowHeightPx() - 70;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int cur_y = 110;
  int delta_y = 50;
  int cur_x = 20;  // for the head line
  int width_button = 100;
  Label* title = CreateLabel((Rect){cur_x, 0, 0, cur_y}, "当前图书库图书：",
                             kBlack, NULL_ID);
  Button* sort_by_id =
      CreateButton((Rect){cur_x += TextStringWidth("当前图书库图书："),
                          cur_x + 100, 80, 125},
                   "按书号排序", SEARCH_COLOR, 1, kWhite, 501);
  Button* sort_by_title =
      CreateButton((Rect){cur_x += 110, cur_x + 100, 80, 125}, "按标题排序",
                   SEARCH_COLOR, 1, kWhite, 502);
  Button* sort_by_author =
      CreateButton((Rect){cur_x += 110, cur_x + 100, 80, 125}, "按作者排序",
                   SEARCH_COLOR, 1, kWhite, 503);
  Button* pre_page_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 504);
  Button* next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 505);
  int count = 1;
  cur_y = top;
  delta_y = (bottom - top - 100) / kLibraryMax;
  for (ListNode* p = library->books_start;
       p != library->books->dummy_tail && count <= kLibraryMax;
       p = p->nxt, count++) {
    books_on_page[count] = p;
    Book* book = p->value;
    Label* name_label =
        CreateLabel((Rect){left_border + 10, 0, 0, cur_y += delta_y},
                    book->title, kBlack, NULL_ID);
    Label* id_label =
        CreateLabel((Rect){name_label->position.right + 10, 0, 0, cur_y},
                    book->id, kBlack, NULL_ID);
    Label* author1 =
        CreateLabel((Rect){id_label->position.right + 10, 0, 0, cur_y},
                    book->authors[0], kBlack, NULL_ID);
    Link* more_info = CreateLink(
        (Rect){right_border - TextStringWidth("详细信息") - 10, 0, 0, cur_y},
        "详细信息", kBlack, 550 + count);
    InsertComp(more_info, kLink);
    InsertComp(author1, kLabel);
    InsertComp(id_label, kLabel);
    InsertComp(name_label, kLabel);
  }
  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(sort_by_author, kButton);
  InsertComp(sort_by_title, kButton);
  InsertComp(sort_by_id, kButton);
  InsertComp(title, kLabel);
}

/*
 * 处理图书显示界面回调
 * 501 按照ID排序
 * 502 按照title排序
 * 503 按照author排序
 * 504 显示上一页书库
 * 505 显示下一页书库
 * 551 - ? 书籍详细信息
 */
void HandleLibraryCallback(int id) {
  State cur_state;
  cur_state.library = cur_info;
  switch (id) {
    case 1:
      cur_state.library->sort_callback(kId);
      break;
    case 2:
      cur_state.library->sort_callback(kTitle);
      break;
    case 3:
      cur_state.library->sort_callback(kAuthor);
      break;
    case 4:
      cur_state.library->turn_page(0);
      break;
    case 5:
      cur_state.library->turn_page(1);
      break;
    default:
      cur_state.library->book_callback(books_on_page[id - 50]);
      break;
  }
}

// 绘制图书显示界面
void AddBookDisplay() {
  BookDisplay* book_display = cur_info;

  int left_border = 200;
  int right_border = GetWindowWidthPx() - 200;
  int top = 100;
  int bottom = GetWindowHeightPx() - 120;
  int middle = GetWindowWidthPx() / 2;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int info_x = middle;
  LibImage img = book_display->book_cover;
  int available_width = middle - left_border - 100;
  int availabel_height = bottom - top - 100;
  int height, width;
  if (availabel_height * img.width < img.height * available_width) {
    height = availabel_height;
    width = height * img.width / img.height;
  } else {
    width = available_width;
    height = width * img.height / img.width;
  }
  Image* book_cover = CreateImage(
      (Rect){(left_border + middle - width) / 2,
             (left_border + middle + width) / 2, (top + bottom - height) / 2,
             (top + bottom + height) / 2},
      img, NULL_ID);
  InsertComp(book_cover, kImage);
  int cur_y = top;
  int delta_y = (bottom - top) / 16;
  Label* title = CreateLabel((Rect){left_border, 0, 0, top - 5}, "图书信息",
                             kBlack, NULL_ID);
  Label* book_id_label =
      CreateLabel((Rect){info_x, 0, 0, cur_y += GetFontHeight()}, "书号：",
                  kBlack, NULL_ID);
  Label* book_id_context =
      CreateLabel((Rect){info_x + TextStringWidth("书号："), 0, 0, cur_y},
                  book_display->book->id, kBlack, NULL_ID);
  book_id_on_page = book_id_context->caption;
  Label* book_name_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                       "书名：", kBlack, NULL_ID);
  Label* book_name_context =
      CreateLabel((Rect){info_x + TextStringWidth("书名："), 0, 0, cur_y},
                  book_display->book->title, kBlack, NULL_ID);
  book_name_on_page = book_name_context->caption;
  Label* author_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                    "作者：", kBlack, NULL_ID);
  Label* author_context[3];
  cur_y -= delta_y;
  for (int i = 0; i < 3; i++) {
    author_context[i] = CreateLabel(
        (Rect){info_x + TextStringWidth("作者："), 0, 0, cur_y += delta_y},
        book_display->book->authors[i], kBlack, NULL_ID);
    InsertComp(author_context[i], kLabel);
    book_authors_on_page[i] = author_context[i]->caption;
  }
  Label* press_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                   "出版社：", kBlack, NULL_ID);
  Label* press_context =
      CreateLabel((Rect){info_x + TextStringWidth("出版社："), 0, 0, cur_y},
                  book_display->book->press, kBlack, NULL_ID);
  book_press_on_page = press_context->caption;
  Label* public_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                    "出版日期：", kBlack, NULL_ID);
  Label* public_context =
      CreateLabel((Rect){info_x + TextStringWidth("出版日期："), 0, 0, cur_y},
                  book_display->book->publication_date, kBlack, NULL_ID);
  book_public_date_on_page = public_context->caption;
  Label* keyword_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                     "关键词：", kBlack, NULL_ID);
  Label* keyword_context[5];
  cur_y -= delta_y;
  for (int i = 0; i < 5; i++) {
    keyword_context[i] = CreateLabel(
        (Rect){info_x + TextStringWidth("关键词："), 0, 0, cur_y += delta_y},
        book_display->book->keywords[i], kBlack, NULL_ID);
    InsertComp(keyword_context[i], kLabel);
    book_keywords_on_page[i] = keyword_context[i]->caption;
  }
  Label* catagory_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                      "分类：", kBlack, NULL_ID);
  Label* catagory_context =
      CreateLabel((Rect){info_x + TextStringWidth("分类："), 0, 0, cur_y},
                  book_display->book->category, kBlack, NULL_ID);
  catagory_on_page = catagory_context->caption;
  // Days to borrow
  Book* book = book_display->book;
  Label* days_to_borrow_label = CreateLabel(
      (Rect){info_x, 0, 0, cur_y += delta_y}, "可借阅天数：", kBlack, NULL_ID);
  char borrowed_days[50];
  sprintf(borrowed_days, "%d", book->available_borrowed_days);
  Label* days_to_borrow_info =
      CreateLabel((Rect){info_x + TextStringWidth("可借阅天数："),
                         right_border - 10, 0, cur_y},
                  borrowed_days, kBlack, NULL_ID);
  days_to_borrow_on_page = days_to_borrow_info->caption;

  // Books on shelf
  Label* books_on_shelf_label = CreateLabel(
      (Rect){info_x, 0, 0, cur_y += delta_y}, "在架数：", kBlack, NULL_ID);
  char number_on_the_shelf[50];
  sprintf(number_on_the_shelf, "%d", book->number_on_the_shelf);
  Label* books_on_shelf_info = CreateLabel(
      (Rect){info_x + TextStringWidth("在架数："), right_border - 10, 0, cur_y},
      number_on_the_shelf, kBlack, NULL_ID);
  number_of_books_on_page = books_on_shelf_info->caption;
  Button* borrow_button =
      CreateButton((Rect){left_border, left_border + 80, bottom, bottom + 50},
                   "借书", SEARCH_COLOR, 1, kWhite, 804);
  InsertComp(days_to_borrow_label, kLabel);
  InsertComp(days_to_borrow_info, kLabel);
  InsertComp(books_on_shelf_label, kLabel);
  InsertComp(books_on_shelf_info, kLabel);
  InsertComp(borrow_button, kButton);
  InsertComp(title, kLabel);
  InsertComp(catagory_label, kLabel);
  InsertComp(catagory_context, kLabel);
  InsertComp(book_id_label, kLabel);
  InsertComp(book_id_context, kLabel);
  InsertComp(book_name_label, kLabel);
  InsertComp(book_name_context, kLabel);
  InsertComp(author_label, kLabel);
  InsertComp(public_label, kLabel);
  InsertComp(public_context, kLabel);
  InsertComp(press_label, kLabel);
  InsertComp(press_context, kLabel);
  InsertComp(keyword_label, kLabel);
}

// 绘制图书修改/新建界面
void AddBookModify() {
  BookDisplay* book_modify = cur_info;
  /*BookDisplay* book_modify = malloc(sizeof(BookDisplay));
  book_modify->book = GenBook()->dummy_head->nxt->value;
  loadImage("D:\\Documents\\ZJU\\Study\\2020Spring\\C\\eLibrary\\Resource\\test.jpg",
  &book_modify->book_cover);*/

  int left_border = 200;
  int right_border = GetWindowWidthPx() - 200;
  int top = 100;
  int bottom = GetWindowHeightPx() - 120;
  int middle = GetWindowWidthPx() / 2;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int info_x = middle;
  int cur_y = top;
  int delta_y = (bottom - top) / 17;

  Book* book = book_modify->book;
  LibImage img = book_modify->book_cover;
  int available_width = middle - left_border - 100;
  int availabel_height = bottom - top - 100;
  int height, width;
  if (availabel_height * img.width < img.height * available_width) {
    height = availabel_height;
    width = height * img.width / img.height;
  } else {
    width = available_width;
    height = width * img.height / img.width;
  }
  Image* book_cover = CreateImage(
      (Rect){(left_border + middle - width) / 2,
             (left_border + middle + width) / 2, (top + bottom - height) / 2,
             (top + bottom + height) / 2},
      img, 802);
  InsertComp(book_cover, kImage);

  Label* title;
  if (cur_page == kBookInit) {
    title = CreateLabel((Rect){left_border, 0, 0, top - 10}, "图书新建：",
                        kBlack, NULL_ID);
  } else {
    title = CreateLabel((Rect){left_border, 0, 0, top - 10}, "图书修改：",
                        kBlack, NULL_ID);
  }

  // Id
  Label* book_id_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                     "书号：", kBlack, NULL_ID);
  InputBox* book_id_context = CreateInputBox(
      (Rect){info_x + TextStringWidth("书号："), right_border - 10, 0, cur_y},
      book->id, NULL_ID, 0);
  book_id_on_page = book_id_context->context;

  // Title
  Label* book_name_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                       "书名：", kBlack, NULL_ID);
  InputBox* book_name_context = CreateInputBox(
      (Rect){info_x + TextStringWidth("书名："), right_border - 10, 0, cur_y},
      book->title, NULL_ID, 0);
  book_name_on_page = book_name_context->context;

  // Authors
  Label* author_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                    "作者：（最多三人）", kBlack, NULL_ID);
  InputBox* author_context[3];
  for (int i = 0; i < 3; i++) {
    author_context[i] =
        CreateInputBox((Rect){info_x + TextStringWidth("作者："),
                              right_border - 10, 0, cur_y += delta_y},
                       book->authors[i], NULL_ID, 0);
    book_authors_on_page[i] = author_context[i]->context;
    InsertComp(author_context[i], kInputBox);
  }

  // Press
  Label* press_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                   "出版社：", kBlack, NULL_ID);
  InputBox* press_context = CreateInputBox(
      (Rect){info_x + TextStringWidth("出版社："), right_border - 10, 0, cur_y},
      book->press, NULL_ID, 0);
  book_press_on_page = press_context->context;

  // Public Date
  Label* public_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                    "出版日期：", kBlack, NULL_ID);
  InputBox* public_context =
      CreateInputBox((Rect){info_x + TextStringWidth("出版日期："),
                            right_border - 10, 0, cur_y},
                     book->publication_date, NULL_ID, 0);
  book_public_date_on_page = public_context->context;

  // Keywords
  Label* keyword_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                     "关键词：（最多五个）", kBlack, NULL_ID);
  InputBox* keyword_context[5];
  for (int i = 0; i < 5; i++) {
    keyword_context[i] =
        CreateInputBox((Rect){info_x + TextStringWidth("关键词："),
                              right_border - 10, 100, cur_y += delta_y},
                       book->keywords[i], NULL_ID, 0);
    book_keywords_on_page[i] = keyword_context[i]->context;
    InsertComp(keyword_context[i], kInputBox);
  }

  // Catagory
  Label* catagory_label = CreateLabel((Rect){info_x, 0, 0, cur_y += delta_y},
                                      "分类：", kBlack, NULL_ID);
  InputBox* catagory_input = CreateInputBox(
      (Rect){info_x + TextStringWidth("分类："), right_border - 10, 0, cur_y},
      book->category, NULL_ID, 0);
  catagory_on_page = catagory_input->context;

  // Days to borrow
  Label* days_to_borrow_label = CreateLabel(
      (Rect){info_x, 0, 0, cur_y += delta_y}, "可借阅天数：", kBlack, NULL_ID);
  char borrowed_days[50];
  sprintf(borrowed_days, "%d", book->available_borrowed_days);
  InputBox* days_to_borrow_input =
      CreateInputBox((Rect){info_x + TextStringWidth("可借阅天数："),
                            right_border - 10, 0, cur_y},
                     borrowed_days, NULL_ID, 0);
  days_to_borrow_on_page = days_to_borrow_input->context;

  // Books on shelf
  Label* books_on_shelf_label = CreateLabel(
      (Rect){info_x, 0, 0, cur_y += delta_y}, "在架数：", kBlack, NULL_ID);
  char number_on_the_shelf[50];
  sprintf(number_on_the_shelf, "%d", book->number_on_the_shelf);
  InputBox* books_on_shelf_input = CreateInputBox(
      (Rect){info_x + TextStringWidth("在架数："), right_border - 10, 0, cur_y},
      number_on_the_shelf, NULL_ID, 0);
  number_of_books_on_page = books_on_shelf_input->context;

  Button* confirm_button =
      CreateButton((Rect){left_border, left_border + 80, bottom, bottom + 50},
                   "确认", SEARCH_COLOR, 1, kWhite, 801);
  Button* delete_button = CreateButton(
      (Rect){left_border + 80, left_border + 160, bottom, bottom + 50}, "删除",
      SEARCH_COLOR, 1, kWhite, 803);
  Button* borrow_button = CreateButton(
      (Rect){left_border + 160, left_border + 240, bottom, bottom + 50}, "借书",
      SEARCH_COLOR, 1, kWhite, 804);
  Button* copy_button = CreateButton(
      (Rect){left_border + 240, left_border + 300, bottom, bottom + 50}, "复制",
      SEARCH_COLOR, 1, kWhite, 805);
  Button* admin_button = CreateButton(
      (Rect){left_border + 300, left_border + 400, bottom, bottom + 50},
      "借阅记录", SEARCH_COLOR, 1, kWhite, 806);
  if (cur_page == kBookInit) {
    InsertComp(delete_button, kButton);
    InsertComp(confirm_button, kButton);
  } else {
    InsertComp(delete_button, kButton);
    InsertComp(confirm_button, kButton);
    InsertComp(borrow_button, kButton);
    InsertComp(copy_button, kButton);
    InsertComp(admin_button, kButton);
  }

  InsertComp(books_on_shelf_label, kLabel);
  InsertComp(books_on_shelf_input, kInputBox);
  InsertComp(days_to_borrow_label, kLabel);
  InsertComp(days_to_borrow_input, kInputBox);
  InsertComp(title, kLabel);
  InsertComp(catagory_input, kInputBox);
  InsertComp(catagory_label, kLabel);
  InsertComp(book_id_label, kLabel);
  InsertComp(book_id_context, kInputBox);
  InsertComp(book_name_label, kLabel);
  InsertComp(book_name_context, kInputBox);
  InsertComp(author_label, kLabel);
  InsertComp(public_label, kLabel);
  InsertComp(public_context, kInputBox);
  InsertComp(press_label, kLabel);
  InsertComp(press_context, kInputBox);
  InsertComp(keyword_label, kLabel);
}

/*
 * 处理图书修改/新建界面的回调
 * 801 确认新建 / 修改
 * 802 新建 / 修改图片
 * 803 删除图书按钮
 * 804 借书按钮
 * 805 复制图书信息
 * 806 管理员查看图书详细信息
 */
void HandleBookCallback(int id) {
  BookDisplay* cur_state = cur_info;
  strcpy(cur_state->book->id, book_id_on_page);
  strcpy(cur_state->book->title, book_name_on_page);
  strcpy(cur_state->book->press, book_press_on_page);
  for (int i = 0; i < 3; i++) {
    strcpy(cur_state->book->authors[i], book_authors_on_page[i]);
  }
  for (int i = 0; i < 5; i++) {
    strcpy(cur_state->book->keywords[i], book_keywords_on_page[i]);
  }
  strcpy(cur_state->book->category, catagory_on_page);
  strcpy(cur_state->book->publication_date, book_public_date_on_page);
  cur_state->book->available_borrowed_days = atoi(days_to_borrow_on_page);
  cur_state->book->number_on_the_shelf = atoi(number_of_books_on_page);
  switch (id) {
    case 1:
      cur_state->confirm_callback();
      break;
    case 2:
      cur_state->cover_callback();
      break;
    case 3:
      cur_state->delete_callback();
      break;
    case 4:
      cur_state->borrow_callback();
      break;
    case 5:
      cur_state->copy_paste_callback();
      break;
    case 6:
      cur_state->admin_callback();
      break;
  }
}

// 绘制借还记录显示界面
void AddBorrowDisplay() {
  BorrowDisplay* borrow_display = cur_info;
  /*
  BorrowDisplay *borrow_display = malloc(sizeof(borrow_display));
  borrow_display->book_id = malloc(sizeof(char) * 10);
  memset(borrow_display->book_id, 0, sizeof(borrow_display->book_id));
  strcpy(borrow_display->book_id, "他改变了中国");
  borrow_display->borrow_record = GenBorrowRecord();
  borrow_display->borrow_record_start =
  borrow_display->borrow_record->dummy_head->nxt;
  */

  int left_border = 100;
  int right_border = GetWindowWidthPx() - 100;
  int top = 100;
  int bottom = GetWindowHeightPx() - 100;

  Frame* frame = CreateFrame((Rect){left_border, right_border, top, bottom},
                             PANEL_COLOR, 1);
  InsertFrame(frame);

  int cur_y = top;
  int delta_y = (bottom - top - 100) / (kBorrowDisplayMax + 1);
  Label* title = CreateLabel((Rect){left_border, 0, 0, cur_y += delta_y},
                             "借还书记录：", kBlack, NULL_ID);
  Label* book_title =
      CreateLabel((Rect){title->position.right + 10, 0, 0, cur_y},
                  borrow_display->book_id, kBlack, NULL_ID);
  Label* borrow_frequency =
      CreateLabel((Rect){book_title->position.right + 10, 0, 0, cur_y},
                  "近一个月借书次数：", kBlack, NULL_ID);

  Label* borrow_times =
      CreateLabel((Rect){borrow_frequency->position.right + 10, 0, 0, cur_y},
                  "", kBlack, NULL_ID);
  sprintf(borrow_times->caption, "%d", borrow_display->frequency);

  InsertComp(borrow_frequency, kLabel);
  InsertComp(borrow_times, kLabel);
  int count = 1;
  for (ListNode* p = borrow_display->borrow_record_start;
       p != borrow_display->borrow_record->dummy_tail &&
       count <= kBorrowDisplayMax;
       p = p->nxt, count++) {
    BorrowRecord* borrow_record = p->value;
    Label* user_id =
        CreateLabel((Rect){left_border + 10, 0, 0, cur_y += delta_y},
                    borrow_record->user_id, kBlack, NULL_ID);
    Label* info_1 =
        CreateLabel((Rect){user_id->position.right + 10, 0, 0, cur_y}, "在",
                    kBlack, NULL_ID);
    Label* borrow_time =
        CreateLabel((Rect){info_1->position.right + 10, 0, 0, cur_y},
                    borrow_record->borrowed_date, kBlack, NULL_ID);
    Label* info_2 = CreateLabel(
        (Rect){borrow_time->position.right + 10, 0, 0, cur_y},
        borrow_record->book_status == BORROWED ? "借出，应于" : "借出，于",
        kBlack, NULL_ID);
    Label* return_time =
        CreateLabel((Rect){info_2->position.right + 10, 0, 0, cur_y},
                    borrow_record->returned_date, kBlack, NULL_ID);
    Label* info_3 =
        CreateLabel((Rect){return_time->position.right + 10, 0, 0, cur_y},
                    "归还", kBlack, NULL_ID);
    InsertComp(user_id, kLabel);
    InsertComp(info_1, kLabel);
    InsertComp(borrow_time, kLabel);
    InsertComp(info_2, kLabel);
    InsertComp(return_time, kLabel);
    InsertComp(info_3, kLabel);
  }
  Button* pre_page_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", TURN_COLOR, 1, kWhite, 901);
  Button* next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", TURN_COLOR, 1, kWhite, 902);
  InsertComp(next_page_button, kButton);
  InsertComp(pre_page_button, kButton);
  InsertComp(book_title, kLabel);
  InsertComp(title, kLabel);
}

/*
 * 处理借还记录显示界面的回调函数
 * 901 借还书界面上一页
 * 902 借还书界面下一页
 */
void HandleBorrowDisplayCallback(int id) {
  BorrowDisplay* cur_state = cur_info;
  switch (id) {
    case 1:
      cur_state->turn_page(0);
      break;
    case 2:
      cur_state->turn_page(1);
      break;
  }
}

// 绘制统计显示界面
void AddStatistics() {
  Statistics* statistics = cur_info;
  /*Statistics *statistics = malloc(sizeof(Statistics));
  statistics->borrow_record = GenBorrowRecord();
  statistics->borrow_record_start = statistics->borrow_record->dummy_head->nxt;
  statistics->catalogs = GenCatalogs();
  statistics->catalogs_start = statistics->catalogs->dummy_head->nxt;*/

  int left_border = 100;
  int right_border = GetWindowWidthPx() - 100;
  int middle = GetWindowWidthPx() * 0.4;
  int top = 100;
  int bottom = GetWindowHeightPx() - 70;

  Frame* frame_left = CreateFrame((Rect){left_border, middle - 10, top, bottom},
                                  PANEL_COLOR, 1);
  InsertFrame(frame_left);
  Frame* frame_right = CreateFrame(
      (Rect){middle + 10, right_border, top, bottom}, PANEL_COLOR, 1);
  InsertFrame(frame_right);

  Label* title = CreateLabel((Rect){left_border + 10, 0, 0, top - 10},
                             "分类统计：", kBlack, NULL_ID);
  InsertComp(title, kLabel);

  int cur_y = top;
  int delta_y = (bottom - top - 100) / (kStatisticsCatalogsMax + 1);
  int left_x = left_border + 10;
  int right_x = middle + 20;
  Label* catalog_title = CreateLabel((Rect){left_x, 0, 0, cur_y += delta_y},
                                     "现有分类：", kBlack, NULL_ID);
  InsertComp(catalog_title, kLabel);
  int count = 1;
  for (ListNode* p = statistics->catalogs_start;
       p != statistics->catalogs->dummy_tail && count <= kStatisticsCatalogsMax;
       p = p->nxt, count++) {
    char* catalog = p->value;
    catalog_on_page[count] = p;
    Link* catalog_link = CreateLink((Rect){left_x + 10, 0, 0, cur_y += delta_y},
                                    catalog, kBlack, 1050 + count);
    InsertComp(catalog_link, kLink);
  }

  count = 1;
  cur_y = top;
  delta_y = (bottom - top - 100) / (kStatisticsBorrowRecordMax + 1);
  Label* record_title = CreateLabel((Rect){right_x, 0, 0, cur_y += delta_y},
                                    "该分类下借还数据：", kBlack, NULL_ID);
  InsertComp(record_title, kLabel);

  Label* borrow_frequency =
      CreateLabel((Rect){record_title->position.right + 10, 0, 0, cur_y},
                  "近一个月借书次数：", kBlack, NULL_ID);

  Label* borrow_times =
      CreateLabel((Rect){borrow_frequency->position.right + 10, 0, 0, cur_y},
                  "", kBlack, NULL_ID);
  sprintf(borrow_times->caption, "%d", statistics->frequency);

  InsertComp(borrow_frequency, kLabel);
  InsertComp(borrow_times, kLabel);
  for (ListNode* p = statistics->borrow_record_start;
       p != statistics->borrow_record->dummy_tail &&
       count <= kStatisticsCatalogsMax;
       p = p->nxt, count++) {
    BorrowRecord* borrow_record = p->value;
    Label* user_id = CreateLabel((Rect){right_x, 0, 0, cur_y += delta_y},
                                 borrow_record->user_id, kBlack, NULL_ID);
    Label* info_1 =
        CreateLabel((Rect){user_id->position.right + 10, 0, 0, cur_y}, "在",
                    kBlack, NULL_ID);
    Label* borrow_time =
        CreateLabel((Rect){info_1->position.right + 10, 0, 0, cur_y},
                    borrow_record->borrowed_date, kBlack, NULL_ID);
    Label* info_2 = CreateLabel(
        (Rect){borrow_time->position.right + 10, 0, 0, cur_y},
        borrow_record->book_status == BORROWED ? "借出，应于" : "借出，于",
        kBlack, NULL_ID);
    Label* return_time =
        CreateLabel((Rect){info_2->position.right + 10, 0, 0, cur_y},
                    borrow_record->returned_date, kBlack, NULL_ID);
    Label* info_3 =
        CreateLabel((Rect){return_time->position.right + 10, 0, 0, cur_y},
                    "归还", kBlack, NULL_ID);
    Label* book_id =
        CreateLabel((Rect){info_3->position.right + 10, 0, 0, cur_y},
                    borrow_record->book_id, kBlack, NULL_ID);
    InsertComp(user_id, kLabel);
    InsertComp(info_1, kLabel);
    InsertComp(borrow_time, kLabel);
    InsertComp(info_2, kLabel);
    InsertComp(return_time, kLabel);
    InsertComp(info_3, kLabel);
    InsertComp(book_id, kLabel);
  }

  Button* left_pre_page_button =
      CreateButton((Rect){left_border, left_border + 80, bottom - 50, bottom},
                   "上一页", SEARCH_COLOR, 1, kWhite, 1001);
  Button* left_next_page_button =
      CreateButton((Rect){middle - 90, middle - 10, bottom - 50, bottom},
                   "下一页", SEARCH_COLOR, 1, kWhite, 1002);
  Button* right_pre_page_button =
      CreateButton((Rect){middle + 10, middle + 90, bottom - 50, bottom},
                   "上一页", SEARCH_COLOR, 1, kWhite, 1003);
  Button* right_next_page_button =
      CreateButton((Rect){right_border - 80, right_border, bottom - 50, bottom},
                   "下一页", SEARCH_COLOR, 1, kWhite, 1004);
  InsertComp(right_next_page_button, kButton);
  InsertComp(right_pre_page_button, kButton);
  InsertComp(left_next_page_button, kButton);
  InsertComp(left_pre_page_button, kButton);
}

/*
 * 处理统计界面的回调函数
 * 1001 分类统计标签上一页
 * 1002 分类统计标签下一页
 * 1003 分类统计结果上一页
 * 1004 分类统计结果下一页
 * 1050 - ? 分类标签
 */
void HandleStatisticsCallback(int id) {
  Statistics* cur_state = cur_info;
  switch (id) {
    case 1:
      cur_state->turn_page(0, 0);
      break;
    case 2:
      cur_state->turn_page(1, 0);
      break;
    case 3:
      cur_state->turn_page(0, 1);
      break;
    case 4:
      cur_state->turn_page(1, 1);
      break;
    default:
      cur_state->select_callback(catalog_on_page[id - 50]);
      break;
  }
}

// 绘制界面
void AddContents() {
  switch (cur_page) {
    case kWelcome:
      AddWelcome();
      break;
    case kLendAndBorrow:
      AddLendAndBorrow();
      break;
    case kBookSearch:
      AddBookSearch();
      break;
    case kUserSearch:
      AddUserSearch();
      break;
    case kManual:
    case kAbout:
      AddManual();
      break;
    case kUserRegister:
      AddUserRegister();
      break;
    case kUserLogIn:
      AddUserLogIn();
      break;
    case kUserModify:
      AddUserModify();
      break;
    case kUserManagement:
      AddUserManagement();
      break;
    case kLibrary:
      AddLibrary();
      break;
    case kBookDisplay:
      AddBookDisplay();
      break;
    case kBookModify:
    case kBookInit:
      AddBookModify();
      break;
    case kBorrowDisplay:
      AddBorrowDisplay();
      break;
    case kStatistics:
      AddStatistics();
      break;
  }
}

// 提供给view调用的函数，绘制当前界面
void DrawUI(Page page, User* user, void* info, char* terminal) {
  cur_page = page;
  cur_user = user;
  cur_info = info;
  strcpy(cur_terminal, terminal);
  free(terminal);
  InitPage();
}

// 初始化一个新界面
void InitPage() {
  InitFrame();
  InitSurface();
  InitComponents();
  AddHeadBar();
  AddFooBar();
  AddContents();
  FlushScreen(GetMouseX(), GetMouseY());
}

// 初始化GUI模块
void InitGUI() {
  InitGraphics();
#ifdef _DEBUG
  InitConsole();
#endif  // _DEBUG
  InitializeUI();
  SetWindowTitle("eLibrary");
  cur_page = kWelcome;
  cur_user = NULL;
  hb_status = kUnclicked;
  loadImage(".\\Resource\\folder.jpg", &folder_icon_image);
  loadImage(".\\Resource\\book.jpg", &book_icon_image);
  loadImage(".\\Resource\\sync_alt.jpg", &lend_icon_image);
  loadImage(".\\Resource\\person.jpg", &user_icon_image);
  loadImage(".\\Resource\\search.jpg", &search_icon_image);
  loadImage(".\\Resource\\show_chart.jpg", &statistics_icon_image);
  loadImage(".\\Resource\\about.jpg", &about_image);
  loadImage(".\\Resource\\manual.jpg", &manual_image);
  loadImage(".\\Resource\\welcome.jpg", &welcome_image);
  InitPage();
}

/*
 * 处理回调函数
 * Callback id:
 * 1 文件新建
 * 2 文件打开
 * 3 文件保存
 * 4 退出
 * 5 图书新建
 * 6 图书显示
 * 7 登录
 * 8 用户新建
 * 9 用户审核
 * 10 用户登出
 * 11 关于
 * 12 用户手册
 * 13 用户搜索
 * 14 图书搜索
 * 15 用户修改
 */
void CallbackById(int id) {
#ifdef _DEBUG
  printf("%d\n", id);
#endif  // _DEBUG
  if (id < 0) {
    // click on the head bar
    InitSurface();
    AddSubmenu(id);
    switch (id) {
      case LEND_ID:
        NavigationCallback(kLendAndBorrow);
        break;
      case ST_ID:
        NavigationCallback(kStatistics);
        break;
      case RTN_ID:
        NavigationCallback(kReturn);
        break;
    }
  } else if (id < 100) {
    switch (id) {
      case 1:
        NavigationCallback(kInitLibrary);
        break;
      case 2:
        NavigationCallback(kOpenLibrary);
        break;
      case 3:
        NavigationCallback(kSaveLibrary);
        break;
      case 4:
        NavigationCallback(kExit);
        break;
      case 5:
        NavigationCallback(kBookInit);
        break;
      case 6:
        NavigationCallback(kLibrary);
        break;
      case 7:
        NavigationCallback(kUserLogIn);
        break;
      case 8:
        NavigationCallback(kUserRegister);
        break;
      case 9:
        NavigationCallback(kUserManagement);
        break;
      case 10:
        NavigationCallback(kLogout);
        break;
      case 11:
        NavigationCallback(kAbout);
        break;
      case 12:
        NavigationCallback(kManual);
        break;
      case 13:
        NavigationCallback(kUserSearch);
        break;
      case 14:
        NavigationCallback(kBookSearch);
        break;
      case 15:
        NavigationCallback(kUserModify);
        break;
    }
  } else {
    switch (id / 100) {
      case 1:
        HandleUserSearchCallback(id % 100);
        break;
      case 2:
        HandleUserRegisterCallback(id % 100);
        break;
      case 3:
        HandleUserModifyCallback(id % 100);
        break;
      case 4:
        HandleUserLoginCallback(id % 100);
        break;
      case 5:
        HandleLibraryCallback(id % 100);
        break;
      case 6:
        HandleBookSearchCallback(id % 100);
        break;
      case 7:
        HandleUserManagementCallback(id % 100);
        break;
      case 8:
        HandleBookCallback(id % 100);
        break;
      case 9:
        HandleBorrowDisplayCallback(id % 100);
        break;
      case 10:
        HandleStatisticsCallback(id % 100);
        break;
      case 11:
        HandleLendAndBorrowCallback(id % 100);
        break;
    }
  }
#ifdef _DEBUG
  printf("%d\n", id);
#endif  // _DEBUG
}

// 快捷键
void HandleCtrl(int key) {
  switch (key) {
    case 6:  // F
      InitSurface();
      AddSubmenu(FILE_ID);
      break;
    case 2:  // B
      InitSurface();
      AddSubmenu(BOOK_ID);
      break;
    case 19:  // S
      InitSurface();
      AddSubmenu(SEARCH_ID);
      break;
    case 21:  // U
      InitSurface();
      AddSubmenu(USER_ID);
      break;
    case 8:  // H
      InitSurface();
      AddSubmenu(HELP_ID);
      break;
  }
}
