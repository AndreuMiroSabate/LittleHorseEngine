#pragma once

enum class LogType
{
	INFO,
	WARNING,
	Error
};

struct LogEntry
{
	std::string message;
	LogType type;
};

class Console
{
public:
	
	Console() = default;

	void Log(const std::string& msg, LogType type = LogType::INFO);
	void Draw(bool* open);

private:
	std::vector<LogEntry> entries;
	bool autoScroll = true;

};

