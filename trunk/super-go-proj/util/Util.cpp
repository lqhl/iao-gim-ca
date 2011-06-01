#include "util.h"

Poco::Util::PropertyFileConfiguration* Util::props = NULL;

int Util::getInt(const string& name){
	return props->getInt(name);
}

bool Util::getBoolean(const string& name) {
	return props->getBool(name);
}

double Util::getDouble(const string& name) {
	return props->getDouble(name);
}

string Util::get(const string& name) {
	return props->getString(name);
}

void Util::init(const string& fileName) {
	props = new PropertyFileConfiguration(fileName);
}

void Util::close() {
	//delete props;
}