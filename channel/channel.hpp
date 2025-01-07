#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Server.hpp"
class Client;

class Channel {
private:
	std::string _nameChannel;
	std::string _topicChannel;
	std::map<int, Client*> _clientsCha; // mapa dos fd's dos clientes
	std::vector<int> _operatorsCha; // lista dos fd's dos operadores
	int _ownerCha;
	bool _invOnly;
	bool _topicRestr;
	std::string _keyCha;
	size_t _usrLimit;

public:
	Channel();
	Channel(const std::string &name);
	~Channel();

	// ----- Getters -----
	const std::string &getNameChannel() const;
	const std::string &getTopicChannel() const;
	const std::map<int, Client*> &getClients() const;
	const std::vector<int> &getOperators() const;
	bool isInvOnly();
	bool isTopicRestr();
	bool isKeyProtected();
	bool compareKey(std::string &key);
	bool canAddUsr();

	// ----- Client management -----
	void addClient(Client *client);
	void removeClient(int fd);
	bool hasClient(int fd) const;

	// ----- Operator/Owner management -----
	void addRemOperator(int fd);
	//void removeOperator(int fd);
	bool isOperator(int fd) const;
	bool isOwner(int fd) const;

	// ----- Message -----
	void broadcastMsg(const std::string &msg, int sender_fd);

	//  ----- Parsing -----
	bool parseMessage(const std::string &msg, int sender_fd);

	// ----- Operations -----
	void kickClient(std::string &rest, int sFd);
	void inviteClient(std::string &rest, int sFd);
	void changeTopic(std::string &rest, int sFd);
	void changeMode(std::string &rest, int sFd);


	// ----- Exceptions -----
	class WrongArgException : std::exception {
		virtual const char* what() const throw();
	};
};

#endif //CHANNEL_HPP
