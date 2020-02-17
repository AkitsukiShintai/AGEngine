//#define EULER_FIRST
#ifdef EULER_FIRST
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

struct Pair {
	float x;
	float y;
	void Print() {
		std::cout << x << " | " << y;
	}
};
using Func = float(*)(float);
using DifferentialFunc = float(*)(const Pair&);

float Function(float x) {
	return x / (1 + x * x);
}

float FirstOrderDifferential(const Pair& inValue) {
	return (1 / (inValue.x * inValue.x) - 1) * inValue.y * inValue.y;
}

float CalculateRatio(float appro, float actu) {
	return abs(appro - actu) / abs(actu);
}

Pair EulerIntegration(const Pair& inValue, float h, DifferentialFunc diffFunc) {
	Pair result;
	result.y = inValue.y + h * diffFunc(inValue);
	result.x = inValue.x + h;
	return result;
}

int main() {
	Pair inValue = { 1.0f,0.5f };
	std::cout << std::fixed << std::setprecision(4);
	inValue.Print();
	std::cout << " | " << inValue.y << " | " << CalculateRatio(inValue.y, inValue.y) << std::endl;
	for (float i = 1.0f; i <= 9.5f; i += 0.5f)
	{
		inValue = EulerIntegration(inValue, 0.5f, FirstOrderDifferential);
		float actuallyY = Function(inValue.x);
		float ratio = CalculateRatio(inValue.y, actuallyY);
		inValue.Print();
		std::cout << " | " << actuallyY << " | " << ratio << std::endl;
	}

	return 0;
}
#endif