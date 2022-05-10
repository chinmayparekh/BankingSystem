#ifndef datastructures_h
#define datastructures_h

#define PASSWORD_LENGTH 30
#define USERNAME_LENGTH 100
enum AccountType
{
    single,
    joint
};

enum LoginType
{
    normal,
    admin
};

struct user
{
    int id;
    int account_id;
    char password[PASSWORD_LENGTH];
    enum LoginType user_type;
    int flag;
};

struct account
{
    int id;
    enum AccountType acctype;
    int balance;
    int flag; // 0 for deleted and 1 if present
};

struct transaction
{
    int txn_id;
    int acc_id;
    int user_id;
};

struct user_credentials
{
    int id;
    char password[PASSWORD_LENGTH];
};

struct admin_credentials
{
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    int found;
};
struct Id_Cookie
{
    int id;
    int cookie;
    int is_deleted; // flag. set to 1 when logging oput
};
struct Id_Cookie loggedin_users[1000];

struct UserCred
{
    int id;
    char password[PASSWORD_LENGTH];
};

#endif