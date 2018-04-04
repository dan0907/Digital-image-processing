#include <stdio.h>
#include <stdlib.h>

#define SIZE 256

unsigned char rgb_to_gray(unsigned char r, unsigned char g, unsigned char b)
{
    return 0.299 * r + 0.587 * g + 0.114 * b;
}

int main(void)
{
    unsigned char data[3][SIZE][SIZE];
    unsigned char grayscale_data[SIZE][SIZE];
    unsigned char grayscale_flipping_data[SIZE][SIZE];
    FILE *fp;
    int i, j;
    if ((fp = fopen("sample1.raw", "rb")) == NULL) { 
        printf("Error\n");
        exit(1);
    }
    fread(data, sizeof(unsigned char), sizeof(data), fp);
    fclose(fp);

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            grayscale_data[i][j] = rgb_to_gray(data[0][i][j],
                    data[1][i][j], data[2][i][j]);
        }
    }

    fp = fopen("grayscale.raw", "wb");

    fwrite(grayscale_data, sizeof(unsigned char), sizeof(grayscale_data), fp);
    fclose(fp);

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            grayscale_flipping_data[j][i] = grayscale_data[i][j];

    fp = fopen("B.raw", "wb");	

    fwrite(grayscale_flipping_data, sizeof(unsigned char), sizeof(grayscale_flipping_data), fp);
    fclose(fp);


    return 0;
}	
