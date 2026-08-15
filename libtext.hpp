#ifndef _TEXTLIB_
#define _TEXTLIB_

#include <vector>
#include <chrono>
#include <mutex>
#include <string>

enum MessageImportance {
	SPECIAL = 2, IMPORTANT = 1, ORDINARY = 0
};


std::string importance_to_string(MessageImportance imoprtance);

class Message {
private:
	MessageImportance importance;
	std::chrono::system_clock::time_point received_at;
	std::string message_content;
public:
	std::chrono::system_clock::time_point get_message_time();
	MessageImportance get_message_importance();
	std::string get_message_content();

	operator std::string() const;
	friend std::ostream& operator <<(std::ostream& os, const Message& msg);

	Message(std::string message_content, MessageImportance importance);
	Message(std::string message_content, MessageImportance importance, std::chrono::system_clock::time_point timestamp);
	~Message();
};

class Journal {
private:
	MessageImportance basic_importance;
	std::string journal_name;
	std::vector<Message> messages;
	std::mutex message_mutex;

public:
	Journal(std::string journal_name, MessageImportance basic_importance);
	~Journal();

	void loadJournal();
	void changeImportance(MessageImportance new_importance_level);

	void addMessage(Message message);
	std::size_t size();

	Message& operator[](std::size_t index);
	void saveJournal();
};

#endif
