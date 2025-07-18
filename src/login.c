#include <stdio.h>
#include <string.h>
#include "login.h"
#include "utilities.h"
#include "globals.h"

User_Array user_data = {
    {
        {
            "admin",
            "123",
            3
        },
        {
            "user",
            "123",
            2
        },
        {
            "read",
            "123",
            1
        }
    },
    3
};

User current_user;

int login(void) {

    FILE *fp = fopen("configs.dat", "rb");

    //if configs.dat does not exist, create it and write the default user creds into the file
    if(fp == NULL) {
        fp = fopen("configs.dat", "wb");
        fwrite(&user_data, sizeof(User_Array), 1, fp);
        fclose(fp);
        fp = fopen("configs.dat","rb");
    }

    //to check if the user struc is read from file
    int read_config_count = fread(&user_data, sizeof(User_Array), 1, fp);

    if(read_config_count < 1) {
        fprintf(stderr, "Failed reading config.dat");
        return 1;
    }

    fclose(fp);

    char input_user_name[MAX_USER_NAME_LENGTH];
    char input_user_pass[MAX_USER_PASSWORD_LENGTH];

    printf("Enter User Name: ");
    read_input(input_user_name, sizeof(input_user_name));

    printf("Enter Password: ");
    read_input(input_user_pass, sizeof(input_user_pass));

    for (int i = 0; i < user_data.user_count; i++) {
        if (strcmp(user_data.user_arr[i].username, input_user_name) == 0 &&
            strcmp(user_data.user_arr[i].password, input_user_pass) == 0) {
                current_user = user_data.user_arr[i];
                printf("Login successful! User: %s\n", current_user.username);
                wait_for_enter();
                return 1;
            }
    }

    return 0;
    
}