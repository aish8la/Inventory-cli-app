#ifndef LOGIN_H
#define LOGIN_H


#define MAX_USER_NAME_LENGTH 15
#define MAX_USER_PASSWORD_LENGTH 15
#define MAX_NUMBER_OF_USERS 5

typedef struct {
    char username[MAX_USER_NAME_LENGTH];
    char password[MAX_USER_PASSWORD_LENGTH];
    char role[10];
} User;

typedef struct {
    User user_arr[MAX_NUMBER_OF_USERS];
    int user_count;
} User_Array;

int login(void);

#endif