// A example of C++ dependency injection

#ifndef __DEPENDENCY_ENTRY_H__
#define __DEPENDENCY_ENTRY_H__

#include <map>

typedef void(*CbPtr)(void*);
typedef void(*CbVoid)();

using std::map;
using std::string;

/**

[Example] How to use DependencyManager

#include "DependencyEntry.h"

DependencyManager* DependencyManager::instance = nullptr;

void DependencyManager::RegisterDependency(const char* nameOfDependency, CbVoid cbStartup)
{
	if (DependencyManager::instance == nullptr) {
		DependencyManager::instance = new DependencyManager();
	}
	auto ins = DependencyManager::instance;
	ins->nameToStartup[nameOfDependency] = cbStartup;
}

class App {

protected:
	class GlConsoleManager : public DependencyManager {
	public:
		static void RunAllStartup() {
			BringUp();
		}
	};

public:
	void Run() {
		std::cout << "run some init() code" << std::endl;
		GlConsoleManager::RunAllStartup();
		std::cout << "run other init() code" << std::endl;
	}
};


[Example] How to register a DependencyEntry

#include "DependencyEntry.h"

void cbStartup01() {
	std::cout << "cbStartup01" << std::endl;
}
DependencyEntry s1("s1", cbStartup01);

void cbStartup02() {
	std::cout << "cbStartup02" << std::endl;
}
DependencyEntry s2("s2", cbStartup02);

*/
class DependencyManager {
public:
	static void RegisterDependency(const char* nameOfDependency, CbVoid cbStartup);

private:
	DependencyManager() {}

protected:
	// unordered bring up
	static void BringUp() {
		auto ins = DependencyManager::instance;
		auto& map = ins->nameToStartup;
		for (auto it = map.begin(); it != map.end(); it++) {
			auto item = (*it);
			auto cbStartup = item.second;
			cbStartup();
		}
	}

	// ordered bring up from config
	static void BringUpOne(const char* name) {
		string strName(name);
		auto& map = instance->nameToStartup;
		auto it = map.find(strName);
		if (it != map.end()) {
			auto item = (*it);
			auto cbStartup = item.second;
			cbStartup();
		}
	}

protected:
	static DependencyManager* instance;
	map<string, CbVoid> nameToStartup;
};


class DependencyEntry
{
public:
	DependencyEntry(const char* nameOfDependency, CbVoid cbStartup) {
		DependencyManager::RegisterDependency(nameOfDependency, cbStartup);
	}
};

#endif
