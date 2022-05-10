#include <stdio.h>
#include "server.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "operations.h"

int get_balance(int client, int userID) // client is client socket fd and user id uniquely determines a user
{
    int bal = getBalance(userID); // call db ops get balance or something

    write(client, &bal, sizeof(bal)); // write to client their balance
    if (bal < 0)
        return NOT_ENOUGH;
    else
        return SUCCESS;
}

int deposit(int client_fd, int userID)
{
    int amt, return_value;

    read(client_fd, &amt, sizeof(amt)); // how much does client want to deposit
    if (amt < 0)
    {
        return_value = ERROR;
        write(client_fd, &return_value, 4);
        return return_value;
    }

    int check = Deposit(userID, amt); // call db op here

    if (check == 0)
        return_value = SUCCESS;
    else
        return_value = DEPOSIT_FAILED;

    write(client_fd, &return_value, 4);
    return return_value;
}

int withdraw(int clientfd, int userID)
{
    int amt, return_value;
    read(clientfd, &amt, sizeof(amt)); // how much does client want to deposit
    if (amt < 0)
    {
        return_value = ERROR;
        write(clientfd, &return_value, 4);
        return return_value;
    }

    int check = Withdraw(userID, amt); // call db op here

    if (check == 0)
        return_value = SUCCESS;
    else
        return_value = DEPOSIT_FAILED;

    write(clientfd, &return_value, 4);
    return return_value;
}

int password_change(int clientfd, int userID)
{
    char buff[30] = {0};
    char oldpass[30] = {0};

    read(clientfd, oldpass, sizeof(oldpass));

    read(clientfd, buff, sizeof(buff)); // new password

    int check = PasswordChange(userID, oldpass, buff); // call db op here
    write(clientfd, &check, 4);
    return check;
}

int view_details(int clientfd, int userID) // view all acc and user details of userId
{
    int details[4];
    getUserDetails(userID, details);
    write(clientfd, details, sizeof(details));
    return 0;
}

int authenticate(int clientfd, int type)
{

    if (type == normal)
    {
        struct UserCred user;
        read(clientfd, &user, sizeof(struct UserCred));

        int checkauth = ValidateUserLogin(&user);
        write(clientfd, &checkauth, sizeof(checkauth));
        normal_login_prompt(clientfd, user.id);
    }
    else
    {
        struct admin_credentials admin;
        read(clientfd, &admin, sizeof(admin));

        int checkauth = ValidateAdminLogin(admin);
        write(clientfd, &checkauth, sizeof(checkauth));
        admin_login_prompt(clientfd, admin.username);
    }

    return -1;
}

int check_login(int clientfd, int type)
{

    int checkauth = authenticate(clientfd, type);

    return checkauth;
}

int admin_login_prompt(int clientfd, char *username)
{
    int choice;

    read(clientfd, &choice, sizeof(choice));

    switch (choice)
    {
    case 1:
        add_account(clientfd);
        break;
    case 2:
        delete_account(clientfd);
        break;
    case 3:
        modify_account_type(clientfd);
        break;
    case 4:
        modify_admin_password(clientfd, username);
        break;
    case 5:
        viewAccountDetails(clientfd);
        break;
    case 6:
        client_exit(clientfd, -1);
        return 0;
        break;
    }
    admin_login_prompt(clientfd, username);

    return 0;
}

void add_account(int clientfd)
{
    char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                         'h', 'i', 'j', 'k', 'l', 'm', 'n',
                         'o', 'p', 'q', 'r', 's', 't', 'u',
                         'v', 'w', 'x', 'y', 'z'};

    int uid = getUserId();
    int aid = getAccountId();
    int x = rand() % 26;
    int type;
    read(clientfd, &type, sizeof(type));
    struct account acc = {aid, type, 0, 1};
    printf("New user id: %d\n", uid);
    char pass = alphabet[x];

    if (!type)
    {
        struct user usr = {uid, aid, pass, 0, 1};
        usr.flag = 1;
        addUser(usr);
    }
    else
    {
        struct user usr1 = {uid++, aid, pass, 0, 1};
        struct user usr2 = {uid, aid, pass, 0, 1};
        usr1.flag = 1;
        usr2.flag = 1;
        addUser(usr1);
        addUser(usr2);
    }
    int check = AddAccount(acc);
    write(clientfd, &check, sizeof(check));
    write(clientfd, &pass, 1);
    updateMaxUsers(++uid);
    updateMaxAccounts(++aid);
}

int delete_account(int clientfd)
{
    int accno;
    read(clientfd, &accno, sizeof(accno)); // input account number to be deleted

    int check = DeleteAccount(accno); // call db operation here
    write(clientfd, &check, sizeof(check));

    return check;
}

int modify_account_type(int clientfd)
{
    int userid;
    read(clientfd, &userid, sizeof(userid)); // input account number to be deleted
    int new_type;
    read(clientfd, &new_type, sizeof(new_type));

    printf("user id =%d\n", userid);
    printf("type = %d\n", new_type);
    int check = AdminModifyAccount(userid, new_type); // call db operation here
    write(clientfd, &check, sizeof(check));

    return check;
}

int modify_admin_password(int clientfd, char *username)
{

    char oldpass[30] = {0};
    char newpass[30] = {0};
    read(clientfd, oldpass, sizeof(oldpass));
    read(clientfd, newpass, sizeof(newpass));
    int check = AdminPasswordChange(username, oldpass, newpass);

    write(clientfd, &check, sizeof(int));
    return check;
}

int viewAccountDetails(int clientfd)
{
    int uid;
    read(clientfd, &uid, sizeof(uid));
    view_details(clientfd, uid);
}

int normal_login_prompt(int clientfd, int userid)
{
    int choice;

    read(clientfd, &choice, sizeof(choice));

    switch (choice)
    {
    case 1:
        deposit(clientfd, userid);
        break;

    case 2:
        withdraw(clientfd, userid);
        break;
    case 3:
        get_balance(clientfd, userid);
        break;

    case 4:
        password_change(clientfd, userid);
        break;
    case 5:
        view_details(clientfd, userid);
        break;
    case 6:
        client_exit(clientfd, userid);
        return 0;
        break;
    }

    normal_login_prompt(clientfd, userid); // calls itself forever until client exit
    return 0;
}
int client_exit(int clientfd, int userid)
{
    write(clientfd, "Bye", sizeof("Bye"));
    close(clientfd);
}
int check_logged_in(int id, int cookie)
{
    for (int i = 0; i < no_logged_in; i++)
    {
        if (loggedin_users[i].id == id && loggedin_users[i].cookie == cookie)
            return SUCCESS;
    }

    return NOT_LOGGED_IN;
}

int login(int clientfd)
{
    enum LoginType type;

    read(clientfd, &type, sizeof(type));

    authenticate(clientfd, type);
}

int main()
{
    int socket_d;
    struct sockaddr_in serveraddr, clientaddr;
    int opt = 1;

    socket_d = socket(AF_INET, SOCK_STREAM, 0); // creating socket in namespace
    if (socket_d == 0)
    {
        int err = errno;
        fprintf(stderr, "server end socket creation failed: %s\n", strerror(err));
        return -1;
    }
    // forcing port reuse
    setsockopt(socket_d, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);

    if (bind(socket_d, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        int err = errno;
        fprintf(stderr, "server end binding failed: %s\n", strerror(err));
        return -1;
    }

    socklen_t addr_size = sizeof(clientaddr);
    listen(socket_d, 10);
    int childpid;

    while (1)
    {
        int client = accept(socket_d, (struct sockaddr *)&clientaddr, &addr_size);
        if (client < 0)
        {
            int err = errno;
            fprintf(stderr, "connection failed: %s\n", strerror(err));
            return -1;
        }
        printf("connection accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        if ((childpid = fork()) == 0)
        {
            close(socket_d);
            login(client);
            return 1;
        }
        else
        {
            close(client);
        }
    }
    return 0;
}
