#include "knapsack_brute_force_minchange.h"
#include <string.h>
//#include <iostream>

#ifdef __GNUC__
#  define ffs(x) (__builtin_ffs(x) - 1)
#elif __INTEL_COMPILER
#  define ffs(x) _bit_scan_forward(x)
#endif

#ifdef _WIN32
int getFirstSetBitPos(int n)
{
	if (n == 0)
	{
		return 0;
	}
	return log2(n);
}
# define ffs(x) getFirstSetBitPos(x)
#endif

GreyCode::GreyCode(int s):lastValue(1 << s), printValue(1), lastOne(0)
{
}

////////////////////////////////////////////////////////////////////////////////
std::pair< bool, std::pair< bool, int > > GreyCode::Next() {
	int half = printValue >> 1; //integer division uses floor for positive numbers
	int gray_code = printValue ^ half;
	int compare = gray_code ^ lastOne;
	int index = ffs(compare);
  
	int valueAtIndex = (gray_code & (1 << index)) != 0 ? 1 : 0;

	lastOne = gray_code;
	printValue++;
	int  pos = index;// which position is modified 
	bool add = valueAtIndex == 1; // is true if new value is 1 (add item), false otherwise 
	bool last = printValue == lastValue;// is this the last permutation 

    return std::make_pair( !last, std::make_pair( add, pos ) );
}

////////////////////////////////////////////////////////////////////////////////
std::vector<bool> knapsack_brute_force( std::vector<Item> const& items, Weight const& W )
{
	int currentValue = 0;
	Weight currentWeight;
	int maxValue = 0;
	std::vector<bool> result;
	std::vector<bool> record;
	record.resize(items.size(), false);
	result.resize(items.size(), false);
	GreyCode greyCode(items.size());

	bool go = true;
	while (go)
	{
		//std::cout << " current value:       " << currentValue <<  "current weight: " << currentWeight << std::endl;
		std::pair< bool, std::pair< bool, int > > r = greyCode.Next();
		go = r.first;
		bool add = r.second.first;
		int  pos = r.second.second;

		if (add)
		{
			//std::cout << "Add at position: " << pos << " value: " << items[pos].GetValue() << " weight:"  << items[pos].GetWeight() << std::endl;

			record[pos] = true;
			currentWeight = currentWeight + items[pos].GetWeight();
			currentValue = currentValue + items[pos].GetValue();
			if (currentWeight > W)
			{
				
				continue;
			}

			if (currentValue > maxValue)
			{

				maxValue = currentValue;
				result = record;
			}

		}
		else
		{
			//std::cout << "Minus at position: " << pos << " value: " << items[pos].GetValue() << " weight:" << items[pos].GetWeight() << std::endl;

			record[pos] = false;
			currentWeight = currentWeight - items[pos].GetWeight();
			currentValue = currentValue - items[pos].GetValue();
			if (currentWeight > W)
			{

				continue;
			}

			if (currentValue > maxValue)
			{

				maxValue = currentValue;
				result = record;
			}
		}
	}
	return result;
}
