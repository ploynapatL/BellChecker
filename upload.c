#include "upload.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void uploadFileSilent(char *outPath, int maxLen) {
    printf("\n>> Please enter or Drag & Drop the file path here \n");
    printf(">> (Or just press ENTER to cancel): ");
    
    if (fgets(outPath, maxLen, stdin) != NULL) {
        outPath[strcspn(outPath, "\r\n")] = 0; // ตัด Enter ทิ้ง
        
        if (strlen(outPath) == 0) {
            return; // ผู้ใช้กด Enter ทิ้ง = ยกเลิกการทำงาน
        }

        // 1. Clean-up: ล้างพวก Spacebar หรือเครื่องหมาย " ที่ติดมาตอนลากไฟล์
        int start = 0;
        while (outPath[start] == ' ' || outPath[start] == '"' || outPath[start] == '\'') start++;
        int end = strlen(outPath) - 1;
        while (end >= start && (outPath[end] == ' ' || outPath[end] == '"' || outPath[end] == '\'')) end--;
        
        int len = end - start + 1;
        if (len > 0) {
            memmove(outPath, outPath + start, len);
            outPath[len] = '\0';
        } else {
            outPath[0] = '\0';
            return;
        }

        // 2. AUTO-CONVERT WINDOWS PATH TO WSL PATH (ฟีเจอร์ใหม่!)
        // ดักจับว่าถ้า Path เริ่มด้วย C:\ หรือ D:\ ให้แปลงเป็น /mnt/c/ หรือ /mnt/d/
        if (strlen(outPath) >= 3 && outPath[1] == ':' && outPath[2] == '\\') {
            char drive = outPath[0];
            // แปลงอักษรไดรฟ์ให้เป็นตัวพิมพ์เล็ก (เช่น C -> c)
            if (drive >= 'A' && drive <= 'Z') drive += 32; 
            
            char tempBuf[1024];
            snprintf(tempBuf, sizeof(tempBuf), "/mnt/%c/%s", drive, outPath + 3);
            
            // แปลง Backslash (\) ทั้งหมดให้เป็น Slash (/) แบบที่ Linux ชอบ
            for(int i = 0; tempBuf[i] != '\0'; i++){
                if(tempBuf[i] == '\\') tempBuf[i] = '/';
            }
            
            // เอา Path ที่แปลงเสร็จแล้วใส่กลับคืน
            strncpy(outPath, tempBuf, maxLen);
        }
    }
}

int silentCopy(const char *sourcePath) {
    if (sourcePath == NULL || strlen(sourcePath) == 0) return 0;
    
    char tempPath[1024];
    strncpy(tempPath, sourcePath, sizeof(tempPath));
    
    char *fileName = strrchr(tempPath, '\\');
    if (!fileName) fileName = strrchr(tempPath, '/');
    
    if (fileName) fileName++; else fileName = tempPath;

    FILE *src = fopen(sourcePath, "rb");
    FILE *dst = fopen(fileName, "wb");
    if (!src || !dst) {
        if (src) fclose(src);
        if (dst) fclose(dst);
        return 0;
    }

    unsigned char buffer[16384];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }
    
    fclose(src); 
    fclose(dst);
    return 1;
}