#include "configparser.h"
#include "debug.h"
#include <ctype.h>

#define MAX_LINE_LEN	1024


/*
 * Trim whitespaces from beginning and end of a string
 */
static char *trim(char *str)
{
	while (isspace(*str))
		++str;

	char *p = str + strlen(str);
	while (p > str && isspace(*--p));

	*++p = '\0';

	return str;
}

void ConfigParser::sectionParsed()
{
	if (listener && *profile.getName())
		listener->sectionParsed(&profile);

	// Prepare for the next section
	profile.reset();
}

/*
 * Parse [section]
 */
bool ConfigParser::parseSection(const char *line)
{
	char name[MAX_LINE_LEN];

	char *p = name;
	while (*++line != ']' && *line)
		*p++ = *line;

	if (!*line)
		return false;

	*p = '\0';
	profile.setName(trim(name));
	return true;
}

/*
 * Parse a line
 */
void ConfigParser::parseLine(const char *line)
{
	// A new section begins
	if (*line == '[') {
		sectionParsed();
		parseSection(line);
		return;
	}

	char name[MAX_LINE_LEN];
	char value[MAX_LINE_LEN];

	// Get the variable name
	char *p = name;
	while (*line && *line != '=')
		*p++ = tolower(*line++);	// name is case insensitive
	*p = '\0';

	// Get the variable value
	p = value;
	while (*line && *++line)
		*p++ = *line;
	*p = '\0';

	const char *key = trim(name);
	const char *val = trim(value);

	// Put the (key, value) into the map only when they are both invalid
	if (*key && *val)
		profile.set(key, val);
}

bool ConfigParser::start(ConfigListener *l)
{
	FILE *file = fopen(fileName.c_str(), "rt");
	if (!file) {
		ICQ_LOG("Can not open file: %s\n", fileName.c_str());
		return false;
	}

	listener = l;

	char line[MAX_LINE_LEN];

	// Parse it line by line
	while (fgets(line, sizeof(line), file)) {
		// Comments are started by #
		if (!*line || *line == '#')
			continue;

		parseLine(line);
	}

	// Notify listener that the last section has parsed
	sectionParsed();

	listener = NULL;

	fclose(file);
	return true;
}

Profile *ConfigParser::getProfile(const char *section)
{
	profile.reset();

	FILE *file = fopen(fileName.c_str(), "rt");
	if (!file) {
		ICQ_LOG("Can not open file: %s\n", fileName.c_str());
		return &profile;
	}

	char line[MAX_LINE_LEN];
	bool inSection = false;

	while (fgets(line, sizeof(line), file)) {
		if (!inSection && *line == '[') {
			parseSection(line);

			// Whether we found it
			if (strcmp(profile.getName(), section) == 0)
				inSection = true;

		} else if (inSection) {
			// If the next section begins, break out of the loop
			if (*line == '[')
				break;
			parseLine(line);
		}
	}

	fclose(file);
	return &profile;
}
