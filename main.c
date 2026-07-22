#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"
#include "name.h"
#include "auth.h"     
#include "manager.h"  

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// อัปเดตฟังก์ชัน waitEnter: ใช้ fgets เพื่อดักจับคนพิมพ์ขยะให้แม่นยำขึ้น
void waitEnter() {
    char buffer[1024];
    while (1) {
        printf("\nPress [ENTER] to continue...");
        
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // ถ้ากดแค่ Enter ตัวแรกสุด (ถูกต้อง) ให้หลุดจากลูปไปต่อ
            if (buffer[0] == '\n') {
                break; 
            } else {
                // ถ้ามีตัวอักษรอื่นพิมพ์มาก่อน (ดื้อ)
                // เช็คว่ายาวล้นบัฟเฟอร์ไหม ถ้าล้นก็ล้างทิ้ง
                if (!strchr(buffer, '\n')) {
                    int c; 
                    while ((c = getchar()) != '\n' && c != EOF);
                }
                // เด้งด่า และวนลูปให้กดใหม่!
                printf("[!] Warning: Invalid input. Please just press ENTER without typing anything else.\n");
            }
        } else {
            break; // กรณีเจอ EOF
        }
    }
}

// ฟังก์ชันรับค่าเมนู: เพิ่มระบบล้างบัฟเฟอร์เมื่อพิมพ์ยาวเกินกำหนด
int getMenuChoice() {
    char buffer[100];
    int val = -1;
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (buffer[0] == '\n') return 0; 

        if (!strchr(buffer, '\n')) {
            int c; 
            while ((c = getchar()) != '\n' && c != EOF);
        }
        sscanf(buffer, "%d", &val);
    }
    return val;
}

// ฟังก์ชัน Upload ให้มีระบบดักจับ Path
void handleUpload() {
    char selectedPath[1024] = {0};
    char generatedHash[33] = {0};

    printf("\n>>> Please provide a file to REGISTER into base...\n");

    if (getValidFilePath("Enter File Path (or press ENTER to cancel): ", selectedPath, 1000)) {
        get_hash(selectedPath, generatedHash); 
        addFileToBase(selectedPath, generatedHash);
    } else {
        printf("\n[CANCELLED] Operation aborted by user.\n");
    }
}

int main() {
    int programRunning = 1;

    // --- Outer Loop: วงจรโปรแกรมหลัก (หน้า LoginPage) ---
    while (programRunning) {
        char loggedUserName[50] = {0};
        int isAuthenticated = 0;

        // วนลูปหน้า Login จนกว่าจะเข้าสู่ระบบสำเร็จ หรือเลือก Exit
        while (!isAuthenticated) {
            clearScreen();
            printHeader();
            printStatus();

            printf("\n==== WELCOME TO SECURITY SYSTEM ====\n");
            printf("1. Login\n2. Register\n3. Exit Program\n");
            printf("Selection: ");
            
            int authChoice = getMenuChoice();

            if (authChoice == 1) {
                int loginStatus = loginUser(loggedUserName);
                if (loginStatus == 1) {
                    printf("\n[SUCCESS] Welcome, %s! Access Granted.\n", loggedUserName);
                    loadUserBase(loggedUserName); 
                    isAuthenticated = 1; 
                    waitEnter();
                } else if (loginStatus == -1) {
                    printf("\n[CANCELLED] Login aborted.\n");
                    waitEnter();
                } else {
                    printf("\n[ERROR] Invalid credentials. Please try again.\n");
                    waitEnter();
                }
            } else if (authChoice == 2) {
                registerUser();
                waitEnter();
            } else if (authChoice == 3) {
                printf("Exiting Program...\n");
                programRunning = 0; 
                break;
            } else {
                printf("[!] Invalid choice. Please select 1, 2, or 3.\n");
                waitEnter();
            }
        }

        if (!programRunning) break;

        // --- Inner Loop: เมนูหลัก (หน้า MainPage) ---
        while (isAuthenticated) {
            clearScreen();
            printf("\n====================================\n");
            printf("   USER: %s | STATUS: ONLINE\n", loggedUserName);
            printf("====================================\n");
            printf("1. Upload File (Register to Base)\n");
            printf("2. Check File Integrity (Compare)\n");
            printf("3. List All Files in Base\n");
            printf("4. Delete File from Base\n");
            printf("5. Logout\n"); 
            printf("------------------------------------\n");
            printf("Select Option: ");
            
            int mainMenuChoice = getMenuChoice();

            switch (mainMenuChoice) {
                case 1: 
                    handleUpload(); 
                    waitEnter(); 
                    break;
                case 2: 
                    runCheck(); 
                    waitEnter(); 
                    break;
                case 3: 
                    displayBase(); 
                    waitEnter(); 
                    break;
                case 4: 
                    runDelete(); 
                    waitEnter(); 
                    break;
                case 5: 
                    printf("\nLogging out... Goodbye %s!\n", loggedUserName); 
                    isAuthenticated = 0; 
                    waitEnter();
                    break;
                default: 
                    printf("\n[!] Invalid option. Please try again.\n"); 
                    waitEnter(); 
                    break;
            }
        }
    }
    
    return 0;
}