#include "perm-jt.h"

PermJohnsonTrotter::PermJohnsonTrotter(int _size) :numbers(),result(),size(_size),endNumber(0),currentNumber(1)
{
	if (_size <= 0)
	{
		currentNumber = endNumber = 0;
		return;
	}
	numbers.resize(size);
	result.resize(size);
	for (int i = 0; i < size; i++)
	{
		numbers[i].first = i +1;
		numbers[i].second = -1;

		result[i] = i + 1;
	}
	endNumber = 1;
	for (int i = 1; i <= size; i++)
		endNumber = endNumber * i;
}

bool PermJohnsonTrotter::Next()
{
	//get largest mobile number
	int mobile = 0; int mobileValue = 0;
	if (currentNumber == endNumber)
	{
		return false;
	}
	for (int i = 0; i < size; i++)
	{
		//the next one is i-1
		if (numbers[i].second < 0)
		{
			if (i - 1 >= 0 && numbers[i].first > numbers[i - 1].first)
			{
				if (numbers[i].first > mobileValue)
				{
					mobile = i;
					mobileValue = numbers[i].first;
				}
			}
		}
		else//the next one is i + 1
		{
			if (i + 1 < size && numbers[i].first > numbers[i + 1].first)
			{
				if (numbers[i].first > mobileValue)
				{
					mobile = i;
					mobileValue = numbers[i].first;
				}
			}
		}
	}
	//swap mobile and next one
	std::swap(numbers[mobile], numbers[mobile + numbers[mobile].second]);

	// changing the directions for elements greater than largest mobile
	for (int i = 0; i < size; i++)
	{
		if (numbers[i].first > mobileValue)
		{
			numbers[i].second = -numbers[i].second;
		}

		result[i] = numbers[i].first;
	}

	//set to result
	currentNumber++;

	return true;
}

std::vector<int> const& PermJohnsonTrotter::Get() const
{
	return result;
}
