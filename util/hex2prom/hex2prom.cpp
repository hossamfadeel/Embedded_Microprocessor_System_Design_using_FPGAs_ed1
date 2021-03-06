/* Name: Uwe Meyer-Baese
Date: 6/15/2017 
About this project: This program takes HEX programs and convert 
                    it to HDL code for FPGAs
Assumptions: The user enter a valid file name that is located in the same
             directory as the program.
All coding below was performed by Uwe Meyer-Baese */ 

#define DEBUG 1
#define USEOTHERS 1

#include <iostream> // Required for I/O
#include <string>	// for string objects
#include <iomanip>  // Required for setprecision
#include <fstream>  // Required for file I/O
#include <cctype>	// for tolower()
#include <cstdlib>	// for srand(), rand()
#include <cstring>	// for strlen()

using namespace std;

//User function
void hex2bin(char c, char str[3]);
void hex4bin(char c, char str[5]);
void i4psm(int i, char psm[20]);
int h2i(char c);

int main(int argc, char *argv[]) {
	char c, str[80], infile[20], outfile[20], voutfile[20], psm[20];
	char bin0[5], bin1[5], bin2[5], bin3[5], bin4[5];
	int k, i, j;
	bool vhdl2008 = false;

	cout << "argc = " << argc << endl;
	for (i = 0; i < argc; i++) {
		cout << "arg[" << i << "]= " << argv[i] << endl;
	}

	cout << "Copyright (C) 2019 Springer Press.\n";
	cout << "Utility program" << argv[0] << " for the book:\n";
	cout << "EuP System Design with FPGAs\n";
	cout << "      by Dr. Uwe Meyer-Baese\n";
	cout << "************************************\n";
	if (argc == 1) {
	cout << "No file specified ... good bye;\nEnter any char to finish\n";
	cin >> c;
	return(0);
    }
/////////////// First part read the file /////////////////////////
  ifstream fin;
  if (argc>1) {
	  strcpy(str, argv[1]);k=0;j=0;
	  while (str[k]!='\0') {
		  infile[j++]=str[k++];
		  if ((str[k-1]=='\\') || (str[k-1]=='/')) j=0;
	  }
	  infile[j++]='\0';
     cout << "Try to open = " << infile << endl;
	  fin.open(infile);
    if (!fin) { cerr << "Attempt to open file failed\n"; }
  } 
  if (!fin)
  do {	
    cout << "Please enter the name of the input file.\n";
    cout << "Filename: "; 
    cin >> infile;
    fin.open(infile);
    if (!fin) { cerr << "Attempt to open file failed\n"; }
   } while (!fin);

   k=0; // Next determine the VHDL file name
   while (infile[k]!='.') outfile[k]=infile[k++];
   outfile[k++]='.'; outfile[k++]='v';
   outfile[k++]='h'; outfile[k++]='d'; outfile[k++]='\0';
   k=0; // Next determine the Verilog file name
   while (infile[k]!='.') voutfile[k]=infile[k++];
   voutfile[k++]='.'; voutfile[k++]='v'; voutfile[k++]='\0';

   ofstream fout;  // Cerate the VHDL file
   cout << "********* Now writing fout: " << outfile << endl;
   fout.open(outfile);
   if (!fout){ cerr << "Attempt to create file failed\n"; exit(1);}

  // ofstream vout;  // Cerate the Verilog file
  // cout << "********* Now writing vout: " << voutfile << endl;
  // vout.open(voutfile);
  // if (!vout){ cerr << "Attempt to create file failed\n"; exit(1);}

  fout << "-- File generated with hex2prom.exe  DO NOT EDIT THIS FILE!\n";
  fout << "-- Copyright (C) 2017  Dr. Uwe Meyer-Baese.\n";
  fout << "--------------------------------------------------------------------\n";
  fout << "LIBRARY ieee;\n";
  fout << "USE ieee.STD_LOGIC_1164.ALL;\n";
  fout << "USE ieee.STD_LOGIC_arith.ALL;\n";
  fout << "USE ieee.STD_LOGIC_unsigned.ALL;\n";
  fout << "-- -----------------------------------------------------\n";
  fout << "ENTITY rom4096x18 IS\n";
  fout << "PORT (clk   : IN STD_LOGIC;      -- System clock\n";
  fout << "      reset : IN STD_LOGIC;      -- Asynchronous reset\n";
  fout << "      pma   : IN STD_LOGIC_VECTOR(11 DOWNTO 0); -- Program memory address\n";
  fout << "      pmd   : OUT STD_LOGIC_VECTOR(17 DOWNTO 0)); -- Program memory data\n";
  fout << "END ENTITY rom4096x18;\n";
  fout << "-- -----------------------------------------------------\n";
  fout << "ARCHITECTURE fpga OF rom4096x18 IS\n";
  fout << "-- Program ROM definition and values\n";
  fout << "TYPE MEMP IS ARRAY (0 TO 4095) OF STD_LOGIC_VECTOR(17 DOWNTO 0);\n";
  fout << "CONSTANT prom : MEMP := (\n";
  
  k=1;
   fin.getline(str, 80);	// reads an entire line (up to newline)
   while ((!fin.eof()) && (k<=4095)) {
	 if (vhdl2008) {
     if (strncmp (str,"00000",5) != 0) cout << "18X\"" << str << "\"," << endl;
	 if (strncmp (str,"00000",5) != 0) {fout << "18X\"" << str << "\", ";k++;}
     if ((k%5)==0) fout << endl;
	 } else {
		 if ((strncmp (str,"00000",5) != 0) || (USEOTHERS==0)) { 
			 hex2bin(str[0],bin0);
			 cout << setw( 2 ) << k-1 << ") " << str << " = " << bin0 << '_';
			 hex4bin(str[1],bin1); cout << bin1 << '_';
			 hex4bin(str[2],bin2); cout << bin2 << '_';
			 hex4bin(str[3],bin3); cout << bin3 << '_';
			 hex4bin(str[4],bin4); cout << bin4 ;
			 i = h2i(str[0])*16 + h2i(str[1]); i4psm(i, psm);
			 cout << " = " << psm << endl;
			 fout << "\"" << bin0 << bin1 << bin2 << bin3 << bin4 << "\", ";
			 if ((k%3)==0) fout << endl; k++;
		 }
	 }
     fin.getline(str, 80);	// reads an entire line (up to newline)
   }
   if (vhdl2008)
     fout << "OTHERS => X\"22000\");\n";
   else
	 fout << "OTHERS => \"100010000000000000\");\n";
   fout << "\nBEGIN\n";
  fout << "-- The following process will be mapped to a BlockRAM for Zynq/Virtex/Spartan\n";
  fout << "-- devices (if addr > 4 bit!) and to Distributed RAM in all other cases\n\n";
  fout << "prog_rom: PROCESS (reset, clk)\n";
  fout << "BEGIN\n";
  fout << "  IF reset = '0' THEN               -- Asynchronous clear\n";
  fout << "    pmd <= (OTHERS => '0');     \n";
  fout << "  ELSIF rising_edge(clk) THEN\n";
  fout << "      pmd <= prom(CONV_INTEGER(pma)); -- Read from ROM\n";
  fout << "  END IF;\n";
  fout << "END PROCESS;\n\n";
  fout << "END ARCHITECTURE fpga;\n";
  fout.close();   
  // vout.close();
/////////////// Now generate the Verilog file /////////////////////////
   cout << "Processing complete;\nEnter any char to finish\n";
   cin >> c;
}

/************* conversion hex to 4 bit binary ***********/
void hex4bin(char c, char str[5]) {   
  switch (c) {
    case '0': strcpy(str,"0000\0"); break;
    case '1': strcpy(str,"0001\0"); break;
    case '2': strcpy(str,"0010\0"); break;
    case '3': strcpy(str,"0011\0"); break;
    case '4': strcpy(str,"0100\0"); break;
    case '5': strcpy(str,"0101\0"); break;
    case '6': strcpy(str,"0110\0"); break;
    case '7': strcpy(str,"0111\0"); break;
    case '8': strcpy(str,"1000\0"); break;
    case '9': strcpy(str,"1001\0"); break;
    case 'A': case 'a': strcpy(str,"1010\0"); break;
    case 'B': case 'b': strcpy(str,"1011\0"); break;
    case 'C': case 'c': strcpy(str,"1100\0"); break;
    case 'D': case 'd': strcpy(str,"1101\0"); break;
    case 'E': case 'e': strcpy(str,"1110\0"); break;
    case 'F': case 'f': strcpy(str,"1111\0"); break;
    default : strcpy(str,"0000\0"); break;
  }
}

/************* conversion hex to 2 bit binary ***********/
void hex2bin(char c, char str[3]) {   
  switch (c) {
    case '0': strcpy(str,"00\0"); break;
    case '1': strcpy(str,"01\0"); break;
    case '2': strcpy(str,"10\0"); break;
    case '3': strcpy(str,"11\0"); break;
    default : strcpy(str,"00\0"); break;
  }
}

/************* conversion int to asm code ***********/
void i4psm(int i, char psm[20]) {   
  switch (i) {
    case 0x00: strcpy(psm," LOAD sX, sY\0"); break;
    case 0x01: strcpy(psm," LOAD sX, kk\0"); break;
    case 0x02: strcpy(psm," AND sX, sY\0"); break;
    case 0x03: strcpy(psm," AND sX, kk\0"); break;
    case 0x04: strcpy(psm," OR  sX, sY\0"); break;
    case 0x05: strcpy(psm," OR  sX, kk\0"); break;
    case 0x06: strcpy(psm," XOR sX, sY\0"); break;
    case 0x07: strcpy(psm," XOR sX, kk\0"); break;
    case 0x08: case 0x09: strcpy(psm," INPUT\0"); break;    
    case 0x0A: strcpy(psm," FETCH sX, (sY)\0"); break;
    case 0x0B: strcpy(psm," FETCH sX, ss\0"); break;    
    case 0x10: strcpy(psm," ADD sX, sY\0"); break;
    case 0x11: strcpy(psm," ADD sX, kk\0"); break;
    case 0x12: strcpy(psm," ADDCY sX, sY\0"); break;
    case 0x13: strcpy(psm," ADDCY sX, kk\0"); break;
    case 0x18: strcpy(psm," SUB sX, sY\0"); break;
    case 0x19: strcpy(psm," SUB sX, kk\0"); break;
    case 0x1A: strcpy(psm," SUBCY sX, sY\0"); break;
    case 0x1B: strcpy(psm," SUBCY sX, kk\0"); break;
    case 0x20: strcpy(psm," CALL aaa\0"); break;
    case 0x22: strcpy(psm," JUMP aaa\0"); break;
    case 0x25: strcpy(psm," RETURN\0"); break;
    case 0x2B: case 0x2C: case 0x2D: strcpy(psm," OUTPUT\0"); break;    
    case 0x2E: strcpy(psm," STORE sX, (sY)\0"); break;
    case 0x2F: strcpy(psm," STORE sX, ss\0"); break; 
    case 0x32: strcpy(psm," JUMP Z, aaa\0"); break;
    case 0x36: strcpy(psm," JUMP NZ, aaa\0"); break;      
    default : strcpy(psm,"??? Not supported\0"); break;
  }
}
/************* conversion hex to integer value ***********/
int h2i(char c) {   
  switch (c) {
    case '1': return 1; break;
    case '2': return 2; break;
    case '3': return 3; break;
    case '4': return 4; break;
    case '5': return 5; break;
    case '6': return 6; break;
    case '7': return 7; break;
    case '8': return 8; break;
    case '9': return 9; break;
    case 'A': case 'a': return 10; break;
    case 'B': case 'b': return 11; break;
    case 'C': case 'c': return 12; break;
    case 'D': case 'd': return 13; break;
    case 'E': case 'e': return 14; break;
    case 'F': case 'f': return 15; break;
    default : return 0; break;
  }
}
