#ifndef COUNTER_H
#define COUNTER_H

#include <string>
#include <map>
using std::string;
using std::map;

class Counter{
public:
	int value;

	void inc();
	void dec();
	void set();
	void get();

	Counter();

	static map<string, Counter> counters;

	static Counter& getCounter(const string& name);

	static int getValue(const string& name);

};
#endif