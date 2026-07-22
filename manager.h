#ifndef MANAGER_H
#define MANAGER_H

#define MAX_FILES 100

typedef struct {
    char fileName[256];
    char filePath[1024];
    char savedHash[33];
} FileBase;

extern FileBase fileBase[MAX_FILES];
extern int fileCount;
extern char currentUser[50];

void loadUserBase(const char* username);
void saveUserBase();
void addFileToBase(const char *path, const char *hash);
void displayBase();
void runCheck();
void runDelete();
void get_hash(const char *path, char *output);

// ฟังก์ชันดักจับการกรอก File Path
int getValidFilePath(const char *prompt, char *pathBuffer, int maxLen);

#endif