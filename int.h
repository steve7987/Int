//num[0] is the first digit, and num[length - 1] the last
typedef struct
{
	size_t length;  
	unsigned char * num;
	unsigned char negative;  //0 is positive, non 0 is negative
} BigInt;

//in general these functions return 1 if the function failed, and 0 if it succeeded
//when saving and loading sign is stored in the first byte then the least significant byte is stored first
int LoadInt(char * filename, BigInt * result);  //loads a big int from a file into memory
int SaveInt(char * filename, BigInt * tosave);  //saves a big int to a file
int MakeInt(int value, BigInt * result);  //creates a big int with the value of the given int
int CopyInt(BigInt * source, BigInt * dest);  //makes a deep copy of the source
void DeleteInt(BigInt * deleteme);  //frees the data of the int
void PrintInt(BigInt * printme);  //prints the int in hexadecimal
int Compare(BigInt * x, BigInt * y);  //returns 1 if x > y, 0 if x == y, -1 if x < y
double Approximate(BigInt * x);  //gives the double approximation of this int


//these functions perform arithmetic where one input is small
//in general they will modify the input big int (except modulo)
void AddS(BigInt * x, unsigned char y);
void SubtractS(BigInt * x, unsigned char y);
void MultiplyS(BigInt * x, unsigned char y);
void DivideS(BigInt * x, unsigned char y);
unsigned char ModuloS(BigInt * x, unsigned char y);
void RightShift(BigInt * x, unsigned int amt);  //right shifts x by amt bits  
void LeftShift(BigInt * x, unsigned int amt);  //left shifts x by amt bits (excess bits should not be discarded)


//these functions perform arithmetic where both inputs are big
//they should not modify either input
//in general they return 1 if the function failed, and 0 if it succeeded
int Add(BigInt * x, BigInt * y, BigInt * sum);
int Subtract(BigInt * x, BigInt * y, BigInt * difference);
int Multiply(BigInt * x, BigInt * y, BigInt * product);
int Divide(BigInt * x, BigInt * y, BigInt * quotient);
int Modulo(BigInt * x, BigInt * y, BigInt * remainder);

char * ToBase10(BigInt * x);  //returns a null terminated string that contains the base 10 representation of the int
void GCD(BigInt * x, BigInt * y, BigInt * gcd);  //computes the greatest common divisor of the two inputs

