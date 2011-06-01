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
#include <string>

using std::string;
using Poco::Util::PropertyFileConfiguration;
using Poco::Logger;


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
};
#endif