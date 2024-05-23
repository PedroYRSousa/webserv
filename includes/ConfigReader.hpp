#include "lib.hpp"

class ConfigReader
{
public:
	ConfigReader(std::string file)
	{
		// _conf_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		_conf_file.open(file.c_str());
		if (!_conf_file.is_open())
			throw std::ios_base::failure::exception();
		_lineNumber = 0;
		_lineVector.push_back("");
	}
	// ConfigReader(const ConfigReader& src);
	// ConfigReader& operator=(const ConfigReader&rhs);
	~ConfigReader(){};

	std::istream &readLine()
	{
		_currentLine.clear();
		std::getline(_conf_file, _currentLine);
		_lineNumber++;
		splitLine();
		verifyTabLevel();
		_numberOfWords = _lineVector.size();
		return (_conf_file);
	};
	int getLineLevel() { return (_lineTabLevel); };
	int getLineNumber() { return (_lineNumber); };
	int getNumberOfWords() { return (_numberOfWords); };
	bool hasLine() { return (_lineVector.size()); };
	std::ifstream &getIfstream() { return (_conf_file); };
	std::string getLineString() { return (_currentLine); };
	std::vector<std::string> getLineVector() { return (_lineVector); };

private:
	void verifyTabLevel()
	{
		_lineTabLevel = 0;
		while (_lineVector[0][0] == '\t')
		{
			_lineTabLevel++;
			_lineVector[0].erase(0, 1);
		}
	};
	void splitLine()
	{
		std::stringstream ss(_currentLine);
		std::string line;

		_lineVector.clear();
		while (std::getline(ss, line, ' '))
		{
			if (line == "")
				continue;
			_lineVector.push_back(line);
		}
	};
	std::ifstream _conf_file;
	std::string _currentLine;
	int _lineNumber;
	std::vector<std::string> _lineVector;
	int _numberOfWords;
	int _lineTabLevel;
};
