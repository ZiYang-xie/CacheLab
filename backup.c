/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);

void transpose_4x4(int M, int N, int A[N][M], int B[M][N]);
void transpose_32x32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64x64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61x67(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M == 32 && N == 32)
        transpose_32x32(M, N, A, B);    
    else if(M == 64 && N == 64)
        transpose_32x32(M, N, A, B);
	else if(M == 61 && N == 67)
		transpose_61x67(M, N, A, B);
		
}

void transpose_4x4(int M, int N, int A[N][M], int B[M][N])
{
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

	for(int x = 0; x < 3; x += 2)
	{
		tmp0 = A[x][0], tmp1 = A[x][1], tmp2 = A[x][2], tmp3 = A[x][3];
		tmp4 = A[x + 1][0], tmp5 = A[x + 1][1], tmp6 = A[x + 1][2], tmp7 = A[x + 1][3];

		B[0][x] = tmp0, B[1][x] = tmp1, B[2][x] = tmp2, B[3][x] = tmp3; 
		B[0][x + 1] = tmp4, B[1][x + 1] = tmp5, B[2][x + 1] = tmp6, B[3][x + 1] = tmp7; 
	}
}

void transpose_32x32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, x, y;
    int tmp, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for(i = 0; i < N; i += 4)
        for(j = 0; j < M; j += 4)
			for(x = i; x < i + 4; ++x)
			{
				if(i == j)
				{
					tmp0 = A[x][j], tmp1 = A[x][j + 1], tmp2 = A[x][j + 2], tmp3 = A[x][j + 3];
					tmp4 = A[x][j + 4], tmp5 = A[x][j + 5], tmp6 = A[x][j + 6], tmp7 = A[x][j + 7];

					B[j][x] = tmp0, B[j + 1][x] = tmp1, B[j + 2][x] = tmp2, B[j + 3][x] = tmp3; 
					B[j + 4][x] = tmp4, B[j + 5][x] = tmp5, B[j + 6][x] = tmp6, B[j + 7][x] = tmp7; 
				}
				else 
				{
					for(y = j; y < j + 4; ++y)
					{
						tmp = A[x][y], B[y][x] = tmp;
					}
				}
			}
}

void transpose_64x64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, x, y, z;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for(i = 0; i < N; i += 8)
        for(j = 0; j < M; j += 8)
        {
			for(x = i; x < i + 4; ++x)
			{
				tmp0 = A[x][j], tmp1 = A[x][j + 1], tmp2 = A[x][j + 2], tmp3 = A[x][j + 3];
				tmp4 = A[x][j + 4], tmp5 = A[x][j + 5], tmp6 = A[x][j + 6], tmp7 = A[x][j + 7];

				B[j][x] = tmp0, B[j + 1][x] = tmp1, B[j + 2][x] = tmp2, B[j + 3][x] = tmp3; 
				B[j][x + 4] = tmp4, B[j + 1][x + 4] = tmp5, B[j + 2][x + 4] = tmp6, B[j + 3][x + 4] = tmp7; 
			}
			for(y = j; y < j + 4; ++y)
			{
				tmp0 = A[i + 4][y], tmp1 = A[i + 5][y], tmp2 = A[i + 6][y], tmp3 = A[i + 7][y];
				tmp4 = B[y][i + 4], tmp5 = B[y][i + 5], tmp6 = B[y][i + 6], tmp7 = B[y][i + 7];

				B[y][i + 4] = tmp0, B[y][i + 5] = tmp1, B[y][i + 6] = tmp2, B[y][i + 7] = tmp3;
				B[y + 4][i] = tmp4, B[y + 4][i + 1] = tmp5, B[y + 4][i + 2] = tmp6, B[y + 4][i + 3] = tmp7;
			}
			for(z = i + 4; z < i + 8; z += 2)
			{
				tmp0 = A[z][j + 4], tmp1 = A[z][j + 5], tmp2 = A[z][j + 6], tmp3 = A[z][j + 7];
				tmp4 = A[z + 1][j + 4], tmp5 = A[z + 1][j + 5], tmp6 = A[z + 1][j + 6], tmp7 = A[z + 1][j + 7];

				B[j + 4][z] = tmp0, B[j + 5][z] = tmp1, B[j + 6][z] = tmp2, B[j + 7][z] = tmp3;
				B[j + 4][z + 1] = tmp4, B[j + 5][z + 1] = tmp5, B[j + 6][z + 1] = tmp6, B[j + 7][z + 1] = tmp7;
			}
		}
}

void transpose_61x67(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, x, y, tmp;

    for(i = 0; i < N; i += 17)
        for(j = 0; j < M; j += 17)
			for(x = i; x < N && x < i + 17; ++x)
				for(y = j; y < M && y < j + 17; ++y)
				{
					tmp = A[x][y];
					B[y][x] = tmp; 
				}
			
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

