//#define HEUN_SECOND
#ifdef HEUN_SECOND
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
float g = 1.0f;
float L = 1.0f;
float PI = 3.14159265359f;
struct Pair {
	float x;
	float y;
	float t;
	void Print() {
		std::cout << x << " | " << y;
	}
};
using Func = float(*)(float);
using DifferentialFunc = float(*)(float);

float Function(float x) {
	return x / (1 + x * x);
}

float DiffeOfTheta(float w) {
	return w;
}

float DiffeOfW(float theta) {
	return -(g / L) * sinf(theta);
}

float CalculateRatio(float appro, float actu) {
	return abs(appro - actu) / abs(actu);
}

Pair HeunIntegration(const Pair& inValue, float h, DifferentialFunc diffFuncX, DifferentialFunc diffFuncY) {
	Pair result;
	result.y = inValue.y + h * diffFuncY(inValue.x);//t1 - w
	result.x = inValue.x + h * diffFuncX((result.y + inValue.y) * 0.5f);
	result.t = inValue.t + h;
	return result;
}

float CalculateE(const Pair& inValue) {
	return 0.5f * L * inValue.y * inValue.y + g * (1 - cosf(inValue.x));
}

int main() {

	Pair inValue = { PI / 4.0f,0 , 0 };// theta, w , t
	std::cout << std::fixed << std::setprecision(4) << 0.0f;
	std::cout << inValue.x << " " << CalculateE(inValue) << std::endl;
	for (float i = 0.0f; i <= 1.9f; i += 0.1f)
	{
		inValue = HeunIntegration(inValue, 0.1f, DiffeOfTheta, DiffeOfW);
		std::cout << inValue.t << " " << inValue.x << " " << CalculateE(inValue) << std::endl;
	}
	return 0;
}
#endif