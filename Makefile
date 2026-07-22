# 1. ชื่อโปรแกรมที่ต้องการสร้าง
TARGET = bellchecker7

# 2. คอมไพเลอร์และ Flags
CC = gcc
CFLAGS = -I. -Wall

# 3. รายชื่อไฟล์ .c ทั้งหมด (อัปเดตตามการรวมโมดูลใหม่)
# ตัด register.c, login.c, base.c, check.c, delete.c ออก
# แทนที่ด้วย auth.c และ manager.c
SRCS = main.c \
       md5.c \
       upload.c \
       auth.c \
       manager.c

# 4. แปลงชื่อไฟล์ .c เป็น .o สำหรับการ Link
OBJS = $(SRCS:.c=.o)

# 5. ตรวจสอบระบบปฏิบัติการ (สำหรับรองรับทั้ง Mac และ Windows)
ifeq ($(OS),Windows_NT)
    EXE = $(TARGET).exe
    RM = del /Q
else
    EXE = $(TARGET)
    RM = rm -f
endif

# --- กฎการทำงาน (Rules) ---

# คำสั่งหลัก: พิมพ์ 'make' เพื่อคอมไพล์
all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(CFLAGS)
	@echo "------------------------------------------------"
	@echo "Build Successful: $(EXE)"
	@echo "------------------------------------------------"

# กฎในการสร้างไฟล์ .o จาก .c แต่ละไฟล์
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# คำสั่ง 'make clean': ล้างไฟล์ที่คอมไพล์ค้างไว้
clean:
	$(RM) *.o $(EXE)
	@echo "Cleaned up all build files."

# คำสั่ง 'make run': คอมไพล์เสร็จแล้วสั่งรันทันที
run: all
	./$(EXE)