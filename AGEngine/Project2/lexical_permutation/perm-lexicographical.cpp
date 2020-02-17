#include "perm-lexicographical.h"
#include <algorithm>
#include <iostream>

bool nextPermLexicographical (std::vector<int> & p) {
	//std::sort(p.begin(), p.end());
	if (p.size() <= 1)
	{
		return false;
	}
	int number = p.size();
	int checkP = -1;
	for (int i = number - 2; i >=0 ; --i)
	{
		int last = i + 1;
		if (p[i] < p[last])
		{
			checkP = i;
			break;
		}		
	}
	if (checkP == -1)
	{
		return false;
	}
	int checkP2 = -1;
	for (int i = number - 1;i > checkP; --i)
	{
		if (p[i] > p[checkP])
		{
			checkP2 = i;
			break;
		}
	}
	std::swap(p[checkP], p[checkP2]);
	int add = checkP + number;
	int mid = (checkP + number) / 2;
	for (int i = checkP+1 ;i<= mid;i ++)
	{
		int swapId = add - i;
		std::swap(p[i], p[swapId]);
	}
	return true;
}
