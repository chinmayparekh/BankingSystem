#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "datastructures.h"

int NUMBER_ACCOUNT = 3;
int NUMBER_USERS  = 3;
#define NUMBER_ADMINS 2
int populate_account()
{
    int fd_account = open("accounts.dat", O_CREAT | O_RDWR, 0744);
    if (fd_account == -1)
    {
        printf("unable to create and open file\n");
        return -1;
    }
    lseek(fd_account, 0, SEEK_SET);

    struct account listOfAccount[NUMBER_ACCOUNT];

    listOfAccount[0].id = 0;
    listOfAccount[0].acctype = 0;
    listOfAccount[0].balance = 10000;
    listOfAccount[0].flag=1;

    listOfAccount[1].id = 1;
    listOfAccount[1].acctype = 1;
    listOfAccount[1].balance =20000;
    listOfAccount[1].flag=1;

    listOfAccount[2].id = 2;
    listOfAccount[2].balance = 5000;
    listOfAccount[2].acctype = 0;
    listOfAccount[2].flag=1;

    for (int i = 0; i < NUMBER_ACCOUNT; i++)
    {
        write(fd_account, &listOfAccount[i], sizeof(struct account));
    }
    close(fd_account);
    return 0;
}

int populate_user()
{
    int fd_user = open("users.dat", O_CREAT | O_RDWR, 0744);
    if (fd_user == -1)
    {
        printf("unable to create and open file\n");
        return -1;
    }
    lseek(fd_user, 0, SEEK_SET);

    struct user listOfUser[NUMBER_USERS];

    listOfUser[0].id = 0;
    listOfUser[0].account_id = 0;
    strcpy(listOfUser[0].password, "user1");
    listOfUser[0].user_type = single;
    listOfUser[0].flag = 1;

    listOfUser[1].id = 1;
    listOfUser[1].account_id = 1;
    strcpy(listOfUser[1].password, "user2");
    listOfUser[1].user_type = single;
    listOfUser[1].flag = 1;

    listOfUser[2].id = 2;
    listOfUser[2].account_id = 2;
    strcpy(listOfUser[2].password, "user3");
    listOfUser[2].user_type = single;
    listOfUser[1].flag = 1;

    for (int i = 0; i < NUMBER_USERS; i++)
    {
        write(fd_user, &listOfUser[i], sizeof(struct user));
    }
    return 0;
}

int populate_admin()
{
    int fd = open("admins.dat", O_CREAT | O_RDWR, 0777);
    if (fd == -1)
    {
        printf("unable to create and open file\n");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);

    struct admin_credentials admins[NUMBER_ADMINS];
    strcpy(admins[0].username, "admin1");
    strcpy(admins[0].password, "password1");

    strcpy(admins[1].username, "admin2");
    strcpy(admins[1].password, "password2");
    for (int i = 0; i < NUMBER_ADMINS; i++)
    {
        write(fd, &admins[i], sizeof(struct admin_credentials));
    }
    close(fd);
    return 0;
}

int populate_values(){
    int fd = open("maxvalues", O_RDWR | O_CREAT | 0744);
    write(fd, &NUMBER_USERS,4);
    write(fd, &NUMBER_ACCOUNT, 4);
    return 0;
}

int main()
{
    int status1 = populate_account();
    int status2 = populate_user();
    int status3 = populate_values();
    populate_admin();
    if (status1 == 0 && status2 == 0)
    {
        printf("Data is populated successfully!\n");
        return 0;
    }
    return 1;
}