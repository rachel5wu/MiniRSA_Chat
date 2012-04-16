#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
/*
 * returns 0 if an error occurs, returns 1 if everything is fine
 */

int extractTwos(int m ,int *s, int *d) {
    if (s == NULL  && d == NULL) {
        return 0;
    }
    *s = 0;
    *d = 0;
    while(m % 2 == 0) {
        m = m / 2;
        *s += 1;
      
    }
    *d = m;
    return 1;

}

int int2BaseTwo (int x, int *c) {
    int numOfDigits = 1 + log2(x);
    int i = 0;

    if (c == NULL) {
        return 0;
    }   
    for(i = 0; i < numOfDigits ; i++) {
        c[i] = x % 2;
        x = x / 2;
       // printf("x = %d", x);
  
    }
//      c[numOfDigits] = '\0';

    return 1;

}

int GCD(int a, int b) {
    int remainder = 1;
    while(remainder != 0) {
        remainder = a % b;
        a = b;
        b = remainder; 
    }
    return a;
}

int coprime(int x) {
    int random, i;
    for(i = 0; i < 200; i++)  {
        random = rand() % x + 2;
        //printf("rannd = %d\n", random);
        if(GCD(x, random) == 1) 
            return random;
    }

}

long modulo(int a, int d, int n) {
    int *c;
    int *auxc;
    int numOfDigits = 1 + log2(d);
    int i = 0;
    long retValue = 1;
    c = (int *)malloc((numOfDigits )* sizeof(int)); 
    auxc = (int *)malloc((numOfDigits )* sizeof(int)); 
    int2BaseTwo(d, c);
    for (i = 0; i < numOfDigits; i++) {
       // printf("%d\n", c[i]);
    }
    auxc[0] = a % n;
    for (i = 1; i < numOfDigits; i++) {
    auxc[i] = ((int)pow(auxc[i - 1], 2)) % n;

    }
    for (i = 0; i < numOfDigits; i++) {
        if (c[i]) {
            retValue = retValue * auxc[i] % n;
        }
    }
    free(c);
    free(auxc);

    return retValue;
}
/*
 * returns 0 if its not prime else returns 1
 */
int millerRabin(int n, int k) {
    if ((n % 2) == 0)
         return 0;

    int s = 0;
    int d = 0;
    int i = 0;
    int j = 0;
    int randNum = 0;
    int temp = 0;
    srand(time(NULL));
   
    extractTwos(n , &s, &d);
    printf("s = %d", s); 
    printf("d = %d", d);
    for (i = 0; i < k; i++) {
        randNum = (rand() % (n - 2)) + 2;
       // printf("%d\n", randNum); 
  temp = modulo(randNum, d, n);
        if(temp == 1 || temp == n - 1)
            continue;
        for(j = 0; j < s; j++) {
            temp = modulo(temp, 2, n);
            if (temp == 1)
                 return 0;
            if (temp == n - 1)
                 break;
            if (j == s - 1)
                return 0;
            j += 1;

        }
 

    }
    return 1;
}

/*
 * returns -1 if no inverse exists else returns the modular inverse
 */


int extendedEuclid(int a, int b, int *d, int *x, int *y) {
    
    int  x1, x2,  y2, y1; 
    if(b == 0) {
       *d = a;
       *x = 1;
       *y = 0;
       return 1;   

    }
    x2 = 1;
    x1 = 0;
    y2 = 0;
    y1 = 1;
    int quotient; int remainder;
    while (b > 0) {
         quotient = a / b;
         remainder = a - (quotient) * b;
         *x = x2 - quotient * x1;
         *y = y2 - quotient * y1;
         a = b;
         b = remainder;
         x2 = x1;
         x1 = *x;
         y2 = y1;
         y1 = *y; 

    }
    *d = a;
    *x = x2;
    *y = y2;
    
    return 1;
    
 
}

int isPrime(int x) {
    int i = 0;
    for(i = 2; i  < x; i++) {
        if((x % i) == 0)
            return 0; 
    }
    return 1;
}


int totient(int n) {
    //first find all possible prime numbers between printf("My private key: %d, %d\n", args->my_d, args->my_c);2 and n
    int *primeList  = (int *)malloc(n * sizeof(int));
    primeList[0] = 2;
    int j = 0;
    int p;
    int q;
    int index = 1;
    int i = 2;
    for(i = 3; i < n; i +=2) {
        if(isPrime(i) == 1) {
            primeList[index] = i;
            index++;
        }
    } 
    for(i = 0; i < index; i++) {
       // printf("prime = %d\n", primeList[i]);

    }
    //now we have the list in primeList, brute force to find the primeNUmbers
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


int encrypt(int input, int e, int c) {
return modulo(input, e, c);

}


/*
 * returns -1 if numbers are not co-prime
 * else returns the modular inverse
 */


int mod_inverse(int base, int m) {
    int d, x, y;
    int retValue = 0;
    d = 1;
    x = 1;
    y = 1;
    
    if(GCD(base, m) == 1) {
        extendedEuclid(base, m, &d, &x, &y);
        retValue =  x % m;
        if(retValue < 0) 
           retValue = retValue + m;
        return retValue; 
    }
    return -1;
}

char decrypt(int input, int d, int c) {
    printf("%d decrypted using d=%d,c=%d as %d", input, d, c, modulo(input, d, c));
    return modulo(input, d, c);
}

int generatePrimeNumbers(int *m, int *n) {
    int maximum = *m > *n ? *m : *n;
    int *primeList = (int *)malloc(maximum * sizeof(int));
    int index = 1; 
    int i = 3;
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
/*
void main() {
srand(time(NULL));
char *input = "Hello";
int *encOutput = (int *)malloc(sizeof(int) * strlen(input) + 1);
int c = 13 * 17;
int i; 
int a = 13;
int b = 17;
int e = coprime(12 * 16);
int m = totient(13 * 17);
int d = mod_inverse(e, 12 * 16); 
printf("totient = %d", totient(13 * 17));
printf("e = %d\n", coprime(12 * 16));



printf("d = %d\n", mod_inverse(coprime(12 * 16), 12 * 16));
int enc = encrypt(50, e,c);
int dec = decrypt(enc, d, c);
printf("enc = %d\n", enc);
printf("dec = %d\n", dec);


for(i = 0; i < strlen(input); i++) {
    encOutput[i] = encrypt(input[i], e, c);
     
  
}

printf("Hello has been translated into %d %d %d %d %d\n", encOutput[0], encOutput[1], encOutput[2], encOutput[3], encOutput[4]);


//    printf("coprime=%d\n",mod_inverse(3, 30));
//printf("%d", modulo(2349723,423424,12345));
//printf("mod inv = %d\n", extendedEuclid(442, 2278, &d, &x, &y));
//printf("d = %d \t x = %d \t y = %d\n", d,x,y);
//coprime(5);

}*/
