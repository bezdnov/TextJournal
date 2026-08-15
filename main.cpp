#include "libtext.hpp"
#include <charconv>
#include <iostream>
#include <iomanip>
#include <regex>
#include <thread>
// #include <stdlib>


void menu() {
	puts("Journal app");
	puts("m [message_importance:STRING, optional] [message_content:STRING] - add message");
	puts("l [index:INTEGER, optional] - list existing message");
	puts("q - exit program");
	puts("h - display this help");
	puts("j [message_importance:int] - set journal basic importance level of a journal [0:2]");
	puts("s [message_importance:int] - set journal basic importance level of your messages [0:2] (default: 0, ORDINARY)");
	puts("t - print status of a program: amount of messages in journal, basic importance levels");
}

void add_message(Journal& journal, Message message) {
	journal.addMessage(message);
}

// there must be a connection to library on initial stage, and then user gets right to set messages
int main(int argc, char* argv[]) {
	if (argc != 3) {
		puts("Usage: ./program [JOURNAL_NAME:str] [BASIC IMPORTANCE:int from 0 to 2]");
		std::exit(1);
	}
	
	std::string journal_name = argv[1];
	MessageImportance basic_importance = (MessageImportance)std::stoi(argv[2]);

	Journal journal(journal_name, basic_importance);

	MessageImportance message_basic_importance = ORDINARY;
	
	menu();
	char c;
	while (true) {
		std::cout << ">> ";
		char c;
		std::cin >> c;
		switch (c) {
			case 'm': {
				std::string line, token;
				std::getline(std::cin, line);

				std::istringstream iss(line);
				std::string first_word;
				iss >> first_word;

				std::string content;
				MessageImportance importance = message_basic_importance;

				if (first_word == "ORDINARY") {
					importance = ORDINARY;
					iss.ignore(1);
					getline(iss, content);
				} else if (first_word == "IMPORTANT") {
					importance = IMPORTANT;
					iss.ignore(1);
					getline(iss, content);
				} else if (first_word == "SPECIAL") {
					importance = SPECIAL;
					iss.ignore(1);
					getline(iss, content);
				} else {
					// in case importance not specified, the rest of message is considered as a whole message
					content = line;
				}

				Message new_message(content, importance);
				
				std::thread add_message_thread(add_message, std::ref(journal), new_message);
				add_message_thread.join();

				break;
			}
			case 'l': {
				std::size_t index;
				std::string line;

				std::string command_info;
				std::getline(std::cin, line);
				std::istringstream iss(line);

				if (iss >> index) {
					if (journal.size() == 0) {
						std::cout << "Journal is empty\n";
					}
					else if (index >= journal.size())
						std::cout << "Index out of range (min: 0, max: " << journal.size() - 1 << ")\n";
					else
						std::cout << journal[index] << std::endl;
				} else {
					for (std::size_t i = 0; i < journal.size(); ++i) {
						std::cout << journal[i] << std::endl;
					}
				}
				break;
			}
			case 'j': {
				int int_importance;
				// MessageImportance importance;
				std::cin >> int_importance;
				if (std::cin.fail() || int_importance < 0 || int_importance > 2) {
					puts("Incorrect input");
				} else {
					basic_importance = static_cast<MessageImportance>(int_importance);
					journal.changeImportance(basic_importance);
				}
				break;
			}
			case 's': {
				int int_importance;
				std::cin >> int_importance;
				if (std::cin.fail() || int_importance < 0 || int_importance > 2) {
					puts("Incorrect input");
				} else {
					message_basic_importance = static_cast<MessageImportance>(int_importance);
				}
				break;
			}
			case 'h': {
				menu();
				break;
			}
			case 'q': {
				journal.saveJournal();
				return 0;
			}
			case 't': {
				std::cout << "Journal status\n";
				std::cout << "Journal basic importance: " << importance_to_string(basic_importance) << '\n';
				std::cout << "Message basic importance: " << importance_to_string(message_basic_importance) << '\n';
				std::cout << "Journal size: " << journal.size() << '\n';
				break;
			}
			
			default:
				break;
		}
	}

	return 0;
}

