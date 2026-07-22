#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char username[35]; // เผื่อที่ให้ \0
    char password[35];
} User;

int registerUser();
int loginUser(char *loggedUser);

#endif