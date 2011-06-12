#include "Poco/HashMap.h"

using Poco::HashMap;

void testHashTable() {
	
	for(int i=0; i<100000; ++i) {
		HashMap<int, int> map(200);
		for(int j=0; j<100; ++j) {
			map.insert(HashMap<int, int>::PairType(j, j));
		}
		map.clear();

		//HashMap<int, int>::Iterator it1 = map.find(1), it2 = map.find(9);
		//map.erase(it1);
		//map.erase(it2);
		//poco_assert(map.size() == 98 && map.find(1) == map.end() && map.find(9) == map.end());
//		poco_assert(map.empty());
	}
}

void testVector() {

	for(int i=0; i<100000; ++i) {
		vector<int> v;
		for(int j=0; j<100; ++j) {
			v.push_back(j);
		}
		v.clear();

		//HashMap<int, int>::Iterator it1 = map.find(1), it2 = map.find(9);
		//map.erase(it1);
		//map.erase(it2);
		//poco_assert(map.size() == 98 && map.find(1) == map.end() && map.find(9) == map.end());
		//		poco_assert(map.empty());
	}

	for(int i=0; i<1000000; ++i) {
		vector<int> v;
		v.reserve(100);
		for(int j=0; j<100; ++j) {
			v.push_back(j);
		}
		v.clear();
	}
}