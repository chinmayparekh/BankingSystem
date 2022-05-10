#ifndef operations_h
#define operations_h

#include "datastructures.h"

struct user getUser(int id);
struct account getAccount(int userId);
int getBalance(int userId);
int Deposit(int userId, int amount);
int Withdraw(int userId, int amount);
int PasswordChange(int userId, char *currentpassword, char *newpassword);
void ViewDetails(int userId);
int AddAccount(struct account acc);
int db_delete_account(int accno);
int ValidateUserLogin(struct UserCred *user);
int ValidateAdminLogin(struct admin_credentials admin);
void getUserDetails(int userId, int p[]);
int DeleteAccount(int accno);
int addUser(struct user);
int AdminPasswordChange(char *username, char *currentpassword, char *newpassword);
int getUserId();
int getAccountId();
void updateMaxUsers(int);
void updateMaxAccounts(int);
void updateUserFile(int);
int AdminModifyAccount(int userId, int accountType);
#endif