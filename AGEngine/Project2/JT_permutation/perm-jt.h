#ifndef PERM_JOHNSON_TROTTER_H
#define PERM_JOHNSON_TROTTER_H
#include <vector>

class PermJohnsonTrotter {
	struct Pair {
		int first;
		int second;
		
	};
	std::vector<Pair> numbers;
	std::vector<int> result;
	int size;
	int endNumber;
	int currentNumber;
	public:
		PermJohnsonTrotter(int _size);
		bool Next();
		std::vector<int> const& Get() const;
};
#endif
