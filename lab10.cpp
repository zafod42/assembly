#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <conio.h>

struct Procstats //struktura soderjashaya kontext zadachi
{
    unsigned int rax, rbx, rcx, rdx; // 0  2  4  6
    unsigned int rsi, rdi, rbp, rsp; // 8 10 12 14
    unsigned int rcs, rds, res, rss; //16 18 20 22
    unsigned int rip, rflags, a, b;  //24 26 28 30
} stats[3]; //massiv iz dvuh takih struktur

int prior[3] = {10, 2, 4};

void interrupt (*oldkbhit)(...);
void interrupt kbhit(...)
{
	asm xor ax, ax
	asm in al, 0x60
	asm mov	bl, al
	asm mov ax, 0x20
	asm out 0x20, ax
	asm cmp bl, 1
	asm jne end
	asm mov ax, 0x4c00
	asm int 0x21
end:
	oldkbhit();
}

unsigned int cp = 0;
unsigned int current_proc; //nomer tekushey zadachi
unsigned int stats_offset; //adres konteksta zadachi
int x;
//int scanCode = 0;
unsigned int next_proc = 0;
int max_prior = 0;
void interrupt (*oldHandler)(...); //ukazatel na staryi obrabotchik preryvaniya

void interrupt IntHandler(...) //svoy obrabotchik
{
	next_proc = 0;
	max_prior = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (prior[i] > max_prior)
		{
			next_proc = i;
			max_prior = prior[i];
		}
	}
	if (next_proc == cp)
	{
		prior[cp] -= 1;
		prior[(cp + 1) % 3] += 1;
		prior[(cp + 2) % 3] += 1;
		//printf("%d %d %d\n", cp, (cp + 1) % 3, (cp + 2) % 3);
	       //	oldHandler();
	}
	//printf("%d %d %d\t next: %d\t current: %d\n",
	//	prior[0], prior[1], prior[2], next_proc, cp);

	asm {
		mov  si, [current_proc]; //nomer procedury
		mov  cl, 5 //umnojaem nar 32 (razmer struktury)
		shl  si, cl
		//mov  ax, offset stats
		mov  ax, [stats_offset] //pribavlyaem adres nachala massiva struktur
		add  si, ax  //poluchaem adres nujnogo konteksta
		pop  ax      //zapisyvaem v nego registry
		mov  [si+12], ax  //bp
		pop  ax
		mov  [si+10], ax  //di
		pop  ax
		mov  [si+ 8], ax  //si
		pop  ax
		mov  [si+18], ax  //ds
		pop  ax
		mov  [si+20], ax  //es
		pop  ax
		mov  [si+ 6], ax  //dx
		pop  ax
		mov  [si+ 4], ax  //cx
		pop  ax
		mov  [si+ 2], ax  //bx
		pop  ax
		mov  [si+ 0], ax  //ax
		pop  ax
		mov  [si+24], ax  //ip
		pop  ax
		mov  [si+16], ax  //cs
		pop  ax
		mov  [si+26], ax  //flags
		mov  ax, sp
		mov  [si+14], ax  //sp
		mov  ax, ss
		mov  [si+22], ax  //ss
	}
	if (next_proc != cp)
	{
		cp = next_proc;
		//printf("%d - next\n", cp);
		current_proc = cp;
	}/*
	asm {
		mov  ax, [current_proc] //perehodim k sleduyushey zadache
		inc ax
		cmp ax, 3
		jb ax_is_less_than_3
		mov	ax, 0
	}
ax_is_less_than_3:
*/
	asm {
		mov ax, [current_proc]
		//vychislyaem adres eyo konteksta
		mov  si, ax
		mov  cl, 5
		shl  si, cl
		mov  ax, [stats_offset]
		add  si, ax       //zagrujaem registry
		mov  ax, [si+22]  //ss
		mov  ss, ax
		mov  ax, [si+14]  //sp
		mov  sp, ax
		mov  ax, [si+26]  //flags
		push ax
		mov  ax, [si+16]  //cs
		push ax
		mov  ax, [si+24]  //ip
		push ax
		mov  ax, [si+ 0]  //ax
		push ax
		mov  ax, [si+ 2]  //bx
		push ax
		mov  ax, [si+ 4]  //cx
		push ax
		mov  ax, [si+ 6]  //dx
		push ax
		mov  ax, [si+20]  //es
		push ax
		mov  ax, [si+18]  //ds
		push ax
		mov  ax, [si+ 8]  //si
		push ax
		mov  ax, [si+10]  //di
		push ax
		mov  ax, [si+12]  //bp
		push ax
	}
	oldHandler(); //vyzyvaem staryi obrabotchik
}

void proc1(void);
void proc2(void);
void proc3(void);

int main(void)
{
	clrscr();
	unsigned int i, rd, rc, re, rs, ri0, ri1, ri2, f;

	for (i = 0; i < 3; i++)
		memset(stats + i, 0, sizeof(Procstats));
	asm {   //zapisyvaem registry vo vremennye peremennye
		mov  ax, cs
		mov  [rc], ax
		mov  ax, ds
		mov  [rd], ax
		mov  ax, es
		mov  [re], ax
		mov  ax, ss
		mov  [rs], ax
		mov  [ri0], offset proc1 //adresa nachala procedur
		mov  [ri1], offset proc2
		mov  [ri2], offset proc3
		pushf
		pop  ax
		mov  [f], ax
	}
	for (i = 0; i < 3; i++) //zapisyvaem znacheniya segmentnyh registrov,
	{                //flagov
		stats[i].rcs = rc;  //i adresa nacha procedur
		stats[i].rds = rd;  //v sootvetstvuyushye konteksty
		stats[i].res = re;
		stats[i].rss = rs;
		stats[i].rflags = f;
	}
	stats[0].rip = ri0;
	stats[1].rip = ri1;
	stats[2].rip = ri2;
	oldkbhit = getvect(0x9);
	setvect(0x9, kbhit);

	current_proc = 0; //tekushaya procedura - pervaya
	stats_offset = (unsigned)stats; //adres nachala massiva
	oldHandler = getvect(0x8); //perekluchaemsya po najatiyu klavishy
	setvect(0x8, IntHandler);
	proc1();
	setvect(0x8, oldHandler);
	return 0;
}

void proc1(void)
{
	while(1)
    {
		disable();
		printf("%d\n", 1);
		delay(100);
		enable();
	}
}

void proc2(void)
{
	while(1)
    {
		disable();
		printf("%d\n", 2);
		delay(100);
		enable();
	}
}

void proc3(void)
{
	while(1) 
    {
		disable();
		printf("%d\n", 3);
		delay(100);
		enable();
	}
}
