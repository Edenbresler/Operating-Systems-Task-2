#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0) {
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        else{
            sleep(2);
            printf("put 42 data in channel\n"); 
        }
        //channel_put(cd, 43); // Sleeps until cleared
        if(channel_put(cd, 43)<0){
            printf("Failed to put 43 data in channel\n");
            exit(1);
        }
        else{
            sleep(5);
            printf("put 43 data in channel\n"); 
        }
        if(channel_destroy(cd)<0){
            printf("Failed to put destroy\n");
            exit(1);
        }
        
    } else {
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data42 from channel\n");
            exit(1);
        }
        else {
            sleep(7);
             printf(" take data %d from channel\n", data);
        }
        data = channel_take(cd, &data); // 43
        if(data <0){
            printf("Failed to take data43 from channel\n");
            exit(1);
        }
        if(data == 0){
            sleep(10);
            printf(" take data %d from channel\n", data);
            //exit(1);
        }

        data = channel_take(cd, &data); // Sleep until child destroys channel
         if(data <0){
            printf("Failed to take data from channel\n");
            exit(1);
        }
    }

    return 0;
}
