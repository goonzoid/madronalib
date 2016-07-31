
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#pragma once

// TODO this goes away to be replaced by generic message listener.

//#include <ostream>

#include <sstream>

class MLTextStreamListener
{
public:
	MLTextStreamListener() {};
	virtual ~MLTextStreamListener() {};	

	virtual void display() = 0;

	std::stringstream& getStream() { return mStream; }
	
protected:	
	std::stringstream mStream;
};

