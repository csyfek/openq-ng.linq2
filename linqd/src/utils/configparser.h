#ifndef _CONFIG_PARSER_H
#define _CONFIG_PARSER_H

#include "profileimpl.h"


class ConfigListener {
public:
	// Called when a section is parsed
	virtual void sectionParsed(Profile *profile) = 0;
};


/*
 * Configuration file parser (like windows INI format)
 */
class ConfigParser {
public:
	ConfigParser(const char *name) { fileName = name; }

	bool start(ConfigListener *l);

	// Get a profile object by section name.
	// The return value must not be NULL
	Profile *getProfile(const char *section);

private:
	void parseLine(const char *line);
	bool parseSection(const char *line);
	void sectionParsed();

	string fileName;
	ProfileImpl profile;
	ConfigListener *listener;
};


#endif
