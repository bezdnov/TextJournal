#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <charconv>
#include <system_error>
#include "libtext.hpp"

std::string importance_to_string(MessageImportance importance) {
	switch (importance) {
		case ORDINARY:
			return "ORDINARY";
		case IMPORTANT:
			return "IMPORTANT";
		case SPECIAL:
			return "SPECIAL";
	}
	return "ORDINARY";
}

Message::Message(std::string message_content, MessageImportance importance) {
	this->importance = importance;
	this->message_content = message_content;
	this->received_at = std::chrono::system_clock::now();
}

Message::Message(std::string message_content, MessageImportance importance, std::chrono::system_clock::time_point timestamp) {
	this->importance = importance;
	this->message_content = message_content;
	this->received_at = timestamp;
}

MessageImportance Message::get_message_importance () {
	return this->importance;
}

std::chrono::system_clock::time_point Message::get_message_time () {
	return this->received_at;
}

std::string Message::get_message_content () {
	return this->message_content;
}

Message::~Message() {
	// std::cout << "Message was destroyed\n";
}

Message::operator std::string() const {
	std::ostringstream oss;	
	
	oss << "Importance level: ";
	switch (this->importance) {
		case ORDINARY:
			oss << "ordinary\n";
			break;
		case IMPORTANT:
			oss << "important\n";
			break;
		case SPECIAL:
			oss << "of special importance\n";
			break;
	}
	oss << "Received at: ";
	std::time_t time = std::chrono::system_clock::to_time_t(this->received_at);
	std::tm* utc = std::gmtime(&time);
	oss << std::put_time(utc, "%Y-%m-%dT%H:%M:%SZ");
	oss << "\nContent: " << this->message_content << '\n';

	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Message& msg) {
    os << static_cast<std::string>(msg);
    return os;
}

// this is un util function needed just to parse time strings like "2026-08-13T08:01:37Z"
std::chrono::system_clock::time_point parse_timestamp(const std::string& str) {
    std::tm tm = {};

    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    if (ss.fail()) {
        throw std::runtime_error("Invalid timestamp: " + str);
    }

    // tm is UTC because the input ends with 'Z'
    std::time_t time = timegm(&tm);

    return std::chrono::system_clock::from_time_t(time);
}

void Journal::loadJournal() {
	std::ifstream file(this->journal_name);

	if (!file.is_open()) {
		std::cerr << "Warning: Couldn't open file " << this->journal_name << " reading aborted\n";
		return;
	}

	std::vector<Message> msgs;

	std::chrono::system_clock::time_point cur_time;
	std::string cur_content;
	MessageImportance cur_importance;

	std::string current_string;
	
	int cur_str_idx = 0;
	while (std::getline(file, current_string)) {
		cur_str_idx++;

		// first string is importance
		cur_importance = (MessageImportance)std::stoi(current_string);

		auto [ptr, ec] = std::from_chars(current_string.data(), current_string.data() + current_string.size(), (int&)cur_importance);

		if (ec != std::errc{}) {
			std::cerr << "Error happened when scanning line " << cur_str_idx << "; further scanning aborted\n";
			return;
		}

		// second string is a timestamp
		if (!std::getline(file, current_string)) {
			std::cerr << "Missing timestamp at line " << cur_str_idx << '\n';
			return;
		}
		cur_str_idx++;
		
		cur_time = parse_timestamp(current_string);
		
		// third string is a message content
		if (!std::getline(file, current_string)) {
			std::cerr << "Missing message content at line" << cur_str_idx << '\n';
			return;
		}
		cur_str_idx++;
		cur_content = current_string;
		
		// skip '\n'
		if (!std::getline(file, current_string)) {
			std::cerr << "Missing empty line at line " << cur_str_idx << '\n';
			return;
		}
		cur_str_idx++;

		Message new_message(cur_content, cur_importance, cur_time);

		// old message are restored independently from basic importance
		this->messages.push_back(new_message);	
	}
}

void Journal::saveJournal() {
	std::ofstream file(this->journal_name);

	if (!file.is_open()) {
		std::cerr << "Couldn't open file " << this->journal_name << " saving aborted\n";
		return;
	}

	for (auto message: this->messages) {
		file << (int)message.get_message_importance() << '\n';

		std::time_t time = std::chrono::system_clock::to_time_t(message.get_message_time());
		std::tm* utc = std::gmtime(&time);

		if (utc == nullptr) {
			std::cerr << "Couldn't convert message time\n";
			return;
		}
		
		std::ostringstream oss;
		
		oss << std::put_time(utc, "%Y-%m-%dT%H:%M:%SZ");

		file << oss.str() << '\n';
		file << message.get_message_content() << "\n\n";

		if (!file) {
			std::cerr << "Error while saving journal\n";
			return;
		}
	}	
}

void Journal::changeImportance(MessageImportance new_importance) {
	this->basic_importance = new_importance;
}

void Journal::addMessage(Message message) {
	std::lock_guard<std::mutex> lock(message_mutex);
	if (message.get_message_importance() >= this->basic_importance)
		this->messages.push_back(message);
}

std::size_t Journal::size() {
	return this->messages.size();
}

Message& Journal::operator[](std::size_t index) {
	return this->messages[index];
}

Journal::Journal(std::string journal_name, MessageImportance basic_importance) {
	this->basic_importance = basic_importance;
	this->journal_name = journal_name;
	loadJournal();
}


Journal::~Journal() {
	// std::cout << "Journal was destroyed\n";
}
