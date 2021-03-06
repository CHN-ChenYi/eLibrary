﻿#ifndef MODEL_H_
#define MODEL_H_

#include <stdio.h>

#include "basictype.h"
#include "list.h"
#include "utils.h"

typedef struct DB {
  String filename;
  unsigned int pk;
  unsigned int size;
  List* data;
  FILE* database;
} DB;
// Open and close the DB.
/*
  OpenDBConnection

  It's used to open the DB.
  You should call the open function when you want to access to the DB, then
  close it when you don't need to.

  Parameters:
  filename: the db file
  Model(book, user, borrowrecord)

  Return value:
  DBErrno
 */
int OpenDBConnection(const char* filename, Model model);
/*
  CloseDBConnection

  It's used to close the DB.
  You should call the open function when you want to access to the DB, then
  close it when you don't need to.

  Parameters:
  Model(book, user, borrowrecord)

  Return value:
  DBErrno
 */
int CloseDBConnection(Model model);
// Create
/*
  Create

  It's used to create data in the DB

  Parameters:
  handle - pointer pointing to the handle (book,user,record,etc...)
  model - see enum Model

  Return value:
  1 if error appears, 0 otherwise.
 */
// return 1 if error appears, 0 otherwise.
int Create(void* handle, Model model);

// Request
// For many functions in this section, the first parameter is the pointer
// pointing to the actual instance in memory(the item you want) and will be free
// when the DB is closed in order to prevent memory leakage. As a result, you
// should not access to these instances after the DB is closed.
/*
  GetById
  It's used to get data by it's id. The faster way to retrieve data.

  Parameters:

  handle - pointer pointing to the handle (book,user,record,etc...)
  id - uid
  model - see enum Model

  Return value:
  DBErrno
 */
int GetById(void* handle, unsigned int id, Model model);

/*
 Filter

 Get data by one or more arguments.

 Parameters:

 list_handle - pointer pointing to the list_handle
 pointer(book,user,record,etc...) model - see enum Model

 queries

 Formatting your queries like this:
 name1=value1&name2=value2&name3=value3...
 Ex 1.
 Assume we want to get users that are male:
 "gender=male"

 Ex 2.
 Assume we want to get books that are published by "Cambridge" on March 17,
 2013, and have one of the keyword "Math":
 "press=Cambridge&keyword=Math&published_date=2013-03-17"

 Ex 3.
 Assume we want to filter books by two keywords, "Java" and "Javacript"
 "keyword=Java&keyword=JavaScript"
 Notice that in the above example, the filter is case-sensitive.

 You can also add a ;(semicolon) in front of the = for case-insensitive search.

 Ex 4.
 Assume we want to get books that are published by O Reilly and related to C(so
 you might get books about C/C++/C#/Objective-C!) "press=O Reilly&keyword;=C"

 Return value:
 DBErrno
*/
int Filter(List* list_handle, String queries, Model model);

/*
  GetDBSize
  It's used to get the number of rows(elements) in database.

  Parameter:
  model - see enum Model
  size - the pointer pointing to the memory to store the size of DB

  Return value:
  DBErrno
*/

int GetDBSize(Model model, unsigned int* size);

/*
  GetNextPK
  It's used to get The smallest prime key that isn't mapped to a row in
  database.

  Parameter:
  model - see enum Model
  pk - the pointer pointing to the memory to store the prime key

  Return value:
  DBErrno
 */

int GetNextPK(Model model, unsigned int* pk);

// Update
/*
  Update*
  It's used to perform update operation on a row in database.

  Parameter:
  handle - pointer pointing to the handle (book,user,record,etc...)
  id  - uid
  model - see enum Model

  Return value:
  DBErrno
 */

int Update(void* handle, unsigned int id, Model model);

// Delete
/*
  Delete*
  It's used to perform delete operation on a row in database.

  Parameter:
  id - uid
  model - see enum Model

  Return value:
  DBErrno
*/

int Delete(unsigned int id, Model model);
#endif  // MODEL_H_
