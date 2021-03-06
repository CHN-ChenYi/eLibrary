﻿#ifndef BASICTYPE_H_
#define BASICTYPE_H_

#include <stdint.h>

#include "genlib.h"
#include "graphics.h"

typedef struct Book {
  unsigned uid;
  char id[20];
  char title[200];
  char authors[3][200];
  char category[50];
  char press[200];
  char publication_date[10];
  char keywords[5][50];
  unsigned int number_on_the_shelf;
  unsigned int available_borrowed_days;
} Book;

typedef enum Identity { NORMAL_USER = 0, ADMINISTRATOR } Identity;
typedef enum Gender { MALE = 0, FEMALE } Gender;

typedef struct User {
  unsigned uid;
  char id[20];
  char name[20];
  char salt[10];
  uint32_t password[8];
  Gender gender;
  char department[200];
  Identity whoami;
  bool verified;
} User;

typedef enum BookStatus { RETURNED = 0, BORROWED } BookStatus;

typedef struct BorrowRecord {
  unsigned uid;
  unsigned book_uid;
  unsigned user_uid;
  char book_id[20];
  char user_id[20];
  char borrowed_date[10];
  BookStatus book_status;
  // if book_status == BORROWED, returned_date stores the expected returned date
  char returned_date[10];
} BorrowRecord;

#endif  // BASICTYPE_H_
