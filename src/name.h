#ifndef NAME_H
#define NAME_H

#include <stdio.h>
#include <time.h>

// เติม static inline เพื่อป้องกันปัญหา symbol ซ้ำ
static inline void printHeader() {
    printf("  ██████╗ ███████╗██╗     ██╗     \n");
    printf("  ██╔══██╗██╔════╝██║     ██║     \n");
    printf("  ██████╔╝█████╗  ██║     ██║     \n");
    printf("  ██╔══██╗██╔══╝  ██║     ██║     \n");
    printf("  ██████╔╝███████╗███████╗███████╗\n");
    printf("  ╚═════╝ ╚══════╝╚══════╝╚══════╝\n");
        printf("    ██████╗ ██╗  ██╗███████╗ ██████╗ ██╗  ██╗███████╗██████╗ \n");
    printf("   ██╔════╝ ██║  ██║██╔════╝██╔════╝ ██║ ██╔╝██╔════╝██╔══██╗\n");
    printf("   ██║      ███████║█████╗  ██║      █████╔╝ █████╗  ██████╔╝\n");
    printf("   ██║      ██╔══██║██╔══╝  ██║      ██╔═██╗ ██╔══╝  ██╔══██╗\n");
    printf("   ╚██████╗ ██║  ██║███████╗╚██████╗ ██║  ██╗███████╗██║  ██║\n");
    printf("    ╚═════╝ ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝\n");
}

static inline void printDivider() {
    printf("======================================================================\n");
}

static inline void printStatus() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[SYSTEM: 5.8-SMART-INPUT] | [SECURITY: SMART INPUT ENABLED]\n");
    printDivider();
}

#endif
