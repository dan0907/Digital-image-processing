#include <stdio.h>

#define SIZE 256

typedef unsigned char uchar;

void edge_crispening(uchar (*out)[SIZE], const uchar (*in)[SIZE]);
void warping(uchar (*out)[SIZE], const uchar (*in)[SIZE]);


int main(void)
{
    FILE *fp;
    uchar s3[SIZE][SIZE];
    uchar C[SIZE][SIZE];
    uchar D[SIZE][SIZE];


    fp = fopen("sample3.raw", "rb");
    fread(s3, sizeof(uchar), sizeof(s3), fp);
    fclose(fp);

    edge_crispening(C, (const uchar (*)[SIZE])s3);
    warping(D, (const uchar (*)[SIZE])C);


    fp = fopen("C.raw", "wb");
    fwrite(C, sizeof(uchar), sizeof(C), fp);
    fclose(fp);
    fp = fopen("D.raw", "wb");
    fwrite(D, sizeof(uchar), sizeof(D), fp);
    fclose(fp);
    return 0;
}


void edge_crispening(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}

void warping(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}
