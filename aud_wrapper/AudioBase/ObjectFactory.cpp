#include "ObjectFactory.h"

std::map<std::string, NewInstancePt> ObjectFactory::dynCreateMap;

void* ObjectFactory::CreateObject(const char * className) {
	
	std::map<std::string, NewInstancePt>::const_iterator it;
	it = dynCreateMap.find(className);
	if(it == dynCreateMap.end()) {
		return NULL;
	}		
	else {
		NewInstancePt np = it->second;
		return np();
	}

}

void ObjectFactory::RegisterClass(const char * className, NewInstancePt np) {

	dynCreateMap[className] = np;
}

Register::Register(const char * className, NewInstancePt np) {

	ObjectFactory::RegisterClass(className, np);

}

