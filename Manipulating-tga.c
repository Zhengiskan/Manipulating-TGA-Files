#include <stdio.h>

int main()
{
    FILE *ptr;
    ptr = fopen("sample1.tga", "wb");
    if (ptr == NULL){
        printf("ERROR, UNABLE TO OPEN FILE.");
        fclose(ptr);
    }
    
    return 0;
}
