#ifndef UTIL_H
#define UTIL_H

#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Util/Timer.h"
#include "Poco/Random.h"
#include <string>
#include <cstdio>

using std::string;
using Poco::Util::PropertyFileConfiguration;
using Poco::Logger;
using Poco::Random;


// this class contains the loggers and the configuration files
class Util{
public:
	static Logger* threadLogger;

	static PropertyFileConfiguration* props;

	static int getInt(const string& name);

	static bool getBoolean(const string& name);

	static double getDouble(const string& name);

	static string get(const string& name);

	static void init(const string& fileName);

	static void close();

	static Logger& getUctLogger() {
		static Logger* logger = NULL;
		if (logger == NULL) logger = &Logger::get("UctLogger");
		return *logger;
	}

	static Logger& getTreeLogger() {
		static Logger* logger = NULL;
		if (logger == NULL) logger = &Logger::get("TreeLogger");
		return *logger;
	}


	static Logger& getSearchLogger() {
		static Logger* logger = NULL;
		if (logger == NULL) logger = &Logger::get("SearchLogger");
		return *logger;
	}

	static bool SearchDebugEnabled() {
		static bool enabled = props->getBool("SearchDebug");
		return enabled;
	}

	static bool TreeDebugEnabled() {
		static bool enabled = props->getBool("TreeDebug");
		return enabled;
	}

	static bool UctDebugEnabled() {
		static bool enabled = props->getBool("UctDebug");
		return enabled;
	}

	static FILE* LogFile() {
		static FILE* file = fopen("super-go-log.txt", "w");
		return file;
	}

};
#endif
