#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include "client.h"
#define PORT 8080

void Exit(int sd)
{
    close(sd);
    printf("Connection Ended. :)\n");
    exit(0);
}

void getAccountDetails(int sd)
{
    // struct account acc;
    int dets[4];
    read(sd, dets, sizeof(dets));
    if (dets[0] == -1) printf("No details found.\n");

    else{
        int type = dets[3];

        printf("AccountNo: %d\n", dets[1]);

        if (type == 0)
            printf("accountType: Single Account\n");
        else
            printf("accountType: Joint Account\n");

        printf("account balance: %d\n", dets[2]);

    }
    
}

void changePass(int sd)
{
    char old_pass[30], new_pass[30];
    int res;
    printf("Enter old Password: ");
    scanf("%s", old_pass);

    write(sd, old_pass, sizeof(old_pass));
    printf("Enter new Password: ");
    scanf("%s", new_pass);

    write(sd, new_pass, sizeof(new_pass));
    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
    {
        printf("Password Changed successfully\n");
    }
    else
    {
        printf("Error Occurred.\n");
    }
}

void getBalance(int sd)
{
    int res;
    read(sd, &res, sizeof(res));
    if (res > 0)
    {
        printf("current balance: %d\n", res);
    }
    else
    {
        printf("unable to display balance\n");
    }
}

void depostAmount(int sd)
{
    int deposit;
    int res;
    printf("Input Deposit Amount: ");
    scanf("%d", &deposit);

    write(sd, &deposit, sizeof(deposit));

    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
    {
        printf("Money deposited successfully\n");
    }
    else
    {
        printf("Error Occurred.\n");
    }
}

void withdrawAmount(int sd)
{
    int withdraw;
    int res;

    printf("Enter the amount you would like to withdraw: ");
    scanf("%d", &withdraw);

    write(sd, &withdraw, sizeof(withdraw));

    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
    {
        printf("Money Withdrawal Successful\n");
    }
    else
    {
        printf("Unknown Error Occurred.\n");
    }
}

int showNormalLoginOptions(int sd)
{
    write(1, "1. Deposit\n", sizeof("1. Deposit\n"));
    write(1, "2. Withdraw\n", sizeof("1. Withdraw\n"));
    write(1, "3. Balance Enquiry\n", sizeof("3. Balance Enquiry\n"));
    write(1, "4. Password Change\n", sizeof("4. Password Change\n"));
    write(1, "5. Account Details\n", sizeof("5. Account Details\n"));
    write(1, "6. Exit\n", sizeof("6. Exit\n"));
    int response;
    scanf("%d", &response);
    write(sd, &response, sizeof(response));

    switch (response)
    {
    case 1:
        depostAmount(sd);
        break;
    case 2:
        withdrawAmount(sd);
        break;
    case 3:
        getBalance(sd);
        break;
    case 4:
        changePass(sd);
        break;
    case 5:
        getAccountDetails(sd);
        break;
    case 6:
        Exit(sd);
        break;
    }
    showNormalLoginOptions(sd);
}

int showAdminLoginOptions(int sd, char *username)
{
    write(1, "1. Add Account\n", sizeof("1. Add Account\n"));
    write(1, "2. Delete Account\n", sizeof("1. Delete Account\n"));
    write(1, "3. Modify Account Type\n", sizeof("3. Modify Account Type\n"));
    write(1, "4. Password Change\n", sizeof("4. Password Change\n"));
    write(1, "5. Account Details\n", sizeof("5. Account Details\n"));
    write(1, "6. Exit\n", sizeof("6. Exit\n"));
    int response;
    scanf("%d", &response);
    write(sd, &response, sizeof(response));
    switch (response)
    {
    case 1:
        addAccount(sd);
        break;
    case 2:
        deleteAccount(sd);
        break;
    case 3:
        ModifyAccountTypeHandler(sd);
        break;
    case 4:
        modifyAdminPass(sd, username);
        break;
    case 5:
        viewAccountDetails(sd);
        break;
    case 6:
        Exit(sd);
        break;
    }
    showAdminLoginOptions(sd, username);
}

void viewAccountDetails(int sd)
{
    int user_id;
    printf("Enter user ID to check: ");
    scanf("%d", &user_id);
    write(sd, &user_id, sizeof(user_id));

    getAccountDetails(sd);
}

void modifyAdminPass(int sd, char *username)
{
    char opass[30];
    printf("Enter your current admin account password: ");
    scanf("%s", opass);
    char npass[30];
    printf("Enter your new admin account password: ");
    scanf("%s", npass);
    write(sd, opass, sizeof(opass));
    write(sd, npass, sizeof(npass));

    int res;
    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
    {
        printf("Admin Password Changed Successfully\n");
    }
    else
    {
        printf("Unknow Error Occurred.\n");
    }
}

void ModifyAccountTypeHandler(int socket_d)
{

    int account_type;
    int option = 3;
    int user_id;
    printf("Input the user id: ");
    scanf("%d", &user_id);

    printf("New Account type:\n0. Single Account\n1. Joint Account\n");
    scanf("%d", &account_type);

    write(socket_d, &user_id, sizeof(user_id));

    write(socket_d, &account_type, sizeof(account_type));
    int ret;
    read(socket_d, &ret, sizeof(ret));
    if (ret == 0)
    {
        printf("user id:%d's type successfully set\n", user_id);
    }
    else
    {
        printf("failed to change account type\n");
    }
}
void deleteAccount(int sd)
{
    int account_no;

    printf("Enter the Account No. you want to delete: ");
    scanf("%d", &account_no);

    write(sd, &account_no, sizeof(account_no));

    int res;
    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
    {
        printf("Account deleted successfully.\n");
    }
    else
    {
        printf("Unknown error occurred.\n");
    }
}

int addAccount(int sd)
{
    int type;
    printf("Input the type of account you want to add: ");
    scanf("%d", &type);

    write(sd, &type, sizeof(type));
    char pass;
    int ret;
    read(sd, &ret, sizeof(ret));
    if (ret == SUCCESS)
    {
        printf("Account successfully added\n");
        read(sd, &pass, 1);
        printf("Your password is: %c .Note this randomly generated and is advised that you change it.\n", pass);
    }
    else
    {
        printf("Error occured.\n");
    }

    return 0;
}

void normalLogin(int sd)
{

    enum LoginType type = normal;
    int temp = write(sd, &type, sizeof(type));

    struct UserCred *request = malloc(sizeof(struct UserCred));
    int userID;
    char password[30];
    printf("--------- Login ---------\n");
    printf("UserID: ");
    scanf("%d", &request->id);
    printf("password: ");
    scanf("%s", password);
    strcpy(request->password, password);

    int ret = write(sd, request, sizeof(struct UserCred));

    int res;
    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
        showNormalLoginOptions(sd);
    else
    {
        printf("Invalid Details\n");
        showLoginOptions(sd);
    }
}

int adminlogin(int sd)
{
    enum LoginType type = admin;
    int temp = write(sd, &type, sizeof(type));
    struct admin_credentials *request = malloc(sizeof(struct admin_credentials));
    char username[50];
    char password[50];
    printf("--------- Admin Login ---------\n");
    printf("username: ");
    scanf("%s", username);
    printf("password: ");
    scanf("%s", password);
    strcpy(request->password, password);
    strcpy(request->username, username);
    int ret = write(sd, request, sizeof(struct admin_credentials));

    int res;
    read(sd, &res, sizeof(res));
    if (res == SUCCESS)
        showAdminLoginOptions(sd, username);
    else
    {
        printf("Invalid Details\n");
        showLoginOptions(sd);
    }
}

void showLoginOptions(int sd)
{
    printf("\n0. Regular Login\n1. Admin Login\n");
    int res;
    scanf("%d", &res);

    if (!res)
        normalLogin(sd);
    else
        adminlogin(sd);
}

int main()
{
    struct sockaddr_in server;
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection Failed:\n");
        return 0;
    }
    showLoginOptions(socket_fd);
    return 0;
}