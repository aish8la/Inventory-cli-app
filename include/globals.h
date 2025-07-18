#ifndef GLOBALS_H
#define GLOBALS_H



//CONSTRAINTS

//Items Constraints
#define ITEM_CODE_LENGTH  8 
#define ITEM_NAME_LENGTH 30

//User Constraints
#define MAX_USER_PASSWORD_LENGTH 15
#define MAX_NUMBER_OF_USERS       5
#define MAX_LOGIN_ATTEMPTS        3
#define MAX_USER_NAME_LENGTH     15



//STRUCTS

//Login Related Structs
typedef struct {
    char username[MAX_USER_NAME_LENGTH];
    char password[MAX_USER_PASSWORD_LENGTH];
    int access_level; //user access level 1 to 3 with 3 being the highest access level and 1 being read only with limited access
} User;

typedef struct {
    User user_arr[MAX_NUMBER_OF_USERS];
    int user_count;
} User_Array;

/*This is a type def of a Menu item structure which will hold the pointer to the menu item function
and the label of the menu item
this allows for dynamic menu generation*/
typedef struct {
    const char* label;
    int (*action)(void);
    int req_access_lvl; // check login.h for more
} Menu_Item;


#endif