#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

#define SIZE 256

/* Box-Muller transform algorithm */
double generate_gaussian_noise(double mu, double sigma)
{
    static const double epsilon = DBL_MIN;
    static const double two_pi = 2 * 3.14159265358979323846;
    static double z1;
    static int generate;

    double u1, u2, z0;

    generate = !generate;

    if (!generate)
        return z1 * sigma + mu;

    do {
        u1 = (double)rand() / RAND_MAX;
        u2 = (double)rand() / RAND_MAX;
    } while (u1 <= epsilon);


    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);

    return z0 * sigma + mu;
}

void salt_and_pepper(unsigned char (*s)[SIZE], unsigned char (*data)[SIZE], double threshold)
{
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            double num = (double)rand() / RAND_MAX;
            if (num < threshold)
                s[i][j] = 0;
            else if (num > 1 - threshold)
                s[i][j] = 255;
            else
                s[i][j] = data[i][j];	
        }
    }
}

double MSE(unsigned char (*a)[SIZE], unsigned char (*b)[SIZE])
{
    int i, j;
    double total = 0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            total += pow(a[i][j] - b[i][j], 2);	
        }
    }
    return total / (SIZE * SIZE);
}

void apply_gaussian_noise(unsigned char (*g)[SIZE], unsigned char (*data)[SIZE], double mu, double sigma) {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            double tmp = data[i][j] + generate_gaussian_noise(mu, sigma);
            if (tmp < 0)
                g[i][j] = 0;
            else if (tmp > 255)
                g[i][j] = 255;
            else
                g[i][j] = tmp;
        }
    }
}

int symmetry(int n)
{
    if (n < 0)
        return -n + 1;
    if (n >= SIZE)
        return SIZE * 2 - 1 - n;
    return n;
}

void low_pass_filter(unsigned char (*rg)[SIZE], unsigned char (*data)[SIZE])
{
    int side_length = 3;
    int half = side_length / 2;

    double b = 1.5;

    double denominator = pow(b + 2, 2);
    double tmp1 = 1 / denominator;
    double tmp2 = tmp1 * b;
    double tmp3 = tmp2 * b;
    double window[3][3] = {
        {tmp1, tmp2, tmp1},
        {tmp2, tmp3, tmp2},
        {tmp1, tmp2, tmp1}
    };

    int i, j, k, l;
    int w_i, w_j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            double total = 0;
            for (k = i - half, w_i = 0; k <= i + half; k++, w_i++) {
                int x = symmetry(k);
                for (l = j - half, w_j = 0; l <= j + half; l++, w_j++) {
                    int y = symmetry(l);
                    total += window[w_i][w_j] * data[x][y];
                }
            }
            rg[i][j] = total;
        }
    }
}

int cmp(const void *a, const void *b)
{
    return (int)(*(unsigned char *)a) - *(unsigned char *)b;
}

void median_filter(unsigned char (*rs)[SIZE], unsigned char (*data)[SIZE], int side_length)
{
    int half = side_length / 2;
    int total_size = side_length * side_length;
    unsigned char *arr = malloc(sizeof(unsigned char) * total_size);
    int arr_i;
    int median = total_size / 2;

    int i, j, k, l;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            for (k = i - half, arr_i = 0; k <= i + half; k++) {
                int x = symmetry(k);
                for (l = j - half; l <= j + half; l++, arr_i++) {
                    int y = symmetry(l);
                    arr[arr_i] = data[x][y];
                }
            }
            qsort(arr, total_size, sizeof(unsigned char), cmp);
            rs[i][j] = arr[median];
        }
    }
    free(arr);
}

double PSNR(unsigned char (*a)[SIZE], unsigned char (*b)[SIZE])
{
    return 10 * log10(255 * 255 / MSE(a, b));
}

int main(void)
{
    FILE *fp;
    unsigned char data[SIZE][SIZE];
    unsigned char g1[SIZE][SIZE];
    unsigned char g2[SIZE][SIZE];
    unsigned char s1[SIZE][SIZE];
    unsigned char s2[SIZE][SIZE];
    unsigned char rg[SIZE][SIZE];
    unsigned char rs[SIZE][SIZE];
    unsigned char res[SIZE][SIZE];

    srand(time(NULL));

    fp = fopen("sample3.raw", "rb");
    fread(data, sizeof(unsigned char), sizeof(data), fp);
    fclose(fp);

    apply_gaussian_noise(g1, data, 0, 10);
    apply_gaussian_noise(g2, data, 0, 30);

    fp = fopen("g1.raw", "wb");
    fwrite(g1, sizeof(unsigned char), sizeof(g1), fp);
    fclose(fp);

    fp = fopen("g2.raw", "wb");
    fwrite(g2, sizeof(unsigned char), sizeof(g2), fp);
    fclose(fp);


    salt_and_pepper(s1, data, 0.02);
    salt_and_pepper(s2, data, 0.1);

    fp = fopen("s1.raw", "wb");
    fwrite(s1, sizeof(unsigned char), sizeof(s1), fp);
    fclose(fp);

    fp = fopen("s2.raw", "wb");
    fwrite(s2, sizeof(unsigned char), sizeof(s2), fp);
    fclose(fp);

    low_pass_filter(rg, g1);


    fp = fopen("rg.raw", "wb");
    fwrite(rg, sizeof(unsigned char), sizeof(rg), fp);
    fclose(fp);

    median_filter(rs, s1, 3);


    fp = fopen("rs.raw", "wb");
    fwrite(rs, sizeof(unsigned char), sizeof(rs), fp);
    fclose(fp);

    printf("Rg: %.2fdB\n", PSNR(rg, g1));
    printf("Rs: %.2fdB\n", PSNR(rs, s1));

    fp = fopen("sample4.raw", "rb");
    fread(data, sizeof(unsigned char), sizeof(data), fp);
    fclose(fp);

    median_filter(res, data, 19);

    fp = fopen("res.raw", "wb");
    fwrite(res, sizeof(unsigned char), sizeof(res), fp);
    fclose(fp);

    return 0;
}

