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
#include "UCTSearchRunner.h"
#include "util/Counter.h"

using Poco::Logger;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::ConsoleChannel;
using Poco::FileChannel;
using Poco::Message;
using Poco::Mutex;


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
}

void testLog() {
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
}

typedef void (HelloRunnable::*HelloFuncPtrType)();


void testFuncPointer() {
	HelloFuncPtrType f = &HelloRunnable::speak;

	HelloRunnable *r = new HelloRunnable();

	(r->*f)();
}



int main(int argc, char** argv)
{
	testThread();
	system("pause");
	return 0;
}