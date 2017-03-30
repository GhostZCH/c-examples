#include <unordered_map>
#include <iostream>
#include <vector>

using namespace std;

struct CKey
{
	int a;
	int b;

	CKey(int a, int b) :a(a), b(b) {}
};


struct CValue
{
	time_t modify;
	time_t expired;
};

struct CKeyHash
{
	size_t operator ()(const CKey& k) const
	{
		return k.a;
	}
};

struct KeyEqual
{
	bool operator () (const CKey& x, const CKey& y) const
	{
		return x.a == y.a && y.b == x.b;
	}
};

typedef unordered_map<CKey, CValue, CKeyHash, KeyEqual> CMap;

CMap g_map;


int main()
{
	cout << "start" << endl;

	int count = 10 * 1024 * 1024;
	for (int i = 0; i < count; i++)
	{
		g_map[{ i, i }] = {i, i};
	}
	cout << g_map.size() << endl;

	vector<CKey> vecRemove;
	CMap::iterator iter;
	for (iter=g_map.begin() ; iter != g_map.end(); iter++)
	{
		if (iter->second.expired % 3 == 0)
		{
			vecRemove.push_back(iter->first);
		}
	}
	cout << vecRemove.size() << endl;

	vector<CKey>::iterator iterVec;
	for (iterVec = vecRemove.begin(); iterVec != vecRemove.end(); iterVec++)
	{
		g_map.erase(*iterVec);
	}
	cout << g_map.size() << endl;

	std::cout << "end" << endl;
    return 0;
}

