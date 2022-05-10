#define NOT_ENOUGH -1
#define ERROR -2
#define DEPOSIT_FAILED -3
#define AUTHENTICATION_FAILED -4
#define NOT_LOGGED_IN -5
#define SUCCESS 0

#define PORT 8080

#ifndef SERVER_H
#define SERVER_H

#include "datastructures.h"
#include "operations.h"

int get_balance(int client, int userID);
int deposit(int client, int userID);
int withdraw(int client, int userID);
int password_change(int client, int userID);
int view_details(int client, int userID);
int client_exit(int client, int userID);
int authenticate(int clientfd, int type);
int check_login(int clientfd, int type);
int login(int client);
int mainController(int client, int userID, long cookie);
int modify_account_type(int client);
int modify_admin_password(int client, char *);
void add_account(int client);
int viewAccountDetails(int client);
int delete_account(int client);
int normal_login_prompt(int clientfd, int userid);
int admin_login_prompt(int clientfd, char *username);
int no_logged_in = 0;
#endif /* !SERVER_H */