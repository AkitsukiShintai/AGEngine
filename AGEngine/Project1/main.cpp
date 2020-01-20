#include <vector>
#include <iostream>
#include <list>
using namespace std;
#define  max(x,y)   (x)>(y)?(x):(y) 
struct Good {
	int weight;
	int value;
	Good() {
		weight = 0;
		value = 0;
	}
};


int GetValue(vector<Good> & goods, int weight) {
	if (goods.size() == 0)
	{
		return 0;
	}
	vector<vector<int>> s;
	
	s.resize(goods.size() + 1);
	for (auto&t : s)
	{
		t.resize(weight + 1);
	}

	for (int i = 1; i <= goods.size() ; i++)
	{
		int currentValue = goods[i - 1].value;
		int currentWeight = goods[i - 1].weight;
		for (int j = 1; j <= weight; j++)
		{
			if (currentWeight <= j)
			{
				s[i][j] = max(s[i-1][j],s[i-1][j-currentWeight]+currentValue );
			}
			else
			{
				s[i][j] = s[i - 1][j];
			}

		}
	}

	return s[goods.size()][weight];


}

int GetMinNumberOfCoins(const vector<int>& conins,int n,vector<int>& result) {
	result.clear();
	result.resize(n + 1);
	vector<int> values;
	values.resize(n + 1);

	for (int i = 1; i<= n; i++)
	{
		int minNumber = i;
		int currentConin = 0;
		for (int j = 0; j < conins.size(); j ++)
		{
			int coinValue = conins[j];
			if (i >= coinValue)
			{
				if (values[i-coinValue] + 1 <= minNumber)
				{
					if (i - coinValue == 0 || result[i - coinValue] != 0)
					{
						minNumber = values[i - coinValue] + 1;
						currentConin = coinValue;
					}

				}
				


			}
			
		}
		result[i] = currentConin;
		values[i] = minNumber;
	}
	return values[n];

}


class B {
public:
	B(int i_i) { i = i_i; }
	B(const B& a) :i(a.i) {}
	B& operator = (const B& rh) { i = rh.i; return *this; }
private:
	int i;
};


class D :public B {
	int* p;
public:
	D(const D& a) : B(a),p(new int(21) ){}
	D(int a):B(a) {}
	D& operator = (const D& other) {
		B::operator=(other);
		return *this;
	}
};



void DDDDD(std::list<int>& t) {
	auto b = t.begin();
	for (; b!= t.end(); b++)
	{
		if (*b % 2 == 0)
		{
			b = t.insert(b, *b);
			b++;
			b++;
		}
	}

	for (auto& tt : t)
	{
		std::cout << tt << " ";
	}

}



int main1() {
	D d(0);
	D dddd(1);
	dddd = d;
	//int nn = 0;
	//int weight = 0;
	//cin >> nn;//n goods
	//cin >> weight;//package weight
	//vector<Good> t;
	//t.resize(nn);
	//for (int i = 0; i < nn ; i++)
	//{
	//	Good g;
	//	cin >> g.weight >> g.value;
	//	t[i] = g;
	//}
	//int i = GetValue(t, weight);

	vector<int> coins = {1,3,10};
	vector<int >result;
	int minCoins = GetMinNumberOfCoins(coins, 14, result);

	list<int> ddd = { 1,2,3,4,5};
	DDDDD(ddd);
	return 0;
}