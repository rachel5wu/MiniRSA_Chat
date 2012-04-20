#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


long extractTwos( long m , long *s,  long *d) {
    if (s == NULL  && d == NULL) {return 0;}
    *s = 0;
    *d = 0;
    while(m % 2 == 0) {
        m = m / 2;
        *s += 1;
    }
    *d = m;
    return 1;
}

long  long2BaseTwo ( long x,  long *c) {
    long numOfDigits = 1 + log2(x);
    long i = 0;
    if (c == NULL) { return 0;}   
    for(i = 0; i < numOfDigits ; i++) {
        c[i] = x % 2;
        x = x / 2;
    }
    return 1;
}

long GCD( long a,  long b) {
    long remainder = 1;
    if(a == 0) return b;
    if (b == 0) return a; 
    while(remainder != 0) {
        remainder = a % b;
        a = b;
        b = remainder; 
    }
    return a;
}

long coprime( long x) {
    long random, i;    
    for(i = 0; i < 200000; i++)  {
        random = rand() % 1000;
        if(random > 1) {
            if(GCD(x, random) == 1) return random;
        }
    }
    return -1; 
}

long modulo(long a, long d, long n) {
    long *c;
    long *auxc;
    long numOfDigits = 1 + log2(d);
    long i = 0;
    long retValue = 1;
    long remainder = 1;
    c = ( long *)malloc((numOfDigits )* sizeof( long)); 
    auxc = ( long *)malloc((numOfDigits )* sizeof( long)); 
    long2BaseTwo(d, c);
    auxc[0] = a % n;
    remainder = auxc[0]; 
    for (i = 1; i < numOfDigits; i++) {
        remainder = (remainder * remainder) % n;
        auxc[i] = remainder;
        if(auxc[i] < 0)  auxc[i] += n; 
    }
    for (i = 0; i < numOfDigits; i++) {
        if (c[i] == 1) {
            retValue = (retValue * auxc[i]) % n;
        }
    }
    free(c);
    free(auxc);
    return retValue;
}



long extendedEuclid( long a,  long b,  long *u1,  long *u2,  long *u3) {
    int v1, v2, v3, t1, t2, t3;
    int quotient;
    *u1 = 1;
    *u2 = 0;
    *u3 = a;
    v1 = 0;
    v2 = 1;
    v3 = b;
    while(v3 != 0) {
        quotient = *u3 / v3;
        t1 = *u1 - quotient * v1;
        t2 = *u2 - quotient * v2;
        t3 = *u3 - quotient * v3;
        *u1 = v1;
        *u2 = v2;
        *u3 = v3;
        v1 = t1;
        v2 = t2;
        v3 = t3; 
    }
}

long isPrime( long x) {
    long i = 0;
    for(i = 2; i  < x; i++) {
        if((x % i) == 0)return 0; 
    }
    return 1;
}


long totient( long n) {
    long *primeList  = ( long *)malloc(n * sizeof( long));
    primeList[0] = 2;
    long j = 0;
    long p;
    long q;
    long index = 1;
    long i = 2;
    for(i = 3; i < n; i +=2) {
        if(isPrime(i) == 1) {
            primeList[index] = i;
            index++;
        }
    } 
    for(i = 0; i < index; i++) {
        for (j = 0 ; j  <index; j++) {
            if(primeList[i] * primeList[j] == n) {
                p = primeList[i];
                q = primeList[j];
                free(primeList);
                return (p - 1) * (q - 1);
            }
        }
    }
    free(primeList);
    return -1;
}

long encrypt( long input,  long e,  long c) {
    long retValue;
    retValue = modulo(input, e,c);
    return retValue;
}
 
long mod_inverse( long base,  long m) {
    long d, x, y;
    long retValue = 0;
    d = 1;
    x = 1;
    y = 1;
    if(GCD(base, m) == 1) {  
        extendedEuclid( base, m,  &x, &y, &d);
        retValue =  x;
        if(retValue < 0) { retValue = retValue +  m;}
        return retValue; 
    }
    return -1;
}

long decrypt( long input,  long d,  long c) {
    long retValue = modulo(input, d, c);
    return retValue;
}

long generatePrimeNumbers( long *m,  long *n) {
    long maximum = *m > *n ? *m : *n; 
    long *primeList = ( long *)malloc((maximum) * sizeof( long));
    long index = 1; 
    long i = 3;
    primeList[0] = 2;
    while(index < maximum) {
        if (isPrime(i) == 1) {
            primeList[index] = i;
            index++;         
        }
        i += 2; 
    }
    *m = primeList[*m - 1];
    *n = primeList[*n - 1];     
    free(primeList);
}


void generateKeys(long a, long b, long *e, long *d, long *c) {
    *c = a * b;
    *e = coprime( (a - 1) * (b - 1));
    *d = mod_inverse(*e,  ((a - 1) * (b - 1)));
}

