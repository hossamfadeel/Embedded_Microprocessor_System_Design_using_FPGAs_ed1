%% Script Name: h2v.m
%% Date: 5/2/2019 
%% About this project: This MatLab script converts the pixel font *.h file
%%                    it to VHDL code constant ROM style for FPGAs
%%                    Char 0x05 (R); Char 0x06 (C); and Char 0x7F (u) have 
%%                    been changed compared to the original font
%% (c) 2019 by Uwe Meyer-Baese 

clear all; close all;
fin='myAPEAUS.h'
fout='char_table.vhd'
disp('Pick up the first 128x8=1024 hex values from a file')
fin=fopen(fin,'r');
fout=fopen(fout,'w');

fprintf(fout,'-- File generated with h2vhdl.m  DO NOT EDIT THIS FILE!\n');
fprintf(fout,'--------------------------------------------------------------------\n');
fprintf(fout,'LIBRARY ieee;\n');
fprintf(fout,'USE ieee.STD_LOGIC_1164.ALL;\n');
fprintf(fout,'USE ieee.STD_LOGIC_arith.ALL;\n');
fprintf(fout,'USE ieee.STD_LOGIC_unsigned.ALL;\n');
fprintf(fout,'-- -----------------------------------------------------\n');
fprintf(fout,'ENTITY char_rom IS\n');
fprintf(fout,'PORT (clk      : IN STD_LOGIC;      -- System clock\n');
fprintf(fout,'      address  : IN STD_LOGIC_VECTOR(6 DOWNTO 0); -- ROM address\n');
fprintf(fout,'      row, col : IN STD_LOGIC_VECTOR(2 DOWNTO 0); -- x/y with char\n');
fprintf(fout,'      q        : OUT STD_LOGIC); -- ROM data\n');
fprintf(fout,'END ENTITY;\n');
fprintf(fout,'-- -----------------------------------------------------\n');
fprintf(fout,'ARCHITECTURE fpga OF char_rom IS\n');
fprintf(fout,'-- Program ROM definition and values\n');
fprintf(fout,'TYPE MEMP IS ARRAY (0 TO 4799) OF STD_LOGIC_VECTOR(7 DOWNTO 0);\n');
fprintf(fout,'CONSTANT prom : MEMP := (\n');

line=upper(fgets(fin));
num=0;
while ischar(line) & num < 1024
    L=length(line);
    for k=1:L-4
      if (line(k)=='0') & line(k+1)=='X'
       str=sprintf('%s',dec2bin(hex2dec(line(k+2)),4));
       str=sprintf('%s%s',str,dec2bin(hex2dec(line(k+3)),4));
       %disp(str);
       fprintf(fout,'\"%s\",\n',str);
       num = num + 1;
      end
    end
    line=upper(fgets(fin));
end
fprintf(fout,'OTHERS => \"00000000\");\n');
fprintf(fout,'  SIGNAL word : STD_LOGIC_VECTOR(7 DOWNTO 0);\n');
fprintf(fout,'  SIGNAL address_row : STD_LOGIC_VECTOR(9 DOWNTO 0);\n');
fprintf(fout,'  SIGNAL col1, col2 : STD_LOGIC_VECTOR(2 DOWNTO 0);\n');

fprintf(fout,'\n');
fprintf(fout,'BEGIN\n');
fprintf(fout,'-- The following process will be mapped to a BlockRAM for Zynq/Virtex/Spartan\n');
fprintf(fout,'-- devices (if addr > 4 bit!) and to Distributed RAM in all other cases\n');
fprintf(fout,'\n');
fprintf(fout,'address_row <= address & row;\n');
fprintf(fout,'\n');
fprintf(fout,'rom: PROCESS (clk)\n');
fprintf(fout,'BEGIN\n');
fprintf(fout,'  IF rising_edge(clk) THEN\n');
fprintf(fout,'     word <= prom(CONV_INTEGER(address_row)); -- Read from ROM\n');
fprintf(fout,'     col1 <= col;\n');
fprintf(fout,'     col2 <= col1;\n');
fprintf(fout,'  END IF;\n');
fprintf(fout,'END PROCESS;\n');
fprintf(fout,'\n');
fprintf(fout,'q <= word(CONV_INTEGER(NOT col2)); \n');
fprintf(fout,'\n');
fprintf(fout,'END ARCHITECTURE fpga;\n');

fclose(fin);fclose(fout);
str=sprintf('Saving = %d x8 binary for VHDL',num)

