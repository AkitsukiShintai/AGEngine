//#define EULER_SECOND
#ifdef EULER_SECOND
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
using DifferentialFunc = float(*)(const Pair&);

float Function(float x) {
	return x / (1 + x * x);
}

float DiffeOfTheta(const Pair& inValue) {
	return inValue.y;
}

float DiffeOfW(const Pair& inValue) {
	return -(g/L)*sinf(inValue.x);
}

float CalculateRatio(float appro, float actu) {
	return abs(appro - actu) / abs(actu);
}

Pair EulerIntegration(const Pair& inValue, float h, DifferentialFunc diffFuncX, DifferentialFunc diffFuncY) {
	Pair result;
	result.x = inValue.x + h * diffFuncX(inValue);
	result.y = inValue.y + h * diffFuncY(inValue);
	result.t = inValue.t + h ;
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
		inValue = EulerIntegration(inValue, 0.1f, DiffeOfTheta, DiffeOfW);
		std::cout << inValue.t << " " << inValue.x << " " << CalculateE(inValue) << std::endl;
	}

	return 0;
}
#endif