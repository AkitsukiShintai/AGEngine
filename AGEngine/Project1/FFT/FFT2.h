#ifndef FFT2_H
#define FFT2_H
#include "glm/glm.hpp"
#include "Grid.h"
#include "FFT.h"
#include <complex>
template<class T>
glm::vec2 frequencyVector(const Grid<T>& B, int p, int q) {

	float wL = B.width() * B.cellWidth();//L
	float hL = B.height() * B.cellHeight();

	float midW = B.width() / 2;// N/2
	float midH = B.height() / 2;
	glm::vec2 result;
	if (p > midW&& p < B.width())
	{
		result.x = (float)(p - B.width()) / wL;
	}
	else
	{
		result.x = (float)p / wL;
	}

	if (q > midH&& q < B.height())
	{
		result.y = (float)(q - B.height()) / hL;

	}
	else if (q >= 0)
	{
		result.y = (float)q / hL;
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
	void operator()(Grid < std::complex<R>>& A);

};

template<class R>
FFT2<R>::FFT2(int lgW, int lgH) :w(lgW, false), h(lgH, false)
{

}

template<class R>
FFT2<R>::~FFT2()
{
}
template<class R>
void FFT2<R>::operator()(Grid < std::complex<R>>& A) {
	std::vector<std::complex<R>> arr;
	arr.resize(A.height());
	for (int i = 0; i < A.width(); i++)
	{
		for (int j = 0; j < A.height(); j++)
		{
			arr[j] = A(i, j);
		}
		std::complex<R>* transform = h(&arr[0]);
		for (int j = 0; j < A.height(); j++)
		{
			A(i, j) = transform[j];
		}
	}
	arr.resize(A.width());
	for (int i = 0; i < A.height(); i++)
	{
		for (int j = 0; j < A.width(); j++)
		{
			arr[j] = A(j, i);
		}
		std::complex<R>* transform = w(&arr[0]);
		for (int j = 0; j < A.width(); j++)
		{
			A(j, i) = transform[j];
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

	/*for (int i = 0; i< A.width();i++)
	{
		arr[i].resize(A.height());
	}
	for (int r = 0; r <= A.width()/2; ++r)
		for (int c = 0; c <= A.height()/2; ++c) {
			arr[r][c] = A(r,c);
			float f = sqrt(r * r + c * c);
			if (f != 0)
				arr[r][c] *= 1.0f / pow(f,0.5f);
		}
	for (int r = 0; r <= A.width()/2; ++r)
		for (int c = 0; c <= A.width()/2; ++c) {
			if (c != 0)
				arr[r][A.width() - c] = -arr[r][c];
			if (r != 0)
				arr[A.width() - r][c] = -arr[r][c];
			if (r != 0 && c != 0)
				arr[A.width() - r][A.width() - c] = -arr[r][c];
		}*/


	std::vector<std::complex<R>> temp;

	temp.resize(A.width());
	for (int i = 0; i < A.height(); i++)
	{
		for (int j = 0; j < A.width(); j++)
		{
			temp[j] = A(j, i);
		}
		std::complex<R>* transform = w(&temp[0]);
		for (int j = 0; j < A.width(); j++)
		{
			A(j, i) = transform[j];
		}
	}

	temp.resize(A.height());
	for (int i = 0; i < A.width(); i++)
	{
		for (int j = 0; j < A.height(); j++)
		{
			temp[j] = A(i, j);
		}
		std::complex<R>* transform = h(&temp[0]);
		for (int j = 0; j < A.height(); j++)
		{
			A(i, j) = transform[j];
		}
	}



}
#endif