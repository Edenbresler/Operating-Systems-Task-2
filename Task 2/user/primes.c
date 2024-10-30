#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



// Function to check if a number is prime
int isPrime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0)    
            return 0;
    }
    return 1;
}

// Generat numbers till chan is not exist
void generator(int chan) {
    int num = 2; 
    int success = 0; 
    while(success == 0){
        success = channel_put(chan, num);
        num++;
    }
}

// Checker numbers till chan is not exist
void checker(int generatorCann, int printerCann) {
    int num;
    int success = 0; 
    while (success == 0) {
        if (channel_take(generatorCann, &num) == 0){
            if (isPrime(num)) {
            success = channel_put(printerCann, num);
            }
        }
        else 
            break;
    }
    channel_destroy(generatorCann); 
}

// Printer 100 numbers
void printer(int printerCann, int numToPrint) {
    int num; 
    while (numToPrint > 0) {
        if (channel_take(printerCann, &num) >= 0) {
            printf("Prime: %d\n", num);
            numToPrint--; 
        } 
        else break;       
    }
    channel_destroy(printerCann);
}

void main(int argc, char *argv[]) {
    int numOfCheckers = 3;
    int numToPrint = 100;

    //change the numOfCheckers if needed
    if (argc > 1) {
        numOfCheckers = atoi(argv[1]);
    }
   
    while(1){
        int generatorCann = channel_create();
        int printerCann = channel_create();

        for (int i = 0; i < numOfCheckers; i++) {
            if (fork() == 0) { // A checker process
                int pid = getpid();
                checker(generatorCann, printerCann);
                sleep(pid);
                printf("'Checker %d': Process number: %d is finished \n", i, pid);
                exit(0);
            }
        }

        if (fork() == 0) { //The printer process
            int pid = getpid();
            printer(printerCann, numToPrint);
            sleep(pid);
            printf("'Printer': Process number: %d is finished \n\n", pid);
            exit(0);
        }

        generator(generatorCann);
        


        // Wait for all children
        for (int i = 0; i < numOfCheckers+2 ; i++) {
            wait(0);
        }

        char input; 
        char inputBuff[2]; 
        printf("The program is finish.\nDo you want start the system again? [y/n] \n");
        read(0, &inputBuff, sizeof(inputBuff));
        input = inputBuff[0];

        if (input == 'n')
            break;

   }
    
    printf("'Generator': Process number: %d is finished \n", getpid);
    
    exit(0);
}