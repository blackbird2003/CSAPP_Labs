/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    //           00  01  10  11
    // x xor y    0   1   1   0
    // x & y      0   0   0   1
    // ~x & ~y    1   0   0   0
    // a | b = ~(~a & ~b)
    return ~~(~(x&y) & ~(~x&~y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    //x     01111111
    //x+1   10000000
    //only Tmax and -1 satisfy ((x + 1) + x + 1 == 0)
    // ans = !((x + 1) + x + 1) & (x != -1);
    //only -1 satisfy (~x == 0)
    // (x != -1) <=> !!(~x)
    return !((x + 1) + x + 1) & !!(~x);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    //A = 1010
    //let f = 0xAAAAAAAA
    //return (x & f == f)
    // => (x & f) ^ f == 0
    // => !((x & f) ^ f)
    int f = 0xAA;
    f = (f << 8) + f;
    f = (f << 16) + f;
    return !((x & f) ^ f);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    //x = -(a)*2^31+(b)
    //-x = -(-a)*2^31 + (-b)
    //~b = 2^31 - 1 - b
    //-b = ~b + 1 - 2 ^ 31
    //-x = -(1 - a)*2^31 + ~b + 1
    //   = -(~a)*2^31 + (~b) + 1
    //   = ~x + 1
    return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    //00110000 00111001
    //((x >> 4) == 0011) && ((x | 1111) <= 1001)
    //1001 1000 0111...
    //((x >> 4) == 0011) && ((x & 1111) == 1001) || ((x & 1111) == 1000) || ((x & 1111 & 8 == 0)
    // (a == b) <=> !(a^b)
    int f = x & 15;
    return !((x >> 4) ^ 3) & (!(f ^ 9) | !(f ^ 8) | !(f & 8));
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    //if (x != 0) y; else z;
    //(x != 0) <=> (!!x == 1)
    //补码右移 1000 0000 >> 7 = 11111111
    int f = !!x;
    f = f << 31 >> 31;
    return (f & y) | (~f & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    // -x = ~x + 1
    // (y >= x) <=> y + (~x + 1) >= 0
    // (a >= 0) <=> !(a >> 31)
    // 防止溢出,先比较符号位
    // A ? B : (C ? D : E);
    int a = x >> 31;
    int b = y >> 31;
    int A = !(a ^ b); // A: a == b
    int B = !((y + (~x + 1)) >> 31);
    int C = a & !b; // x < 0, y >= 0
    int fC = C << 31 >> 31;
    int resC = (fC & 1) | (~fC & 0);
    int fA = A << 31 >> 31;
    return (fA & B) | (~fA & resC);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    //(x == 0) <=> (x ^ (~x + 1)) >> 31 & 1 == 0
    //(x != 0) <=> (x ^ (~x + 1)) >> 31 & 1 == 1
    //return 1 -  (x ^ (~x + 1)) >> 31 & 1;
    //需要特判 x == Tmin ?
    //注意到1 | 1 = 1 | 0 = 1,把 ^ 改成 | 即可
    return (1 + (~((x | (~x + 1)) >> 31 & 1) + 1));
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    // 12 = 8 + 4 = 01100
    // -5 = 8 - 2 - 1 = 1011
    //  5 = 4 + 1 = 0101
    // 正数 sign + 最高位1的位数
    // 负数 = sign + (取反+1后最高位1的位数)
    // if (x == 0) return 1;
    // if (x < 0) x = ~x + 1;
    // 如何确定正数最高位1的位置
    // int ans = 1;
    // if ((x >> 16) != 0) x >>= 16, ans += 16;
    // if ((x >> 8) != 0) x >>= 8, ans += 8;
    // if ((x >> 4) != 0) x >>= 4, ans += 4;
    // if ((x >> 2) != 0) x >>= 2, ans += 2;
    // if ((x >> 1) != 0) x >>= 1; ans += 1;
    /*
      x ? y : z;
      int f = !!x << 31 >> 31;
      return (f & y) | (~f & z);
     */
    int fsign, f0, ans, f16, f8, f4, f2, f1;
    fsign = !!(x >> 31) << 31 >> 31;
    x = (fsign & (~x)) | (~fsign & x);
    f0 = !x << 31 >> 31;

    ans = 1;

    f16 = !!(x >> 16) << 31 >> 31;
    x = (f16 & (x >> 16)) | (~f16 & x);
    ans = (f16 & ans + 16) | (~f16 & ans);

    f8 = !!(x >> 8) << 31 >> 31;
    x = (f8 & (x >> 8)) | (~f8 & x);
    ans = (f8 & ans + 8) | (~f8 & ans);

    f4 = !!(x >> 4) << 31 >> 31;
    x = (f4 & (x >> 4)) | (~f4 & x);
    ans = (f4 & ans + 4) | (~f4 & ans);

    f2 = !!(x >> 2) << 31 >> 31;
    x = (f2 & (x >> 2)) | (~f2 & x);
    ans = (f2 & ans + 2) | (~f2 & ans);

    f1 = !!(x >> 1) << 31 >> 31;
    ans = (f1 & ans + 1) | (~f1 & ans);

    ans = (f0 & 0) | (~f0 & ans);
    return ans + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    //32 = 1 + 8 + 23
    unsigned s = uf >> 31;
    unsigned exp = uf << 1 >> 24;
    unsigned frac = uf << 9 >> 9;
    // V = (-1)^s * M * 2^E
    // Normal: exp != 0 && exp != 255
    // Denormal : exp = 0 (uf is 0 / is very close to zero)

    // Normal : M = 1 + f, E = e - Bias; Denormal : M = f, E = 1 - Bias

    // Special : exp = 255 (if frac == 0, s = 0/1, uf = +/-inf; else uf = NaN)
    if (exp == 255) return uf;
    else if (!exp) return (s << 31) + (frac << 1);  // 如果frac溢出，恰好Normal表示为原数的两倍
    else return (s << 31) + ((exp + 1) << 23) + frac;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    unsigned s = uf >> 31;
    signed exp = uf << 1 >> 24;
    unsigned frac = uf << 9 >> 9;
    // Normal: exp != 0 && exp != 255
    // Denormal : exp = 0 (uf is 0 / is very close to zero)
    // Special : exp = 255 (if frac == 0, s = 0/1, uf = +/-inf; else uf = NaN)
    if (exp == 255) return 0x80000000u;
    else if (exp == 0) return 0;

    // V = (-1)^s * M * 2^E
    // Normal : M = 1 + f, E = e - Bias;
    exp = exp - 127;
    frac |= 1 << 23;
    if (exp > 31) return 0x80000000u;
    if (exp < 0) return 0;
    if (exp >= 23) {
        frac <<= exp - 23;
    } else {
         frac >>= 23 - exp;
    }
    if (s) frac *= -1;
    return frac;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    /*
    Range of float
    Denormalize [2^-149,2^-126) (2^-126-2^-129)
    Normalize (2^-126,2^128)
     */
    if (x < -149) return 0;
    // V = (-1)^s * M * 2^E
    // Denormal : M = f, E = 1 - Bias = -126;
    // 2 ^ x = M * 2 ^ (-126)
    // M = f = 2 ^ (126 + x)
    if (x < -126) {
        return 1 << (23 + (x + 126));
    }
    if (x <= 127) return (x + 127) << 23;
    return 0xFF << 23;
}
