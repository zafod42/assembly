//      "Обмен IBM PC с клавиатурой"
//             Labv10

#include <iostream.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>
#define Port8042 0x60

long far * pTime=(long far*)0x46C;   // Указатель на счетчик тиков

// Определим указатели начала и конца буфера

int scanCode = 0;

void interrupt (*old)(...);
void interrupt keyb(...)
{
 asm in al, 0x60
 asm lea di, scanCode
 asm mov [di], al
 setvect(9, old);
}

int far * pHeadPtr=(int far  *)0x41A;   // Указатель на указатель головы буф.кл.
int far * pTailPtr=(int far  *)0x41C;   // Указатель на указатель хвоста буф.кл.
unsigned char far * pBuf;

void main()
{
// 3адание N1.
  clrscr();

  cout << "\n 3адание N1:\n";
  cout << "\n Нажимайте клавиши для получения кодов!";
  cout << "\n Пробел - идем дальше.\n";
  char cScan,cAsci;
  int iAdres;
  int status;
  do
  {
   // Загрузить пару регистров, например ds:si значением 0x40:0x1A
	asm{
		push ds 	//
		push si         //  Сохранили регистры
		push di         //
		mov ax,0x40     //
		mov ds,ax       //   Загрузили сегмент
		mov si,0x1A     // и указатели на голову
		mov di,0x1C     // и на хвост
		}
	wait_kbd:
	asm{
	   mov ax,[si]     //   Сравниваем указатели -
	   cmp ax,[di]     // т.е. ждем нажатия
	   je wait_kbd
	// Загрузить регистр di значением 0x1C
	// Сравнить указатели
	// Прочитать указатель
	   mov si,[si]
	   }
	iAdres=_SI;
	// Прочитать значение из буфера
	asm mov ax,[si]
	// Теперь _AL и _AH содержат скан-код и ASCII-код
	asm mov si, 0x17
	asm mov bx, [si]
	asm mov si, 0x1A

	asm pop di
	asm pop si
	asm pop ds
	cScan=_AH;
	cAsci=_AL;
	status = _BL;
	printf("Адрес = %x   Скан = %d      ASCII = %d \n",iAdres,cScan,cAsci);
	printf("Status? = %x\n ", status);
  } while(getch() != 32);
  cout << "\n Конец первого задания. Press any key...\n";
  getch();

       delay(1000);
	 asm in al,0x60
	 cScan=0; // ??????
	 printf(" Скан = %x \n",cScan);
//	 goto met;

printf("Reading scancodes from 0x60(ignoring autorepeat)\n");
getch();
cScan = 0;
int pSC = 1;
while(scanCode != 1)
{
 pSC = scanCode;
 asm in al, 0x60
 asm lea di, scanCode
 asm mov [di], al
 delay(5);
 if (scanCode != pSC)
  printf("scancode: %x\n", scanCode);
}
 printf("Block keyboard\n");
 getch();
 int start = *pTime;
 asm in al, 0x61
 asm or al, 0x80
 asm out 0x61, al
 while(*pTime - start < 10000) {
 }
 asm in al, 0x61
 asm xor al, 0x80
 asm out 0x61, al
 getch();
 getch();

//met:
 //getch();
// asm{
//  in ax, 0x60
//  lea di, cScan
//  mov [di], ax
// }
// printf("Scan code = %x\n", cScan);
// if (cScan != 1)
// {
 // asm jmp met
// }

//
}
// Задание
// 1 Разберите представленную программу
//  Определите отличие ASCII кодов: основных и служебных клавиш
// 2 Напишите мини программу циклического чтения скан кодов из порта 60
//  Определите отличие скан кодов: нажатия и отпускания клавиши
