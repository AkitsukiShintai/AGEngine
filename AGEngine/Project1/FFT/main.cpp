#include "FFT2.h"
#include <iostream>
int main() {
	const int row = 32;
	const int clo = 16;
	Grid<std::complex<float>> grid(row, clo);
	FFT2<float> fft(log2(row), log2(clo));
	IFFT2<float> ifft(log2(row), log2(clo));
	for (int i =0;i< row;i++)
	{
		for (int j = 0; j < clo; j++)
		{
			grid(i, j) = i* row +j;
		}
	}
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < clo; j++)
		{
			std::cout << grid(i, j)<<std::endl;
		}
	}
	fft(grid);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < clo; j++)
		{
			std::cout << grid(i, j) << std::endl;
		}
	}
	ifft(grid);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < clo; j++)
		{
			std::cout << grid(i, j) << std::endl;
		}
	}

	frequencyVector(grid, 1, 2);
}