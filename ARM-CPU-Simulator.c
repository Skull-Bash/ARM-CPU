/*
 * starter.c - More help you get started with the assignment - looking at condition flags
 *
 *  Created on: Nov 12, 2012
 *      Author: mdixon
 */

// #include some headers you will be probably need
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <stdint.h>
#include <String.h>
#include <time.h>

// Defines some types of our own. note: base these on platform independent types defined in <stdint.h>
typedef uint32_t REGISTER;		// registers are 32 bits
typedef uint32_t WORD;			// words are 32 bits
typedef uint8_t BYTE;			// bytes are 8 bits

#define COND_CODE_POS (28)

// Define each condition code.
#define CC_EQ	(0x0)
#define CC_NE	(0x1)
#define CC_CS	(0x2)
#define CC_CC	(0x3)
#define CC_MI	(0x4)
#define CC_PL	(0x5)
#define CC_VS	(0x6)
#define CC_VC	(0x7)
#define CC_HI	(0x8)
#define CC_LS	(0x9)
#define CC_GE	(0xa)
#define CC_LT	(0xb)
#define CC_GT	(0xc)
#define CC_LE	(0xd)
#define CC_AL	(0xe)
#define CC_NV	(0xf)

// Define each status flag
#define STAT_N	(1<<7)
#define STAT_Z	(1<<6)
#define STAT_C	(1<<5)
#define STAT_V	(1<<4)

REGISTER registers[16];	// storing registers within array makes execution code simpler
WORD memory[1024];			// 4 kilobytes of RAM in this example (since 1 word = 4 bytes)
BYTE sr = 0;				// status register


// A function to convert binary to decimal
int binToDecimal(char* bin) {
	int dec = 0,i;
		strrev(bin);
     
    for(i = 0; i < strlen(bin);i++)
        if (((int)bin[i] - '0')% 10)
            dec += pow(2, i);
    return dec;
}

// A function to convert decimal to string
char* decimalToString(int b) {
	
	if(b == 0)
	 return "0";
	 
	char t[] = "";
	int x,i=0;
	while(b) {
		x = b % 10;
		b = b / 10;
		switch(x) {
			case 0:
				strcat(t,"0");
				break;
			case 1:
				strcat(t,"1");
				break;
			case 2:
				strcat(t,"2");
				break;
			case 3:
				strcat(t,"3");
				break;
			case 4:
				strcat(t,"4");
				break;
			case 5:
				strcat(t,"5");
				break;
			case 6:
				strcat(t,"6");
				break;
			case 7:
				strcat(t,"7");
				break;
			case 8:
				strcat(t,"8");
				break;
			case 9:
				strcat(t,"9");
				break;
			default:
				strcat(t,"");
		}
	}
	return t;
}


// Provide some support functions to test condition of a specified SR flag
int isSet(int flag)
{
	return (sr & flag);
}

int isClear(int flag)
{
	return ((~sr) & flag);
}

void setFlag(int flag)
{
	sr = sr | flag;
}

void clearFlag(int flag)
{
	sr = sr & (~flag);
}


// The main work will happen inside this function. It should decode then execute the passed instruction.
void decodeAndExecute(WORD inst)
{

	// Start by extracting the condition code

	int condCode = inst >> COND_CODE_POS;	// get the condition flags (top 4 bits of the instruction).

	int exec;

	// Decide whether the instruction should be executed by examining the appropriate status register flags, depending on condition code.

	switch ( condCode ) {
		case CC_EQ	: exec = isSet( STAT_Z ) ; 			break;	// check if zero flag is set
		case CC_NE	: exec = isClear( STAT_Z ) ;		break;	// check if zero flag is clear
		// other conditions ....

		// some are a bit more complex, e.g.
		case CC_LT	: exec = ( isSet( STAT_N ) && isClear( STAT_V ) ) || ( isClear(STAT_N) && isSet(STAT_V) ) ;	break;
		// other conditions ....

		case CC_AL	: exec = 1; break;
		case CC_NV	: exec = 0; break;
	}

	if ( exec ) {
		// only decode and execute the instruction if required to do so, i.e. condition satisfied.

		// next stage is to decode which type of instruction we have, so want to look at bits 27, 26 and 25 (see assignment tips document)
		printf("\tOp-Code\t\t\t\tAssembly Mnemonics\n");
		printf("----------------------------------------------------------------------------------------------\n");
		printf("\t%04X\t\t\t",inst);	// output the instruction being decoded

		char output[32] = "";						// To store binary representation of hex code
		long long int x = (inst >> 0);		
		long long int n;
		n = x;
		long long int r;
		do {
		r = n % 2;
		if (r == 1) strcat(output,"1");				// Converting to Binary
		else strcat(output,"0");
		n = n / 2;
		}
		while (n > 0);
		
		strrev(output);

		char *op = (char*)malloc(5);
		op[0] = output[4];
		op[1] = output[5];
		op[2] = '\0';
		
		if(strcmp(op,"00") == 0)						// Data Processing Command
		 {						
			
			char* command = (char*)malloc(5);			// Gathering the command
			char* I = (char*)malloc(2);
			I[0] = output[6];							// Gathering I
			I[1] = '\0';
			int S = 0;									// Gathering S
			if (output[11] == '0')	S = 0;
			else S = 1;
			
			int rd = 0,rn = 0,src = 0,i;
			int val1=0,val2=0;
			
			char* buff = (char*)malloc(10);
			int j=0;
			for(i=7;i<=10;i++) {
				command[j] = output[i];
				j+=1;
			}
			command[j] = '\0';
			
			char* asCmd = (char*) malloc(30);			// To write the assembly code
			char* temp;		
			asCmd[0] = '\0';
	
			if (strcmp(command, "0100") == 0) 		// Add
			{
				strcat(asCmd,"ADD R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,", R");
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
				doAdd(rd,val1,val2,S);
			}
	
			else if (strcmp(command, "0010") == 0) 		// Subtract
			{
				strcat(asCmd,"SUB R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,", R");
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
			
				doSub(rd,val1,val2,S);
	
			}
			
				else if (strcmp(command, "0011") == 0) 		// Reverse Subtract
			{
				strcat(asCmd,"SUB R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,", R");
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
			
				doRsb(rd,val1,val2,S);
	
			}
	
			else if (strcmp(command, "1010") == 0)  	// Compare
			{
				strcat(asCmd,"CMP R");
			
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
			
				doCmp(rd,val1,val2,1);
	
			}
		
			else if (strcmp(command, "1101") == 0)  	// Move
			{
				strcat(asCmd,"MOV R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
			
				doMove(rd,val2,S);
	
			}
			
			
			else if (strcmp(command, "0101") == 0) 		// Add with Carry
			{
				strcat(asCmd,"ADC R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,", R");
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
				doAdc(rd,val1,val2,S);
			}
		
			else if (strcmp(command, "0110") == 0) 		// Subtract with carry
			{
				strcat(asCmd,"SBC R");
			
				// Retriving RD
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rd = binToDecimal(buff);	
				temp = decimalToString(rd);
	
				strcat(asCmd,temp);
				
				// Retriving Rn
				j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				rn = binToDecimal(buff);	
				val1 = registers[rn];
				temp = decimalToString(rn);
				strcat(asCmd,", R");
				strcat(asCmd,temp);
				
				// Retriving src
				if (strcmp(I,"0") == 0) {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);	
					val2 = registers[src];
					temp = decimalToString(src);
					strcat(asCmd,", R");
					strcat(asCmd,temp);			}				
				else {
					j=0;
					for(i=24;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';	
					src = binToDecimal(buff);	
					val2 = src;
					temp = decimalToString(src);
					strcat(asCmd,", #");
					strcat(asCmd,temp);	
				}
			
				doSbc(rd,val1,val2,S);
	
			}
	
			
			printf("%s\n",asCmd);	// output the instruction being decoded
		
			// once we know the instruction we can execute it!
			
		
		}
		
		else if(strcmp(op,"10") == 0) 					// Branch Command 
		{					
		
			char* buff = (char*)malloc(30);			// Gathering the command
			
			int j=0,i;
			for(i=8;i<=31;i++) {
				buff[j] = output[i];
				j+=1;
			}
			buff[j] = '\0';
			
			int val = binToDecimal(buff);
			
			branch(val);
		}
		
		else if(strcmp(op,"10") == 0) 					// Memory Command
		{	
			char* I = (char*)malloc(2);
			I[0] = output[6];							// Gathering I
			I[1] = '\0';
			
			char* P = (char*)malloc(2);
			P[0] = output[7];							// Gathering P
			P[1] = '\0';
			
			char* U = (char*)malloc(2);
			U[0] = output[8];							// Gathering U
			U[1] = '\0';
			
			char* B = (char*)malloc(2);
			B[0] = output[9];							// Gathering B
			B[1] = '\0';
			
			char* W = (char*)malloc(2);
			W[0] = output[10];							// Gathering W
			W[1] = '\0';
			
			char* L = (char*)malloc(2);
			L[0] = output[11];							// Gathering L
			L[1] = '\0';
			
			char* buff = (char*)malloc(30);				// Temp Variable

			int Rn=0, Rd = 0,src = 0;
			// Rn
				int i,j=0;
				for(i=12;i<=15;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				Rn = binToDecimal(buff);	

			// Rd	
				j=0;
				for(i=16;i<=19;i++) {
					buff[j] = output[i];
					j+=1;
				}
				buff[j] = '\0';
				Rd = binToDecimal(buff);
				
			// src
				if(strcmp(I,"0") == 0) {
					j=0;
					for(i=20;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = binToDecimal(buff);			
				}
				else {
					j=0;
					for(i=28;i<=31;i++) {
						buff[j] = output[i];
						j+=1;
					}
					buff[j] = '\0';
					src = registers[binToDecimal(buff)];			
				}
			
			
			// Performing ldr Operations
			if(strcmp(L,"1")==0 && strcmp(B,"0")==0) {
				// Offset
				if(strcmp(P,"1")==0 && strcmp(W,"0")==0) {
					registers[Rd] = memory[registers[Rn]+src];
				}
				// Post Index
				else if(strcmp(P,"0")==0 && strcmp(W,"0")==0) {
					registers[Rd] = memory[registers[Rn]];
					registers[Rd] += src;
				}
				// Pre Index
				else if(strcmp(P,"1")==0 && strcmp(W,"1")==0) {
					registers[Rn] = registers[Rn] + src;
					registers[Rd] = memory[registers[Rn]]; 	
				}
			}
			// Performing Str Operations
			else if(strcmp(L,"0")==0 && strcmp(B,"0")==0) {
				// Offset
				if(strcmp(P,"1")==0 && strcmp(W,"0")==0) {
					memory[registers[Rn]+src] = registers[Rd];
				}
				// Post Index
				else if(strcmp(P,"0")==0 && strcmp(W,"0")==0) {
					memory[registers[Rn]] = registers[Rd] + src;
				}
				// Pre Index
				else if(strcmp(P,"1")==0 && strcmp(W,"1")==0) {
					registers[Rd] = registers[Rd] + src;
					memory[registers[Rn]] = registers[Rd]; 	
				}
			}
		}
}
}


// Writing a function for each supported operation

/*
 * Adds op1Value to op2Value and stores in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be added
 * 			op2Value  -  the second value to be added
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doAdd(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value + op2Value;

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}
		
		
	}
}
/*
 * Moves op1Value in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be stored
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doMove(int regNumber, WORD op1Value, int setSR)
{
	WORD result = op1Value;

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Subs op2Value from op1Value and stores in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be subtracted from
 * 			op2Value  -  the second value to be subtracted
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doSub(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value - op2Value;

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Compares op1Value to op2Value and chnages the flag
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value 
 * 			op2Value  -  the second value 
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doCmp(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value - op2Value;


	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Subtractes op2Value from op1Value and also subtracts 1 id carry is set and stores in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be subtracted from
 * 			op2Value  -  the second value to be subtracted
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doSbc(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value - op2Value;
	
	if (isClear(STAT_C))
		result-=1;

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Adds op1Value to op2Value with carry value 1 if carry is set and stores in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be added
 * 			op2Value  -  the second value to be added
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
void doAdc(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value + op2Value;
	
	if (isSet(STAT_C))
		result+=1;
	
	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Reverse subtracts op1Value from op2Value and stores in the specified register
 *
 * enter: 	regNumber - the number of the destination register (0-15)
 * 			op1Value  - the first value to be subtracted
 * 			op2Value  -  the second value to be subtracyed from
 * 			setSR     - flag, if non-zero then the status register should be updated, else it shouldn't
 */
 void doRsb(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op2Value - op1Value;

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {
			setFlag(STAT_Z);
		}
		else {
			clearFlag(STAT_Z);
		}
		
		if (result >= 0) {
			setFlag(STAT_N);
		}
		else {
			clearFlag(STAT_N);
		}
		
		if (result > 2147483647) {				// Max value for 32 bit
			setFlag(STAT_C);
		}
		else {
			clearFlag(STAT_C);
		}		
	}
}
/*
 * Branches off to new memory location with the value provided
 *
 * enter: 	val - the amount of memory to be incremented
 */
void branch(int val) {
	REGISTER pc = registers[15];
	pc = (pc + 8) + val << 2;
	registers[15] = pc; 
}



void showCPUstatus() {
	int i;
	printf("\n REGISTERS : \n\t");
	for(i=0;i<=15;i++) {
		printf("R%d : %d    ",i,registers[i]);
		if((i+1)%5 == 0)
			printf("\n\t");
	}
	printf("\n FLAGS : \n\t");
	printf("N : %d\tZ : %d\n\tC : %d\tV : %d",isSet(STAT_N),isSet(STAT_Z),isSet(STAT_C),isSet(STAT_V));
	printf("\n\n");
}


int main(void)
{
	FILE *filePtr;
	
	int done = 0;

	registers[15] = 0;	// init the PC.

	// Reading the instructions from file in system. This data represents the instructions to be executed.
	
	filePtr = fopen("MachineCodes.txt","r");
	
	if (filePtr == NULL) 
		printf("Can't open file MachineCodes.txt");
	else {
		char ch,*code = (char*) malloc(20);
		code[0] = '\0';
		int i=0,j=0;
		
			do {
	        ch = fgetc(filePtr);
	        if(ch == '\n' || ch == EOF) {
	        	code[i] = '\0';
	        	memory[j++] = strtoul(code, NULL, 16);
	        	i=0;
	        	code[i] = '\0';
			}
			else  code[i++] = ch;
	    } while (ch != EOF);
	    memory[j] = 0;
	}
	char cnd,*extra;
	do {
		printf("Do you want to simulate this program? (Y/N) : ");
		scanf("%c",&cnd);
		fflush(stdin);
	} while(cnd != 'Y' && cnd != 'y' && cnd != 'N' && cnd != 'n');

	while(!done)
	{
		REGISTER pc = registers[15];	// get the PC form the register array

		WORD inst = memory[pc++];		// fetch the next instruction, also increment the PC to point at the next instruction

		registers[15] = pc;				// store the updated PC back into the register array

		if ( inst != 0 ) {
			decodeAndExecute(inst);
			showCPUstatus();
		}
		else {
			// found an instruction with the value 0
			done = 1;	// finish once a 0 hit. Not correct behaviour but good for testing
		}
		if(cnd == 'Y' || cnd == 'y') Sleep(2000);
		else if(done == 0){
			printf("Press enter to execute next step!");
			scanf("%c",&extra);
			fflush(stdin);
		}
	}
	return 0;	// all ok
}
