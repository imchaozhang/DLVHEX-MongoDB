#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "ConfigFile.h"

using namespace std;


	void ConfigFile::removeComment(string &line) const
	{
		if (line.find(';') != line.npos)
			line.erase(line.find(';'));
	}

	bool ConfigFile::onlyWhitespace(const string &line) const
	{
		return (line.find_first_not_of(' ') == line.npos);
	}
	bool ConfigFile::validLine(const string &line) const
	{
		string temp = line;
		temp.erase(0, temp.find_first_not_of("\t "));
		if (temp[0] == '=')
			return false;

		for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
			if (temp[i] != ' ')
				return true;

		return false;
	}

	void ConfigFile::extractKey(string &key, size_t const &sepPos, const string &line) const
	{
		key = line.substr(0, sepPos);
		if (key.find('\t') != line.npos || key.find(' ') != line.npos)
			key.erase(key.find_first_of("\t "));
	}
	void ConfigFile::extractValue(string &value, size_t const &sepPos, const string &line) const
	{
		value = line.substr(sepPos + 1);
		value.erase(0, value.find_first_not_of("\t "));
		value.erase(value.find_last_not_of("\t ") + 1);
	}

	void ConfigFile::extractContents(const string &line) 
	{
		string temp = line;
		temp.erase(0, temp.find_first_not_of("\t "));
		size_t sepPos = temp.find('=');

		string key, value;
		ConfigFile::extractKey(key, sepPos, temp);
		ConfigFile::extractValue(value, sepPos, temp);

		if (!ConfigFile::keyExists(key))
			ConfigFile::contents.insert(pair<string, string>(key, value));
		else
			cout<< "CFG: Can only have unique key names!" << endl;
	}

	void ConfigFile::parseLine(const string &line, size_t const lineNo)
	{
		if (line.find('=') == line.npos)
			cout << "CFG: Couldn't find separator on line: " << lineNo << endl;

		if (!validLine(line))
			cout <<"CFG: Bad format for line: "  << lineNo << endl;

		ConfigFile::extractContents(line);
	}

	void ConfigFile::ExtractKeys()
	{
		 ifstream file;
		file.open(ConfigFile::fName.c_str());
		if (!file)
			cout << "CFG: File " + ConfigFile::fName + " couldn't be found!" << endl;

		 string line;
		size_t lineNo = 0;
		while ( getline(file, line))
		{
			lineNo++;
			 string temp = line;

			if (temp.empty())
				continue;

			ConfigFile::removeComment(temp);
			if (onlyWhitespace(temp))
				continue;

			ConfigFile::parseLine(temp, lineNo);
		}

		file.close();
	}

	ConfigFile::ConfigFile(const  string &fName)
	{
		this->fName = fName;
		ConfigFile::ExtractKeys();
	}

	bool ConfigFile::keyExists(const  string &key) const
	{
		return ConfigFile::contents.find(key) != ConfigFile::contents.end();
	}

	string ConfigFile::getValueOfKey(const  string &key) const
	{
		if (!ConfigFile::keyExists(key))
			return "NULL";

		return (ConfigFile::contents.find(key)->second);
	}
