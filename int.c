#include <stdio.h>
#include <stdlib.h>
#include "int.h"

int SaveInt(char * filename, BigInt * tosave){
	FILE * fp = fopen(filename, "wb");
	int length = tosave->length;
	while(tosave->num[length - 1] == 0){  //remove leading zeros
		length--;
	}
	if (length == 0){
		length = 1;
	}
	if (!fp){
		printf("unable to open file %s", filename);
		return 1;
	}
	//write the sign here
	if (fwrite(&(tosave->negative), sizeof(unsigned char), 1, fp) != 1){
		printf("Failed to write file %s", filename);
		return 1;
	}
	//write the rest of the number
	if (fwrite(tosave->num, sizeof(unsigned char), length, fp) != length){
		printf("Failed to write file %s", filename);
		return 1;
	}
	fclose(fp);
	return 0;
}

int LoadInt(char * filename, BigInt * result){
	FILE * fp = fopen(filename, "rb");
	size_t intlength;
	if (!fp){
		printf("Unable to open file %s", filename);
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	intlength = ftell(fp) - sizeof(unsigned char);  //exclude the sign character
	fseek(fp, 0, SEEK_SET);
	result->num = malloc(sizeof(unsigned char) * ((intlength + sizeof(unsigned char) - 1) / sizeof(unsigned char)));
	if (!result->num){
		printf("Malloc failed when loading file %s", filename);
		return 1;
	}
	result->length = (intlength + sizeof(unsigned char) - 1) / sizeof(unsigned char);
	result->num[result->length - 1] = 0;
	//read sign
	if (fread(&(result->negative), sizeof(unsigned char), 1, fp) != 1){
		printf("Failed reading file %s", filename);
		return 1;
	}
	//read digits
	if (fread(result->num, 1, intlength, fp) != intlength){
		printf("Failed reading file %s", filename);
		return 1;
	}
	
	fclose(fp);
	return 0;
}

int MakeInt(int value, BigInt * result){
	int i;
	result->length = sizeof(int) / sizeof(unsigned char);
	result->num = malloc(sizeof(int));
	if (!result->num){
		printf("Malloc failed when making int with value %d", value);
		return 1;
	}
	if (value < 0){
		result->negative = 1;
		value = value * -1;
	}
	else {
		result->negative = 0;
	}
	for (i = 0; i < result->length; i++){
		result->num[i] = (unsigned char) value;
		value = value >> (8 * sizeof(unsigned char));
	}
	return 0;
}

int CopyInt(BigInt * source, BigInt * dest){
	dest->num = malloc(source->length * sizeof(unsigned char));
	if (!dest->num){
		printf("Copy failed");
		return 1;
	}
	int i;
	dest->length = source->length;
	for (i = 0; i < source->length; i++){
		dest->num[i] = source->num[i];
	}
	dest->negative = source->negative;
	return 0;
}

void DeleteInt(BigInt * deleteme){
	if (deleteme->num){
		free(deleteme->num);
		deleteme->num = 0;
	}
}

void PrintInt(BigInt * printme){
	int i = printme->length - 1;
	while (printme->num[i] == 0) i--;
	if (printme->negative != 0 && i != -1){
		printf("-");
	}
	
	if (i == -1){
		printf("0");
	}
	
	for (; i >= 0; i--){
		printf("%0*X  ", sizeof(unsigned char)*2, printme->num[i]);

	}
	printf("\n");
}

double Approximate(BigInt * x){
	double ret = 0;
	double pow = 1;
	int i;
	for (i = 0; i < x->length; i++){
		ret += x->num[i] * pow;
		pow = pow * (1 << 8*sizeof(unsigned char));
	}
	if (x->negative != 0){
		ret = ret * -1;
	}
	return ret;
}

int Compare(BigInt * x, BigInt * y){
	int i, flag;  //flag flips return result if both numbers are negative
	//check if number are different signs
	if (x->negative == 0 && y->negative != 0){
		//check if x is zero
		for (i = 0; i < x->length; i++){
			if (x->num[i] != 0){
				return 1;
			}
		}
		//check if y is zero
		for (i = 0; i < y->length; i++){
			if (y->num[i] != 0){
				return -1;
			}
		}
		return 0;
	}
	else if (x->negative != 0 && y->negative == 0){
		//check if x is zero
		for (i = 0; i < x->length; i++){
			if (x->num[i] != 0){
				return -1;
			}
		}
		//check if y is zero
		for (i = 0; i < y->length; i++){
			if (y->num[i] != 0){
				return 1;
			}
		}
		return 0;
	}
	else if (x->negative == 0){
		//both positive
		flag = 1;
	}
	else {
		flag = -1;
	}
	if (x->length > y->length){
		//check if anything is non zero
		for (i = x->length - 1; i >= (int) y->length; i--){
			if(x->num[i] != 0){
				return 1 * flag;
			}
		}
	}
	else if (x->length < y->length){
		//check if anything is non zero
		for (i = y->length - 1; i >= x->length; i--){
			if(y->num[i] != 0) return -1 * flag;
		}
	}
	else {
		i = x->length;
	}
	for (; i >= 0; i--){
		
		if (x->num[i] > y->num[i]) return 1 * flag;
		if (x->num[i] < y->num[i]) return -1 * flag;
	}
	return 0;
}

void AddS(BigInt * x, unsigned char y){
	//check that x is positive, otherwise need to subtract
	
	//add y
	int i = 0;
	while (y != 0){
		int result = x->num[i] + y;
		x->num[i] = (unsigned char) result;
		y = result >> (8 * sizeof(unsigned char));  //set y to be the carry
		i++;
	}
}

void SubtractS(BigInt * x, unsigned char y){
	//check if x > y
	
	
	//subtract y
	int i = 0;
	while (y != 0){
		if (y <= x->num[i]) {
			x->num[i] = x->num[i] - y;
			return;
		}
		//need to borrow
		x->num[i] = (1 << 8*sizeof(unsigned char)) + x->num[i] - y;
		y = 1;
		i++;
	}
}

void DivideS(BigInt * x, unsigned char y){
	if (y == 0 || y == 1) return;
	int overflow = 0;
	int quotient = 0;
	int i;
	for (i = x->length - 1; i >= 0; i--){
		quotient = (x->num[i] + (overflow << (8*sizeof(unsigned char)))) / y;
		overflow = (x->num[i] + (overflow << (8*sizeof(unsigned char)))) - quotient * y;
		x->num[i] = quotient;
	}
}

unsigned char ModuloS(BigInt * x, unsigned char y){
	unsigned char retval = 0;
	int factor = 1;
	int i;
	for (i = 0; i < x->length; i++){
		retval = (retval + x->num[i]*factor) % y;
		factor = (factor << sizeof(unsigned char) * 8) % y;
	}
	return retval;
}

void LeftShift(BigInt * x, unsigned int amt){
	//shift by mults here
	
	//shift by rest
	int enter = 0;
	int exit = 0;
	int i;
	for (i = 0; i < x->length; i++){
		exit = x->num[i] >> (8*sizeof(unsigned char) - (amt % (8*sizeof(unsigned char))));
		x->num[i] = (x->num[i] << (amt % (8*sizeof(unsigned char)))) | enter;
		enter = exit;
	}
	if (exit != 0){
		//need to expand int
		unsigned char * tmp = malloc(sizeof(unsigned char)*2*x->length);
		for (i = 0; i < x->length; i++){
			tmp[i] = x->num[i];
		}
		for (i = x->length; i < 2*x->length; i++){
			tmp[i] = 0;
		}
		tmp[x->length] = exit;
		x->length = 2*x->length;
		free(x->num);
		x->num = tmp;
	}
}

int Add(BigInt * x, BigInt * y, BigInt * sum){
	//make y the smaller int based of length only
	if (y->length > x->length){
		BigInt * tmp = x;
		x = y;
		y = tmp;
	}
	sum->length = x->length + 1;
	sum->negative = 0;
	sum->num = malloc(sum->length * sizeof(char));
	if (!sum->num){
		printf("Malloc failed when adding");
		return 1;
	}
	int curindex;
	int carry = 0;
	//add x and y with carry
	for (curindex = 0; curindex < y->length; curindex++){
		carry = carry + x->num[curindex] + y->num[curindex];
		sum->num[curindex] = (unsigned char) carry;
		carry = carry >> 8*sizeof(unsigned char);
	}
	//y out of digits so just add x and carry
	for (; curindex < x->length; curindex++){
		carry = carry + x->num[curindex];
		sum->num[curindex] = (unsigned char) carry;
		carry = carry >> 8*sizeof(unsigned char);
	}
	sum->num[curindex] = (unsigned char) carry;
	return 0;
}

int Subtract(BigInt * x, BigInt * y, BigInt * difference){
	//assuming both are positive
	if (Compare(x, y) < 0){
		BigInt * tmp = x;
		x = y;
		y = tmp;
		difference->negative = 1;
	}
	else {
		difference->negative = 0;
	}
	
	//assume x >= y
	difference->num = malloc(sizeof(unsigned char)*x->length);
	if (!difference->num){
		printf("Error with malloc in Subtract");
		return 1;
	}
	difference->length = x->length;
	int borrow = 0;
	int i;
	//subtracting y from x with borrowing
	for (i = 0; i < y->length; i++){
		if (x->num[i] >= y->num[i] + borrow){
			difference->num[i] = x->num[i] - y->num[i] - borrow;
			borrow = 0;
		}
		else {
			difference->num[i] = x->num[i] - y->num[i] - borrow + (1 << (8*sizeof(unsigned char)));
			borrow = 1;
		}
	}
	//y out of digits just deal with borrow
	while (borrow != 0){
		if (x->num[i] >= borrow){
			difference->num[i] = x->num[i] - borrow;
			borrow = 0;
		}
		else {
			difference->num[i] = x->num[i] - y->num[i] + (1 << (8*sizeof(unsigned char)));
			borrow = 1;
		}
		i++;
	}
	//copy down extra digits of x
	for (;i < x->length; i++){
		difference->num[i] = x->num[i];
	}
	for (;i < difference->length; i++){
		difference->num[i] = 0;
	}
}

int Multiply(BigInt * x, BigInt * y, BigInt * product){
	product->length = x->length + y->length;
	product->num = malloc(sizeof(unsigned char)*product->length);
	if (!product->num){
		printf("Malloc in multiplication failed");
		return 1;
	}
	int i, j;
	int carry = 0;
	int sum;
	for (i = 0; i < product->length; i++){  //loop over digits of result
		sum = carry;
		for (j = 0; j <= i; j++){  //add in all combos that give digit i
			if (j < x->length && i-j < y->length){  
				sum = sum + x->num[j]*y->num[i-j];
			}
		}
		product->num[i] = (unsigned char) sum;
		carry = sum >> (8*sizeof(unsigned char));
	}
	//figure out the sign of the product
	if (x->negative != 0 && y->negative != 0){
		product->negative = 0;
	}
	else {
		product->negative = x->negative ^ y->negative;
	}
	return 0;
}

//basically subtract but does x = x - y;
int DivideHelp(BigInt * x, BigInt * y){
	int borrow = 0;
	int i;
	for (i = 0; i < y->length; i++){
		if (x->num[i] >= y->num[i] + borrow){
			x->num[i] = x->num[i] - y->num[i] - borrow;
			borrow = 0;
		}
		else {
			x->num[i] = x->num[i] - y->num[i] - borrow + (1 << (8*sizeof(unsigned char)));
			borrow = 1;
		}
	}
	while (borrow != 0){
		if (x->num[i] >= borrow){
			x->num[i] = x->num[i] - borrow;
			borrow = 0;
		}
		else {
			x->num[i] = x->num[i] - y->num[i] + (1 << (8*sizeof(unsigned char)));
			borrow = 1;
		}
		i++;
	}
}

int Divide(BigInt * x, BigInt * y, BigInt * quotient){
	//proceed assuming y smaller than x
	BigInt remainder;
	remainder.length = y->length + 1;
	remainder.num = malloc(sizeof(unsigned char)*remainder.length);
	quotient->length = x->length - y->length + 1;
	quotient->num = malloc(sizeof(unsigned char)*quotient->length);
	if (!quotient->num){
		printf("Malloc division failed");
		return 1;
	}
	//set remainder and quotient to zero
	int i;
	for (i = 0; i < remainder.length; i++){
		remainder.num[i] = 0;
	}
	for (i = 0; i < quotient->length; i++){
		quotient->num[i] = 0;
	}
	//follow algorithm for binary division
	for (i = x->length*sizeof(unsigned char)*8 - 1; i >= 0; i--){
		LeftShift(&remainder, 1);
		remainder.num[0] = remainder.num[0] | (x->num[i / (8*sizeof(unsigned char))] >> (i%(8*sizeof(unsigned char))));
		if (Compare(&remainder, y) >= 0){
			DivideHelp(&remainder, y);
			quotient->num[i / (8*sizeof(unsigned char))] = quotient->num[i / (8*sizeof(unsigned char))] | (1 << (i%(8*sizeof(unsigned char))));
		}
	}
	//dont need remainder here
	free(remainder.num);
	//figure out the sign of the quotient
	if (x->negative != 0 && y->negative != 0){
		quotient->negative = 0;
	}
	else {
		quotient->negative = x->negative ^ y->negative;
	}
	return 0;
}

int PrintBase10(BigInt * x){
	char * ret;
	int rlength;
	int index;
	BigInt p, zero;
	CopyInt(x, &p);
	zero.length = 0;
	ret = malloc(x->length*sizeof(char));  //can expand if necessary
	if (!ret){
		printf("Malloc failed in base 10 method");
		return 1;
	}
	rlength = x->length;
	index = 0;
	while (Compare(&p, &zero) != 0){
		ret[index] = '0' + ModuloS(&p, 10);
		DivideS(&p, 10);
		index++;
		//if out of space make array bigger
		if (index >= rlength){
			int i;
			char * tmp = malloc(2*rlength*sizeof(char));
			if (!tmp){
				printf("Malloc failed in base 10 method");
				return 1;
			}
			for (i = 0; i < rlength; i++){
				tmp[i] = ret[i];
			}
			free(ret);
			ret = tmp;
			rlength = 2*rlength;
		}
	}
	//reverse array
	int i;
	for (i = 0; i < index/2; i++){
		char t = ret[i];
		ret[i] = ret[index-i-1];
		ret[index-i-1] = t;
	}
	ret[index] = '\0';
	DeleteInt(&p);
	if (x->negative != 0){
		printf("-");
	}
	printf(ret);
	printf("\n");
	free(ret);
	return 0;
}

char * ToBase10(BigInt * x){
	char * ret;
	int rlength;
	int index;
	BigInt p, zero;
	int offset;
	if (x->negative != 0){  //need extra char for negative number
		offset = 1;
	}
	else {
		offset = 0;
	}

	CopyInt(x, &p);
	zero.length = 0;
	ret = malloc(x->length*sizeof(char));  //can expand if necessary
	if (!ret){
		printf("Malloc failed in base 10 method");
		return 0;
	}
	rlength = x->length;
	index = 0;
	while (Compare(&p, &zero) != 0){
		ret[index] = '0' + ModuloS(&p, 10);
		DivideS(&p, 10);
		index++;
		//if out of space make array bigger
		if (index + offset >= rlength){
			int i;
			char * tmp = malloc(2*rlength*sizeof(char));
			if (!tmp){
				printf("Malloc failed in base 10 method");
				return 0;
			}
			for (i = 0; i < rlength; i++){
				tmp[i] = ret[i];
			}
			free(ret);
			ret = tmp;
			rlength = 2*rlength;
		}
	}
	if (offset != 0){
		ret[index] = '-';
		index++;
	}
	//reverse array
	int i;
	for (i = 0; i < index/2; i++){
		char t = ret[i];
		ret[i] = ret[index-i-1];
		ret[index-i-1] = t;
	}
	ret[index] = '\0';
	DeleteInt(&p);
	return ret;
}