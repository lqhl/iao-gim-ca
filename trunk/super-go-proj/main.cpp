#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include <iostream>
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"
#include "Poco/Mutex.h"
#include "Poco/Util/LoggingConfigurator.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "UCTSearchRunner.h"
#include "util/Counter.h"
#include "Poco/Debugger.h"

using Poco::Logger;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::ConsoleChannel;
using Poco::FileChannel;
using Poco::Message;
using Poco::Mutex;
using Poco::Util::LoggingConfigurator;
using Poco::Util::PropertyFileConfiguration;
using Poco::Debugger;


static Mutex coutLock;

class HelloRunnable: public Poco::Runnable
{
	virtual void run()
	{
		for(int i=0; i<20; ++i) {
			coutLock.lock();
			std::cout << "Hello, world!" << std::endl;
			coutLock.unlock();
		}
	}

public:
	void speak() {
		std::cout << "Speak !" << std::endl;
	}
};

void testThread() {
	HelloRunnable runnable;
	Poco::Thread thread;
	thread.start(runnable);

	Poco::Thread thread2;
	HelloRunnable runnable2;
	thread2.start(runnable2);
	thread.join();
	thread2.join();

	std::cout << "again\n";

	thread.start(runnable2);
	thread.join();
}

void testLog() {
	PropertyFileConfiguration *props = new PropertyFileConfiguration("poco-logging.config");
	LoggingConfigurator configrator;
	configrator.configure(props);

	FormattingChannel* pFCConsole = new FormattingChannel(new PatternFormatter("%s: %p: %t"));
	pFCConsole->setChannel(new ConsoleChannel);
	pFCConsole->open();

	FormattingChannel* pFCFile = new FormattingChannel(new PatternFormatter("%Y-%m-%d %H:%M:%S.%c %N[%P]:%s:%q:%t"));
	pFCFile->setChannel(new FileChannel("sample.log"));
	pFCFile->open();

	// create two Logger objects - one for
	// each channel chain.
	Logger& consoleLogger = Logger::create("ConsoleLogger", pFCConsole, Message::PRIO_INFORMATION);
	Logger& fileLogger    = Logger::create("FileLogger", pFCFile, Message::PRIO_WARNING);

	// log some messages
	consoleLogger.error("An error message");
	fileLogger.error("An error message");

	consoleLogger.warning("A warning message");
	fileLogger.error("A warning message");

	consoleLogger.information("An information message");
	fileLogger.information("An information message");

	poco_information(consoleLogger, "Another informational message");
	poco_warning_f2(consoleLogger, "A warning message with arguments: %d, %d", 1, 2);

	Logger::get("ConsoleLogger").error("Another error message");

	Logger::get("logger1").error("Hello World! Error Just Occurred.");
}

typedef void (HelloRunnable::*HelloFuncPtrType)();


void testFuncPointer() {
	HelloFuncPtrType f = &HelloRunnable::speak;

	HelloRunnable *r = new HelloRunnable();

	(r->*f)();
}

void testAssertion() {
	int i=1;
	poco_assert(i >= 1);
	poco_assert_dbg(i >= 0);
	//poco_bugcheck();
	Debugger::enter();
	std::cout << Debugger::isAvailable() << std::endl;
	poco_bugcheck_msg("invalid values");
}


int main(int argc, char** argv)
{
	testAssertion();
	system("pause");

	// Util::config("super-go.config");
	//
	// SuperGoGame* game = new SuperGoGame("super-go.config");
	// game->init();
	// game->setPlayer(BLACK);
	// while( receive a new move k) {m = game->genMove(k); send m; }


	return 0;
}