#include <stdio.h>

#define SIZE 512

typedef unsigned char uchar;

void first_order_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE]);
void second_order_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE]);
void canny_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE]);
void process(uchar (*out)[SIZE], const uchar (*in)[SIZE]);


int main(void)
{
    FILE *fp;
    uchar s1[SIZE][SIZE];
    uchar s2[SIZE][SIZE];
    uchar a1[SIZE][SIZE];
    uchar a2[SIZE][SIZE];
    uchar a3[SIZE][SIZE];
    uchar b[SIZE][SIZE];



    fp = fopen("sample1.raw", "rb");
    fread(s1, sizeof(uchar), sizeof(s1), fp);
    fclose(fp);
    fp = fopen("sample2.raw", "rb");
    fread(s2, sizeof(uchar), sizeof(s2), fp);
    fclose(fp);

    first_order_edge(a1, (const uchar (*)[SIZE])s1);
    second_order_edge(a2, (const uchar (*)[SIZE])s1);
    canny_edge(a3, (const uchar (*)[SIZE])s1);
    process(b, (const uchar (*)[SIZE])s2);


    fp = fopen("a1.raw", "wb");
    fwrite(a1, sizeof(uchar), sizeof(a1), fp);
    fclose(fp);
    fp = fopen("a2.raw", "wb");
    fwrite(a2, sizeof(uchar), sizeof(a2), fp);
    fclose(fp);
    fp = fopen("a3.raw", "wb");
    fwrite(a3, sizeof(uchar), sizeof(a3), fp);
    fclose(fp);
    fp = fopen("b.raw", "wb");
    fwrite(b, sizeof(uchar), sizeof(b), fp);
    fclose(fp);


    return 0;
}

void first_order_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}

void second_order_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}

void canny_edge(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}

void process(uchar (*out)[SIZE], const uchar (*in)[SIZE])
{
}
