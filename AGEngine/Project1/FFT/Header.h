#pragma once
#include <glm/glm.hpp>
#include "Grid.h"
#include <complex>
template<class T>
glm::vec2 frequencyVector(const Grid<T>& B, int p, int q) {
	assert(p >= 0 && p <= B.cellWidth());
	assert(q >= 0 && q <= B.cellHeight());

	int gridWidth = B.width();//L
	int gridHeight = B.height();

	int midW = B.cellWidth() / 2;// N/2
	int midH = B.cellHeight() / 2;
	glm::vec2 result;
	if (p> midW)
	{
		result.x = (float)p / gridWidth;
	}
	else
	{
		result.x = (float)(p- B.cellWidth()) / gridWidth;
	}

	if (q > midH)
	{
		result.y = (float)q / gridHeight;
	}
	else
	{
		result.y = (float)(q - B.cellHeight()) / gridHeight;
	}

	return result;
}

template<class R>
class FFT2 {
	FFT_base<R> w;
	FFT_base<R> h;
public:
	FFT2(int lgW, int lgH);
	~FFT2();
	void operator()(Grid < std::complex<R>> & A);

};

template<class R>
FFT2<R>::FFT2(int lgW, int lgH):w(lgW,false), h(lgH, false)
{

}

template<class R>
FFT2<R>::~FFT2()
{
}
template<class R>
void FFT2<R>::operator()(Grid < std::complex<R>> & A) {
	std::vector<std::complex<R>> arr;
	arr.resize(A.cellWidth());
	for (int i = 0; i <A.cellHeight(); i ++)
	{
		for (int j = 0; j < A.cellWidth(); j++)
		{
			arr[j] = A(j, i);
		}
		w(&arr[0]);
		for (int j = 0; j < A.cellWidth(); j++)
		{
			A(j, i) = arr[j];
		}
	}
	arr.resize(A.cellHeight());
	for (int i = 0; i < A.cellWidth(); i++)
	{
		for (int j = 0; j < A.cellHeight(); j++)
		{
			arr[j] = A(i, j);
		}
		w(&arr[0]);
		for (int j = 0; j < A.cellHeight(); j++)
		{
			A(i, j) = arr[j];
		}
	}
}

template<class R>
class IFFT2 {
	FFT_base<R> w;
	FFT_base<R> h;
public:
	IFFT2(int lgW, int lgH);
	~IFFT2();
	void operator()(Grid < std::complex<R>>& A);
};

template<class R>
IFFT2<R>::IFFT2(int lgW, int lgH) :w(lgW, true), h(lgH, true)
{

}

template<class R>
IFFT2<R>::~IFFT2()
{
}
template<class R>
void IFFT2<R>::operator()(Grid < std::complex<R>>& A) {
	std::vector<std::complex<R>> arr;
	arr.resize(A.cellWidth());
	for (int i = 0; i < A.cellHeight(); i++)
	{
		for (int j = 0; j < A.cellWidth(); j++)
		{
			arr[j] = A(j, i);
		}
		w(&arr[0]);
		for (int j = 0; j < A.cellWidth(); j++)
		{
			A(j, i) = arr[j];
		}
	}
	arr.resize(A.cellHeight());
	for (int i = 0; i < A.cellWidth(); i++)
	{
		for (int j = 0; j < A.cellHeight(); j++)
		{
			arr[j] = A(i, j);
		}
		w(&arr[0]);
		for (int j = 0; j < A.cellHeight(); j++)
		{
			A(i, j) = arr[j];
		}
	}
}