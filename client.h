#define NOT_ENOUGH -1
#define ERROR -2
#define DEPOSIT_FAILED -3
#define AUTHENTICATION_FAILED -4
#define NOT_LOGGED_IN -5
#define SUCCESS 0


#ifndef CLIENT_H
#define CLIENT_H
#include "datastructures.h"

int normallogin(int client);
int adminlogin(int client); 
void deleteAccount( int socket_d);
void AddAccountHandler( int socket_d);
void modifyAdminPass( int socket_d, char *username);
void ModifyAccountTypeHandler( int socket_d);
void viewAccountDetails(int socket_d);
void showLoginOptions(int sd);
int addAccount(int sd);
int ExitHandler(int sd);
#endif