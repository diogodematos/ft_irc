#include "channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) {
	_nameChannel = name;
	_topicChannel = "";
	_ownerCha = -1;
	_invOnly = false;
	_topicRestr = false;
	_keyCha.clear();
	_usrLimit = 100;
}

Channel::~Channel() {}

// --------- GETTERS ---------

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
	return _operatorsCha;
}

bool Channel::isInvOnly() {
	return _invOnly;
}

bool Channel::isTopicRestr() {
	return _topicRestr;
}

bool Channel::isKeyProtected() {
	return !(_keyCha.empty());
}


bool Channel::compareKey(std::string &key) {
	return (key == _keyCha);
}

size_t Channel::nUsers() {
	return _clientsCha.size();
}

// --------- TOOLS ---------

std::string Channel::capacity() {
	std::stringstream ss;
	ss << "(" << nUsers() << "/" << _usrLimit << ")";
	return ss.str();
}

// --------- ADMIN MANAGEMENT ---------

bool Channel::isOperator(int fd) const {
	return std::find(_operatorsCha.begin(), _operatorsCha.end(), fd) != _operatorsCha.end();
}

bool Channel::isOwner(int fd) const {
	return (_ownerCha == fd);
}

void Channel::addRemOperator(int fd) {
	if (std::find(_operatorsCha.begin(), _operatorsCha.end(), fd) == _operatorsCha.end())
		_operatorsCha.push_back(fd);
}

void Channel::addClient(Client *client) {
	if (client && canAddUsr())
	{
		if (_clientsCha.empty() && _operatorsCha.empty()) // First client to connect is automatically the owner
		{
			_ownerCha = client->getFd();
			std::cout << "Client " << client->getFd() << " added to channel as founder." << std::endl;
		}
		else
			std::cout << "Client " << client->getFd() << " added to channel." << std::endl;
		broadcastMsg(client->getNickname() + " joined your channel. " + capacity() + "\r\n", -1);
		_clientsCha[client->getFd()] = client; // Store the pointer
	}
	else
		std::cerr << "Error: Attempted to add a null client to the channel." << std::endl;
}

void Channel::removeClient(int fd) {
/*	if (isOperator(fd))
		_operatorsCha.;*/
	_clientsCha.erase(fd);
}

bool Channel::hasClient(int fd) const {
	return _clientsCha.find(fd) != _clientsCha.end();
}

int Channel::hasClient(const std::string& name) const {
	std::cout << "Checking if client exists: " << name << std::endl;
	std::map<int, Client*>::const_iterator it = _clientsCha.begin();
	while (it != _clientsCha.end())
		if (it->second->getNickname() == name)
			return it->first;
	return -1;
}

// --------- CHANNEL MANAGEMENT ---------

void Channel::changeTopic(std::vector<std::string> &rest, int sFd) {
	std::string tmp = "Channel topic was changed successfully.\r\n";
	if (isOwner(sFd))
		_topicChannel = rest[4];
	else if (isOperator(sFd))
	{
		if (isTopicRestr())
			tmp = "Channel topic cannot be changed.\r\n";
	}
	else
		tmp = "You don't have the privileges required to execute that operation.\r\n";
	send(sFd, &tmp, tmp.size(), 0);
}

void Channel::changeMode(std::vector<std::string> &rest, int sFd) {
	(void)rest;
	std::string tmp = "Channel topic was changed successfully.\r\n";;
	if (isOwner(sFd))
	{
		char mode = '\0';
		switch (mode) {
			case 'i':
				break;
			case 't':
				break;
			case 'k':
				break;
			case 'o':
				break;
			case 'l':
				break;
			default:
				tmp = "Channel mode not found.\r\n";
		}
	}
	else
		tmp = "You don't have the privileges required to execute that operation.\r\n";
	send(sFd, &tmp, tmp.size(), 0);
}

// --------- CLIENT MANAGEMENT ---------

bool Channel::canAddUsr() {
	return (nUsers() < _usrLimit);
}

/*void Channel::removeOperator(int fd) {
	_operatorsCha.erase(std::remove(_operatorsCha.begin(), _operatorsCha.end(), fd), _operatorsCha.end());
}*/

/*void Channel::inviteClient(Client *client) {
	addClient(client);
}*/

// --------- MESSAGE ---------

void Channel::broadcastMsg(const std::string &msg, int sFd) { // Use sFd=-1 if making a broadcast from the server
	std::string id;
	if (sFd == -1)
		id = "Server: " + msg;
	else
		id = _clientsCha.find(sFd)->second->getNickname() + ": " + msg;

	for (std::map<int, Client*>::iterator it = _clientsCha.begin(); it != _clientsCha.end(); ++it) {
		int fd = it->first; //gets the file descriptor
		//if (fd != sender_fd) //does not send the msg to the sender --- why?
		sendMsg(fd, id);
	}
}

void Channel::sendMsg(int fd, const std::string &msg) {
	send(fd, msg.c_str(), msg.size(), 0);
}

// --------- PARSING ---------

/*static std::string extract(std::string rest, std::string nm) {
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
}*/

bool Channel::parseMessage(const std::string &msg, int sFd) {
	std::vector<std::string> args;
	std::stringstream ss(msg);
	std::string token;

	bool foundCmd = false;
	int i;

	while (ss >> token)
		args.push_back(token);

	std::string ops[4] = { "KICK", "INVITE", "TOPIC", "MODE" };
	for (i = 0; i < 4; i++) {
		if (args[0] == ops[i]){
			foundCmd = true;
			break;
		}
	}
	if (!foundCmd)
		return foundCmd;

	switch (i) { // manda o resto da msg para ser tratada
		case 0:
			Channel::kickClient(args, sFd);
			break;
		case 1:
			send(sFd, "Trying to invt\n", 15, 0);
			//Channel::inviteClient(rest);
			break;
		case 2:
			send(sFd, "Trying to topc\n", 15, 0);
			//Channel::changeTopic(rest);
			break;
		case 3:
			send(sFd, "Trying to mode\n", 15, 0);
			//Channel::changeMode(rest);
			break;
		default:
			return false;
	}
	return true;
}

/*bool Channel::parseMessage(const std::string &msg, int sender_fd) {
	// Procura na msg as várias keywords
	std::string ops[4] = { "KICK", "INVITE", "TOPIC", "MODE" };
	std::string rest;
	int i = 0;

	for (i = 0; i < 4; i++){
		if (int idx = msg.find(ops[i]) != std::string ::npos)
		{
			rest = extract(msg.substr(idx + ops[i].length()), this->_nameChannel); // copia tudo o que esta a frente do comando e channel
			break;
		}
	}

	if (rest.empty()) // Se nao encontrou comandos e argumentos na msg, retorna false
		return false;

	switch (i) { // manda o resto da msg para ser tratado e extraído o valor em cada funcao
		case 0:
			send(sender_fd, "Trying to kick\n", 15, 0);
			Channel::kickClient(rest, sender_fd);
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
}*/

 // --------- OPERATIONS ---------

void Channel::kickClient(std::vector<std::string> &rest, int sFd) {
	// Server log
	std::string kicker = _clientsCha.find(sFd)->second->getNickname();
	std::cout << "Client " << kicker << " kicking " << rest[2].c_str() << "\r\n";

	if (isOwner(sFd) || isOperator(sFd))
	{
		int kFd = hasClient(rest[2].c_str());
		sendMsg(sFd, "Client not in channel\r\n");
		if (kFd == sFd)
			sendMsg(sFd, "Error: you cannot kick yourself.\r\n");
		else if (kFd != -1)
		{
			broadcastMsg(rest[2] + " has been kicked by " + kicker + ".\r\n", -1);
			_clientsCha.erase(kFd);
			//sendMsg(kFd, "You have been kicked from the channel");
		}
		else
			sendMsg(sFd, "Client not in channel\r\n");
	}
	else
		sendMsg(sFd, "Error: you must be an operator to kick someone.");
}

 // --------- EXCEPTIONS ---------
 const char *Channel::WrongArgException::what() const throw() {
 	return "Wrong type of argument for command.";
 }
