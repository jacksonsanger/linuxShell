#include <stdio.h>

int main () {
    char buffer[256];

    //simply reads a string from scanf
    while (scanf("%s", buffer) != EOF){
        printf("Entered in string: %s\n", buffer);
    }

    return 0;
}