#include <stdio.h>
#include "int.h"

//to run this program use: progname file1 file2
//it reads the two files prints and multiplies them

int main(int argc, char * argv[]){
	
	
	BigInt i, j, k;
	if(argc != 3){
		printf("Correct usage of %s is %s intfile", argv[0], argv[0]);
		return 0;
	}
	if (LoadInt(argv[1], &i) || LoadInt(argv[2], &j)){
		return 1;
	}
	
	PrintInt(&i);
	PrintInt(&j);
	PrintBase10(&i);
	PrintBase10(&j);
	Subtract(&i, &j, &k);
	PrintBase10(&k);

	DeleteInt(&i);
	DeleteInt(&j);
	DeleteInt(&k);
	return 0;
	
}