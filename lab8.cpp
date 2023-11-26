#include <dos.h>
#include <conio.h>

#include <stdio.h>
#include <iostream.h>

void interrupt(*old)(...);

int a = 1;
int handler_place = 1;

void interrupt handler(...)
{
	int address = 0;
	int AX_ = 0;
	char c;
	asm {
		mov address, sp
		mov AX_, ax
	}
	cout << "interrupt: sp = " << address << ", ax = "<< AX_ << endl;
	c = getch();
	cout << "you pressed " << c << endl;
	if (c == 'q')
	{
		disable();
		//setvect(handler_place, old);
		enable();
		cout << "stop debug" << endl;

		asm {
			mov ax, [bp + 22]
			xor ax, 100h
			mov [bp + 22], ax
		}
		//~return from interrupt handler with TF = 0~
	}
}	

void thread()
{
	asm {
	 mov cx, 2
	 mov ax, 0
	}
llop:
	asm {
	 add ax, 1
	 add ax, 1
	 
	 add ax, 1
	 mov a, ax
	}
	cout << "end of thread: a = " << a << endl;
}

void main(void)
{
	clrscr();
	cout << "start" << endl;

	old = getvect(handler_place);
	disable();
	setvect(handler_place, handler);
	enable();
	asm { 	// TF = 1
		pushf
		pop ax
		or ax, 100h
		push ax
		popf
	}
	asm {
	 mov ax, 0
	 mov cx, 3
	}
llop:
	asm {
	 add ax, 1
	 loop llop
	}
	cout << "end" << endl;
	getch();
}
