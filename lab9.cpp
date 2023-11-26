//      "����� IBM PC � ��������ன"
//             Labv10

#include <iostream.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>
#define Port8042 0x60

long far * pTime=(long far*)0x46C;   // �����⥫� �� ���稪 ⨪��

// ��।���� 㪠��⥫� ��砫� � ���� ����

int scanCode = 0;

void interrupt (*old)(...);
void interrupt keyb(...)
{
 asm in al, 0x60
 asm lea di, scanCode
 asm mov [di], al
 setvect(9, old);
}

int far * pHeadPtr=(int far  *)0x41A;   // �����⥫� �� 㪠��⥫� ������ ���.��.
int far * pTailPtr=(int far  *)0x41C;   // �����⥫� �� 㪠��⥫� 墮�� ���.��.
unsigned char far * pBuf;

void main()
{
// 3������ N1.
  clrscr();

  cout << "\n 3������ N1:\n";
  cout << "\n �������� ������ ��� ����祭�� �����!";
  cout << "\n �஡�� - ���� �����.\n";
  char cScan,cAsci;
  int iAdres;
  int status;
  do
  {
   // ����㧨�� ���� ॣ���஢, ���ਬ�� ds:si ���祭��� 0x40:0x1A
	asm{
		push ds 	//
		push si         //  ���࠭��� ॣ�����
		push di         //
		mov ax,0x40     //
		mov ds,ax       //   ����㧨�� ᥣ����
		mov si,0x1A     // � 㪠��⥫� �� ������
		mov di,0x1C     // � �� 墮��
		}
	wait_kbd:
	asm{
	   mov ax,[si]     //   �ࠢ������ 㪠��⥫� -
	   cmp ax,[di]     // �.�. ���� ������
	   je wait_kbd
	// ����㧨�� ॣ���� di ���祭��� 0x1C
	// �ࠢ���� 㪠��⥫�
	// ������ 㪠��⥫�
	   mov si,[si]
	   }
	iAdres=_SI;
	// ������ ���祭�� �� ����
	asm mov ax,[si]
	// ������ _AL � _AH ᮤ�ঠ� ᪠�-��� � ASCII-���
	asm mov si, 0x17
	asm mov bx, [si]
	asm mov si, 0x1A

	asm pop di
	asm pop si
	asm pop ds
	cScan=_AH;
	cAsci=_AL;
	status = _BL;
	printf("���� = %x   ���� = %d      ASCII = %d \n",iAdres,cScan,cAsci);
	printf("Status? = %x\n ", status);
  } while(getch() != 32);
  cout << "\n ����� ��ࢮ�� �������. Press any key...\n";
  getch();

       delay(1000);
	 asm in al,0x60
	 cScan=0; // ??????
	 printf(" ���� = %x \n",cScan);
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
// �������
// 1 ������� �।�⠢������ �ணࠬ��
//  ��।���� �⫨稥 ASCII �����: �᭮���� � �㦥���� ������
// 2 ������ ���� �ணࠬ�� 横���᪮�� �⥭�� ᪠� ����� �� ���� 60
//  ��।���� �⫨稥 ᪠� �����: ������ � ���᪠��� ������
