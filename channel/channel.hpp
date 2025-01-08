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

	// ----- Operations -----
	void kickClient(std::vector<std::string> &rest, int sFd);
	void changeTopic(std::vector<std::string> &rest, int sFd);
	void changeMode(std::vector<std::string> &rest, int sFd);
	//	Modes:
	void setInvOnly();
	void setTopicRst(int sFd);
	void setKey(int sFd, std::string &key);
	void setOp(std::string &name);
	void setLimit(int sFd, size_t lim);

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
	size_t nUsers();
	std::string capacity();

	// ----- Client Info -----
	void inviteClient(Client *client);
	void addClient(Client *client);
	void removeClient(int fd);
	bool hasClient(int fd) const;
	int hasClient(const std::string& name) const;

	// ----- Operator/Owner Info -----
	bool isOperator(int fd) const;
	bool isOwner(int fd) const;

	// ----- Message -----
	void broadcastMsg(const std::string &msg, int sFd);
	static void sendMsg(int fd, const std::string &msg);

	//  ----- Parsing -----
	bool parseMessage(const std::string &msg, int sFd);

	// ----- Exceptions -----
	class WrongArgException : std::exception {
		virtual const char* what() const throw();
	};
};

#endif //CHANNEL_HPP
