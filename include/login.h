#ifndef LOGIN_H
#define LOGIN_H

enum Login_Constraints {
    MAX_USER_NAME_LENGTH = 15,
    MAX_USER_PASSWORD_LENGTH = 15,
    MAX_NUMBER_OF_USERS = 5,
    MAX_LOGIN_ATTEMPTS = 3,
};

typedef struct {
    char username[MAX_USER_NAME_LENGTH];
    char password[MAX_USER_PASSWORD_LENGTH];
    int access_level; //user access level 1 to 3 with 3 being the highest access level and 1 being read only with limited access
} User;

typedef struct {
    User user_arr[MAX_NUMBER_OF_USERS];
    int user_count;
} User_Array;

int login(void);

#endif