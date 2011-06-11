#include <iostream>
#include <stdio.h>
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Mutex.h"
#include "Poco/Util/LoggingConfigurator.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/Debugger.h"
#include "util.h"


using Poco::Logger;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::ConsoleChannel;
using Poco::FileChannel;
using Poco::Message;
using Poco::Util::LoggingConfigurator;
using Poco::Util::PropertyFileConfiguration;
using Poco::Debugger;

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
	remove("super-go.log");
	props = new PropertyFileConfiguration(fileName);

	PropertyFileConfiguration *logProps = new PropertyFileConfiguration("poco-logging.config");
	LoggingConfigurator configrator;
	configrator.configure(logProps);
}

void Util::close() {
	//delete props;
}