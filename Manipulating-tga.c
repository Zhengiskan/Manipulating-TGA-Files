#include <stdio.h>

int main()
{
    char buffer[10];
    FILE *ptr;
    ptr = fopen("sample1.tga", "wb");
    if (ptr == NULL) {
        printf("ERROR, UNABLE TO OPEN FILE.");
        fclose(ptr);
    }
    fread(buffer,sizeof(buffer),1,ptr);
    for(int i = 0; i<10; i++)
        printf("%u ", buffer[i]);

    return 0;
}