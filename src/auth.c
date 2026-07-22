#include "auth.h"
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void computeMD5(const char *input, char *output) {
    uint8_t digest[16];
    md5((uint8_t*)input, strlen(input), digest);
    for (int i = 0; i < 16; i++) {
        sprintf(output + (i * 2), "%02x", digest[i]);
    }
}

// ฟังก์ชันรับข้อมูล: มีระบบแจ้งเตือนเมื่อเกินกำหนด วนลูปให้กรอกใหม่ และกด Enter เพื่อยกเลิกได้
static int getAuthInput(const char *prompt, char *buffer, int maxLen) {
    while (1) {
        printf("%s", prompt);
        
        // อ่านความยาว maxLen + 2 เพื่อเผื่อที่ให้ \n และ \0
        if (fgets(buffer, maxLen + 2, stdin) != NULL) {
            if (buffer[0] == '\n') return 0; // กด Enter ว่างๆ เพื่อยกเลิก

            // เช็คว่ามีปุ่ม Enter (\n) ติดมาในบัฟเฟอร์หรือไม่
            // ถ้าไม่มี แปลว่าผู้ใช้พิมพ์ยาวเกินโควต้า maxLen
            if (!strchr(buffer, '\n')) {
                int c; while ((c = getchar()) != '\n' && c != EOF); // ล้างคีย์บอร์ดที่ล้น
                printf("[!] Warning: Input must not exceed %d characters. Please try again.\n", maxLen);
                continue; // วนลูปกลับไปให้กรอกบรรทัดนี้ใหม่
            }

            buffer[strcspn(buffer, "\r\n")] = 0; // ตัด \n ทิ้ง
            if (strlen(buffer) == 0) return 0;
            return 1; // กรอกถูกต้อง ส่งคืนค่า 1
        }
        return 0; // กรณี Error หรือ EOF
    }
}

int registerUser() {
    User newUser;
    char confirmPass[35];

    printf("\n--- User Registration ---\n");

    // ใช้ getAuthInput แทนการใช้ printf และ fgets แยกกัน
    if (!getAuthInput("Username (Max 30 chars, press ENTER to cancel): ", newUser.username, 30)) return 0;
    if (!getAuthInput("Password (Max 30 chars, press ENTER to cancel): ", newUser.password, 30)) return 0;
    if (!getAuthInput("Confirm Password (press ENTER to cancel): ", confirmPass, 30)) return 0;

    // เช็ค Confirm Password
    if (strcmp(newUser.password, confirmPass) != 0) {
        printf("\n[ERROR] Passwords do not match! Registration failed.\n");
        return 0;
    }

    FILE *file = fopen("users.db", "a");
    if (file == NULL) return 0;

    char passwordHash[33] = {0};
    computeMD5(newUser.password, passwordHash);

    // บันทึกแค่ Username และ Password Hash
    fprintf(file, "%s %s\n", newUser.username, passwordHash);
    
    fclose(file);
    printf("\n[SUCCESS] Registered successfully with MD5 Protection!\n");
    return 1;
}

int loginUser(char *loggedUser) {
    char userIn[35], passIn[35];
    char fUser[35], fPassHash[33];

    printf("\n--- Login ---\n");
    
    if (!getAuthInput("Username (press ENTER to cancel): ", userIn, 30)) return -1;
    if (!getAuthInput("Password (press ENTER to cancel): ", passIn, 30)) return -1;

    char passInHash[33] = {0};
    computeMD5(passIn, passInHash);

    FILE *file = fopen("users.db", "r");
    if (file == NULL) {
        printf("\n[ERROR] No users registered yet.\n");
        return 0;
    }

    // อ่านข้อมูลแค่ออกมา 2 ฟิลด์
    while (fscanf(file, "%s %s", fUser, fPassHash) != EOF) {
        if (strcmp(userIn, fUser) == 0 && strcmp(passInHash, fPassHash) == 0) {
            strcpy(loggedUser, fUser);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
