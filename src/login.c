#include <stdio.h>
#include <string.h>
#include "login.h"


const User default_user = {
    "admin",
    "1234",
    "admin",
};

User_Array user_data;

int login(void) {

    User user_arr[MAX_NUMBER_OF_USERS];

    FILE *fp = fopen("configs.dat", "rb");

    if(fp == NULL) {
        user_data.user_arr[0] = default_user;
        user_data.user_count = 1;
        fp = fopen("configs.dat", "wb");
        fwrite(&user_data, sizeof(User_Array), 1, fp);
        fclose(fp);
        fp = fopen("configs.dat","rb");
    }

    int user_count = fread(&user_data, sizeof(User_Array), 1, fp);

    printf("%d", user_data.user_count);

    return 0;
    
}