#include <stdlib.h>
#include <stdio.h>
#include "tqueue.h"

int main(int argc, char*argv[]){
printf("Hello test\n");
TQueue* list = NULL;
//add 10 numbers to the list
for(int value = 0;value<10;value++){
tqueue_enqueue(list,(void*)&value);
}
return 0;
}
