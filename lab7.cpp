#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

int main()
{
 clrscr();
 float ad = 1.12;
 float bb = 2.2;
 cout << "Enter A: ";
 cin >> ad;
 cout << "Enter B: ";
 cin >> bb;
 asm{
  finit
  fld ad
  fld bb
  fadd
  fstp bb

 }
 cout << "A + B = " << bb << endl;
 getch();
 cout << "\nGeneratin NaN\n";
 float nan = 9.999e38;
 float nan2 = -9.999e38;
 int CW1 = 0;
 int SW1 = 0;
 printf("%f + %f = ", nan, nan2);
 asm {
  finit
  fld nan
  fld nan2
  fadd     // changes to faddp ==> st(1) = st(1) + st(0); pop st(0)
  fst nan
  fstcw CW1
  fstsw SW1
 }
 printf("%f\n", nan);
 cout.unsetf(ios::dec);
 cout.setf(ios::hex);
 //
 //	SW = 0x3801 = 0011100000000001;
 //		      |top|        |I|
 //	top = 7, no ops in stack
 //	I = 1 => invalid operation occured
 //
 // 		STATE REGISTER
 // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 //  B| C| T  O  P| C| C| C| E| S| P| U| O| Z| D| I
 //   | 3|        | 2| 1| 0| S| F| E| E| E| E| E| E
 // B - busy
 // TOP - top of stack pointer
 // C0, C1, C2, C3 - states
 // ES - err summary status
 // SF - Stack fault
 // P, U, O, Z, D, I
 // P - precision
 // U - undreflow
 // O - overflow
 // Z - div by 0
 // D - denormalization
 // I - invalid operation(stack or arithm)
 //
 //
 // 	CW = 0x037f = 0000001101111111
 //		           |PC|MaskAll|
 // 		Every exception is masked
 //	    PC = 11 ==> Extended precision
 //
 //		CONTROL REGISTER
 // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 //  x| x| x| x| R C | P C | x| x|PM|UM|OM|ZM|DM|IM
 //
 // at 13 - inf control
 // RC - rounding control
 // PC - precision control
 // Marks:
 // 	P - precision
 //     U - underflow
 //	O - overflow
 // 	Z - div by 0
 //	D - denormalized
 //	I - invalid op
 //
 cout << "Control Regiser: " <<  CW1 << endl;
 cout << "State Regiser: " << SW1 << endl;
 cout.unsetf(ios::hex);
 cout.setf(ios::dec);
 getch();

 cout << "\nGenerating wrong operation exception\n"; // reading from empty stack
 int x87CW = 0x037E;
 float st1 = 9;
 asm {
  finit
  fldcw x87CW
  fst st1
 }
 printf("Result: %f\n", st1);
 getch();
 return 0;
}