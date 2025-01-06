#include "channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) {
	_nameChannel = name;
	_topicChannel = "";
}

Channel::~Channel() {}

const std::string &Channel::getNameChannel() const {
	return _nameChannel;
}

const std::string &Channel::getTopicChannel() const {
	return _topicChannel;
}

const std::map<int, Client*> &Channel::getClients() const {
	return _clientsCha;
}

const std::vector<int> &Channel::getOperators() const {
	return _operatorsChannel;
}

void Channel::setTopic(const std::string &topic) {
	_topicChannel = topic;
}

void Channel::addClient(Client *client) {
	if (client) {
		_clientsCha[client->getFd()] = client; // Store the pointer
		std::cout << "Client " << client->getFd() << " added to channel." << std::endl;
	} else {
		std::cerr << "Error: Attempted to add a null client to the channel." << std::endl;
	}
}

void Channel::removeClient(int fd) {
	_clientsCha.erase(fd);
}

bool Channel::hasClient(int fd) const {
	return _clientsCha.find(fd) != _clientsCha.end();
}

void Channel::addOperator(int fd) {
	if (std::find(_operatorsChannel.begin(), _operatorsChannel.end(), fd) == _operatorsChannel.end())
		_operatorsChannel.push_back(fd);
}

void Channel::removeOperator(int fd) {
	_operatorsChannel.erase(std::remove(_operatorsChannel.begin(), _operatorsChannel.end(), fd), _operatorsChannel.end());
}

bool Channel::isOperator(int fd) const {
	return std::find(_operatorsChannel.begin(), _operatorsChannel.end(), fd) != _operatorsChannel.end();
}

/*void Channel::inviteClient(Client *client) {
	addClient(client);
}*/



void Channel::broadcastMsg(const std::string &msg, int sender_fd) {
	for (std::map<int, Client*>::iterator it = _clientsCha.begin(); it != _clientsCha.end(); ++it) {
		int fd = it->first; //gets the file descriptor
		if (fd != sender_fd) { //does not send the msg to the sender
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
}

// --------- PARSING ---------

static std::string extract(std::string rest, std::string nm) {
	std::string extracted;
	rest = rest.substr(rest.find(nm) + nm.length());
	size_t idx = rest.find_first_not_of(" \t\v\n\r\f"); // skip a todos os whitespaces da std::isspace
	if (idx != std::string::npos)
	{
		rest = rest.substr(idx); // faz com que a str seja tudo a seguir aos whitespaces

		idx = rest.find_first_of(" \t\v\n\r\f"); // vai tentar ver se o valor esta entre espaços
		if (idx != std::string::npos)
			extracted = rest.substr(0, idx);
		else
			extracted = rest; // se nao esta entre espaços, vai o resto da str
	}
	return extracted; // se for empty(), e verificado a seguir
}

bool Channel::parseMessage(const std::string &msg, int sender_fd) {
	// Procura na msg as várias keywords
	std::string ops[4] = { "KICK", "INVITE", "TOPIC", "MODE" };
	std::string rest;
	int i = 0;

	for (i = 0; i < 4; i++){
		if (int idx = msg.find(ops[i]) != std::string ::npos)
		{
			rest = extract(msg.substr(idx + ops[i].length()), this->_nameChannel); // copia tudo o que esta a frente do comando
			break;
		}
	}

	if (rest.empty()) // Se nao encontrou comandos e argumentos na msg, retorna false
		return false;

 	switch (i) { // manda o resto da msg para ser tratado e extraído o valor em cada funcao
		case 0:
			send(sender_fd, "Trying to kick\n", 15, 0);
			//Channel::kickClient(rest);
			break;
		case 1:
			send(sender_fd, "Trying to invt\n", 15, 0);
 			//Channel::inviteClient(rest);
 			break;
 		case 2:
			send(sender_fd, "Trying to topc\n", 15, 0);
 			//Channel::changeTopic(rest);
 			break;
 		case 3:
			send(sender_fd, "Trying to mode\n", 15, 0);
 			//Channel::changeMode(rest);
 			break;
 		default:
 			return false;
 	}
 	return true;
 }

 // --------- OPERATIONS ---------

 void Channel::kickClient(std::string &rest) {
 	rest = "kick";
 }

 // --------- EXCEPTIONS ---------
 const char *Channel::WrongArgException::what() const throw() {
 	return "Wrong type of argument for command.";
 }
