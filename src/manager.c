#include "manager.h"
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// =========================================================
// ส่วนที่ 1: Global Variables
// =========================================================
FileBase fileBase[MAX_FILES];
int fileCount = 0;
char currentUser[50] = {0};

// =========================================================
// ส่วนที่ 2: ระบบจัดการฐานข้อมูล (Database Management)
// =========================================================

static void getDbName(char *out) {
    sprintf(out, "%s_base.db", currentUser);
}

void loadUserBase(const char* username) {
    strcpy(currentUser, username);
    fileCount = 0;
    char dbName[100]; getDbName(dbName);
    FILE *f = fopen(dbName, "r");
    if (!f) return;
    
    char line[2048];
    while(fgets(line, sizeof(line), f) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        char *tokHash = strtok(line, "|");
        char *tokName = strtok(NULL, "|");
        char *tokPath = strtok(NULL, "|");
        if (tokHash && tokName && tokPath && fileCount < MAX_FILES) {
            strcpy(fileBase[fileCount].savedHash, tokHash);
            strcpy(fileBase[fileCount].fileName, tokName);
            strcpy(fileBase[fileCount].filePath, tokPath);
            fileCount++;
        }
    }
    fclose(f);
}

void saveUserBase() {
    if (strlen(currentUser) == 0) return;
    char dbName[100]; getDbName(dbName);
    FILE *f = fopen(dbName, "w");
    if (!f) return;
    for(int i = 0; i < fileCount; i++) {
        fprintf(f, "%s|%s|%s\n", fileBase[i].savedHash, fileBase[i].fileName, fileBase[i].filePath);
    }
    fclose(f);
}

// =========================================================
// ส่วนที่ 3: ระบบรักษาความปลอดภัย และการอัปโหลดไฟล์
// =========================================================

void get_hash(const char *path, char *output) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        strcpy(output, "ERROR_FILE_NOT_FOUND");
        return;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buf = malloc(fsize > 0 ? fsize : 1);
    if (fsize > 0) fread(buf, 1, fsize, f);
    fclose(f);
    unsigned char digest[16];
    md5(buf, fsize, digest);
    free(buf);
    for (int i = 0; i < 16; i++) sprintf(output + (i * 2), "%02x", digest[i]);
}

void addFileToBase(const char *path, const char *hash) {
    if (fileCount < MAX_FILES) {
        strncpy(fileBase[fileCount].filePath, path, 1024);
        strncpy(fileBase[fileCount].savedHash, hash, 33);
        char tempPath[1024]; strncpy(tempPath, path, 1024);
        char *name = strrchr(tempPath, '/');
        if(!name) name = strrchr(tempPath, '\\');
        if(name) name++; else name = tempPath;
        strncpy(fileBase[fileCount].fileName, name, 256);
        fileCount++;
        saveUserBase();
        printf("\n[SUCCESS] File '%s' uploaded and secured successfully.\n", name);
    }
}

void displayBase() {
    if (fileCount == 0) {
        printf("\n--- No registered files found ---\n");
        return;
    }
    printf("\n--- Registered Files for %s ---\n", currentUser);
    for (int i = 0; i < fileCount; i++) {
        printf("%d. %s\n", i + 1, fileBase[i].fileName);
    }
}

// =========================================================
// ส่วนที่ 4: ฟังก์ชันดักจับ Input ขั้นสูง (ล้างบัฟเฟอร์แบบเงียบๆ)
// =========================================================

static int getIndexInput(const char* prompt, int* choice) {
    char inputBuffer[100];
    while(1) {
        printf("%s", prompt);
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) return 0;
        if (inputBuffer[0] == '\n') return 0; 
        
        // ล้างบัฟเฟอร์แบบเงียบๆ ไม่เด้งเตือน
        if (!strchr(inputBuffer, '\n')) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
        }

        inputBuffer[strcspn(inputBuffer, "\r\n")] = 0;
        if (strlen(inputBuffer) == 0) return 0;

        if (sscanf(inputBuffer, "%d", choice) == 1) {
            return 1; 
        }
        printf("[!] Invalid input. Please enter a valid number or press ENTER to cancel.\n");
    }
}

int getValidFilePath(const char *prompt, char *pathBuffer, int maxLen) {
    while (1) {
        printf("%s", prompt);
        if (fgets(pathBuffer, maxLen + 2, stdin) != NULL) {
            if (pathBuffer[0] == '\n') return 0;

            if (!strchr(pathBuffer, '\n')) {
                int c; while ((c = getchar()) != '\n' && c != EOF);
            }

            pathBuffer[strcspn(pathBuffer, "\r\n")] = 0;
            if (strlen(pathBuffer) == 0) return 0;

            FILE *testFile = fopen(pathBuffer, "r");
            if (testFile != NULL) {
                fclose(testFile);
                return 1; 
            } else {
                printf("[!] Warning: File not found or cannot be opened!\n");
                printf("    Please check the path and try again.\n");
            }
        } else {
            return 0;
        }
    }
}

// =========================================================
// ส่วนที่ 5: ฟีเจอร์หลัก (Check และ Delete)
// =========================================================

void runCheck() {
    if (fileCount == 0) {
        printf("\n[!] No files in base to check. Please upload first.\n");
        return;
    }
    displayBase();
    
    int choice;
    if (!getIndexInput("Select file index to check (or press ENTER to cancel): ", &choice)) {
        printf("Action cancelled.\n");
        return;
    }

    int idx = choice - 1;
    if (idx >= 0 && idx < fileCount) {
        char currentPath[1024] = {0}, currentHash[33] = {0};
        printf("\n>>> Select the file to compare with '%s'...\n", fileBase[idx].fileName);
        
        if (getValidFilePath("Enter file path (or press ENTER to cancel): ", currentPath, 1000)) {
            get_hash(currentPath, currentHash);
            if (strcmp(currentHash, "ERROR_FILE_NOT_FOUND") == 0) return;
            
            printf("\n------------------------------------\n");
            if (strcmp(currentHash, fileBase[idx].savedHash) == 0) {
                printf("[MATCH] No changes detected. The file is intact.\n");
            } else {
                printf("[ALERT] FILE HAS BEEN MODIFIED OR IS DIFFERENT!\n");
            }
            printf("------------------------------------\n");
        } else {
            printf("\n[CANCELLED] Check aborted.\n");
        }
    } else {
        printf("[ERROR] Invalid index.\n");
    }
}

void runDelete() {
    if (fileCount == 0) {
        printf("\n[!] Base is empty. Nothing to delete.\n");
        return;
    }
    displayBase(); 
    
    int choice;
    if (!getIndexInput("Select file index to DELETE (or press ENTER to cancel): ", &choice)) {
        printf("Deletion cancelled.\n");
        return;
    }
    
    int idx = choice - 1;
    if (idx >= 0 && idx < fileCount) {
        printf("\n[CONFIRM] Deleting file: %s\n", fileBase[idx].fileName);
        for (int i = idx; i < fileCount - 1; i++) fileBase[i] = fileBase[i + 1];
        fileCount--; 
        saveUserBase();
        printf("[SUCCESS] File removed from base.\n");
    } else {
        printf("[ERROR] Invalid index.\n");
    }
}
