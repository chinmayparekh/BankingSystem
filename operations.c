#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "operations.h"
#include "datastructures.h"
extern int errno;

struct user getUser(int id) // id starts with 0
{
    struct user u;
    int fd = open("users.dat", O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        u.flag = 0;
        return u;
    }
    static struct flock lock;
    lseek(fd, 0, SEEK_SET);
    lock.l_type = F_RDLCK;
    lock.l_start = id * sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1)
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        u.flag = 0;
        return u;
    }
    lseek(fd, id * sizeof(struct user), SEEK_SET);
    read(fd, &u, sizeof(struct user));
    if (getUserId() <= id)
    {
        u.flag = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return u;
}
struct account getAccount(int userId)
{
    struct user u = getUser(userId);
    struct account acc;
    int id = u.account_id;
    int fd = open("accounts.dat", O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return acc;
    }

    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = id * sizeof(struct account);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1)
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return acc;
    }

    lseek(fd, id * sizeof(struct account), SEEK_SET);
    read(fd, &acc, sizeof(struct account));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return acc;
}

int getBalance(int userId)
{
    struct account acc = getAccount(userId);
    if (acc.flag == 0)
        return -1;
    return acc.balance;
}

int Deposit(int userId, int amount)
{
    struct account acc = getAccount(userId);
    int fd = open("accounts.dat", O_RDWR);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = acc.id * sizeof(acc);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(acc);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1)
    {
        printf("Attempt to set write lock failed\n");
        return -1;
    }

    lseek(fd, acc.id * sizeof(acc), SEEK_SET);
    printf("initial balace : %d\n", acc.balance);

    acc.balance += amount;
    write(fd, &acc, sizeof(acc));

    lseek(fd, acc.id * (sizeof(acc)), SEEK_SET);
    read(fd, &acc, sizeof(acc));
    printf("final balance : %d\n", acc.balance);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}

int Withdraw(int userId, int amount)
{
    struct account acc = getAccount(userId);
    int fd = open("accounts.dat", O_RDWR);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = acc.id * sizeof(acc);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(acc);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1)
    {
        printf("Attempt to set write lock failed\n");
        return -1;
    }

    lseek(fd, acc.id * sizeof(acc), SEEK_SET);
    printf("initial balace : %d\n", acc.balance);
    int x = 0;
    if (acc.balance >= amount)
    {
        acc.balance -= amount;
        write(fd, &acc, sizeof(acc));
        lseek(fd, acc.id * (sizeof(acc)), SEEK_SET);
        read(fd, &acc, sizeof(acc));
        printf("final balance : %d\n", acc.balance);
    }
    else
    {
        printf("Insufficient funds for withdrawal\n");
        x = 1;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    if (x)
        return -1;
    return 0;
}

int PasswordChange(int userId, char *currentpassword, char *newpassword)
{
    struct user u = getUser(userId);
    if (strcmp(currentpassword, u.password) == 0)
    {
        strcpy(u.password, newpassword);
        int fd = open("users.dat", O_RDWR);
        if (fd == -1)
        {
            printf("Unable to open the file\n");
            return -1;
        }
        static struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_start = u.id * sizeof(u);
        lock.l_whence = SEEK_SET;
        lock.l_len = sizeof(u);
        lock.l_pid = getpid();

        int ret = fcntl(fd, F_SETLKW, &lock);
        if (ret == -1)
        {
            printf("Attempt to set write lock failed\n");
            return -1;
        }

        lseek(fd, u.id * sizeof(u), SEEK_SET);
        write(fd, &u, sizeof(u));

        lseek(fd, u.id * (sizeof(u)), SEEK_SET);
        read(fd, &u, sizeof(u));

        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);
        close(fd);
        return 0;
    }
    printf("Please retry by putting your correct current password\n");
    return -1;
}

void ViewDetails(int userId)
{
    struct user u = getUser(userId);
    struct account acc = getAccount(userId);

    printf("User Details:\n");
    printf("User ID : %d\nUser's account id : %d\n Account Type %d\n", u.id, u.account_id, u.user_type);
    printf("****************************************************************\n");
    printf("Account Details:\n");
    printf("Account balance : %d\n", acc.balance);
}

int addUser(struct user usr)
{
    int fd = open("users.dat", O_RDWR);
    lseek(fd, 0, SEEK_END);
    int ret = write(fd, &usr, sizeof(struct user));
    close(fd);
    if (ret == 1)
        return 0;
    return -1;
}

int getUserId()
{
    int fd = open("maxvalues", O_RDONLY);
    int u;
    lseek(fd, 0, SEEK_SET);
    read(fd, &u, 4);
    return u;
}
int getAccountId()
{
    int fd = open("maxvalues", O_RDONLY);
    int a;
    lseek(fd, 4, SEEK_SET);
    read(fd, &a, 4);
    return a;
}

void updateMaxUsers(int u)
{
    int fd = open("maxvalues", O_WRONLY);
    lseek(fd, 0, SEEK_SET);
    write(fd, &u, 4);
}
void updateMaxAccounts(int a)
{
    int fd = open("maxvalues", O_WRONLY);
    lseek(fd, 4, SEEK_SET);
    write(fd, &a, 4);
}

int AddAccount(struct account acc)
{

    int fd_account = open("accounts.dat", O_RDWR);
    if (fd_account == -1)
    {
        printf("unable to create and open file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int ret = fcntl(fd_account, F_SETLKW, &lock);
    if (ret == -1)
    {
        printf("Attempt to set write lock failed\n");
        return -1;
    }
    lseek(fd_account, 0, SEEK_END);
    write(fd_account, &acc, sizeof(struct account));
    lock.l_type = F_UNLCK;
    fcntl(fd_account, F_SETLKW, &lock);
    close(fd_account);
    return 0;
}

long long Search(int accno)
{
    int fd_account = open("accounts.dat", O_RDWR);
    if (fd_account < 0)
    {
        printf("unable to create and open file\n");
        return -1;
    }

    lseek(fd_account, 0, SEEK_SET);
    struct account acc;
    while (read(fd_account, &acc, sizeof(acc)) > 0)
    {

        if (acc.flag == 1 && acc.id == accno)
        {
            return lseek(fd_account, -1 * sizeof(acc), SEEK_CUR);
            // return ftell(fd_account);
        }
    }

    return -1;
}

int DeleteAccount(int accno)
{
    int fd_account = open("accounts.dat", O_RDWR);
    if (fd_account == -1)
    {
        printf("unable to create and open file\n");
        return -1;
    }
    struct account acc;

    int position = Search(accno);
    if (position != -1)
    {
        lseek(fd_account, position, SEEK_SET);
        read(fd_account, &acc, sizeof(acc));
        acc.flag = 0; // deleted
        lseek(fd_account, position, SEEK_SET);
        write(fd_account, &acc, sizeof(acc));
        updateUserFile(accno);
        return 0;
    }

    return -1;
}

void updateUserFile(int accno)
{
    int fd = open("users.dat", O_RDWR);
    struct user usr;
    while (read(fd, &usr, sizeof(usr)))
    {
        if (usr.account_id == accno)
        {
            usr.flag = 0;
            lseek(fd, -1 * sizeof(usr), SEEK_CUR);
            write(fd, &usr, sizeof(usr));
        }
    }
}

int ValidateUserLogin(struct UserCred *user)
{
    int userId = user->id;
    struct user u = getUser(userId);
    if (u.flag == 0)
    {
        return -1;
    }
    if (strcmp(user->password, u.password) == 0)
    {
        printf("User has been authenticated successfully\n");
        return 0;
    }
    printf("Authentication failed\n");
    return -1;
}

int ValidateAdminLogin(struct admin_credentials admin)
{
    struct admin_credentials a;
    int fd = open("admins.dat", O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return -1;
    }
    static struct flock lock;

    // applying a readlock as we dont know which pat has to be locked
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1)
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &a, sizeof(a)))
    {
        if (strcmp(a.username, admin.username) == 0)
        {
            if (strcmp(a.password, admin.password) == 0)
            {
                printf("Admin is verified !\n");
                return 0;
            }
        }
    }
    printf("No such admin is found!\n");
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return -1;
}
void getUserDetails(int userId, int p[])
{
    struct user u = getUser(userId);
    if (u.flag == 0)
    {
        p[0] = -1;
        p[1] = -1;
        p[2] = -1;
        p[3] = -1;
    }
    else
    {
        int account_balance = getBalance(userId);
        p[0] = userId;
        p[1] = u.account_id;
        p[2] = account_balance;
        p[3] = u.user_type;
    }
}

struct admin_credentials getAdmin(char *username, char *password)
{
    struct admin_credentials a;
    int fd = open("admins.dat", O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return a;
    }
    static struct flock lock;

    // applying a readlock as we dont know which pat has to be locked
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int ret = fcntl(fd, F_SETLK, &lock);
    if (ret == -1)
    {
        int err = errno;
        fprintf(stderr, "Attempt to set write lock failed %s\n", strerror(err));
        return a;
    }

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &a, sizeof(a)))
    {

        if (strcmp(a.password, password) == 0 && strcmp(a.username, username) == 0)
        {
            printf("Admin is found\n");
            return a;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    a.found = -1;
    printf("Failed to find any admin\n");
    return a;
}
int AdminPasswordChange(char *username, char *currentpassword, char *newpassword)
{
    struct admin_credentials admin = getAdmin(username, currentpassword);
    if (admin.found == -1)
    {
        printf("Failed to find such an admin");
    }
    else
    {
        if (strcmp(currentpassword, admin.password) == 0)
        {
            int fd = open("admins.dat", O_RDWR);
            if (fd == -1)
            {
                printf("Unable to open the file\n");
                return -1;
            }
            // locking the full file
            static struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_start = 0;
            lock.l_whence = SEEK_SET;
            lock.l_len = 0;
            lock.l_pid = getpid();

            int ret = fcntl(fd, F_SETLKW, &lock);
            if (ret == -1)
            {
                printf("Attempt to set write lock failed\n");
                return -1;
            }
            lseek(fd, 0, SEEK_SET);

            while (read(fd, &admin, sizeof(admin)))
            {

                if (strcmp(admin.password, currentpassword) == 0 && strcmp(admin.username, username) == 0)
                {
                    lseek(fd, -1 * sizeof(admin), SEEK_CUR);
                    strcpy(admin.password, newpassword);
                    write(fd, &admin, sizeof(admin));
                    printf("Password is updated successfully\n");
                    return 0;
                }
            }

            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &lock);
            close(fd);
            return 0;
        }
        printf("Please retry by putting your correct current password\n");
        return -1;
    }
    return -1;
}

int AdminModifyAccount(int userId, int accountType)
{
    struct user u = getUser(userId);
    u.user_type = accountType;
    int fd = open("users.dat", O_RDWR);
    if (fd == -1)
    {
        printf("Unable to open the file\n");
        return -1;
    }
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = u.id * sizeof(u);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(u);
    lock.l_pid = getpid();

    int ret = fcntl(fd, F_SETLKW, &lock);
    if (ret == -1)
    {
        printf("Attempt to set write lock failed\n");
        return -1;
    }

    lseek(fd, u.id * sizeof(u), SEEK_SET);
    write(fd, &u, sizeof(u));

    lseek(fd, u.id * (sizeof(u)), SEEK_SET);
    read(fd, &u, sizeof(u));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}