#include "channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string &name) {
	_nameChannel = name;
	_topicChannel = "";
	_ownerCha = -1;
	_activUsr = 0;
	_invOnly = false;
	_topicRestr = false;
	success = false;
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

size_t Channel::activeUsers() const {
	//return _activUsr;
	return _clientsCha.size();
}

// --------- ADMIN MANAGEMENT ---------

bool Channel::isOperator(int fd) const {
	return std::find(_operatorsCha.begin(), _operatorsCha.end(), fd) != _operatorsCha.end();
}

bool Channel::isOwner(int fd) const {
	return (_ownerCha == fd);
}

void Channel::addClient(int sFd, Client *client) {
	if (client && canAddUsr())
	{
		if (_clientsCha.empty())// && _operatorsCha.empty()) // First client to connect is automatically the owner
		{
			_clientsCha[sFd] = client; // Store the pointer
			_ownerCha = sFd;
			_operatorsCha.push_back(sFd);

			std::string	msgJoin = ":" + client->getNickname() + " JOIN " + _nameChannel + "\r\n";
			sendToAllClients(msgJoin);

			std::string msgMode = ":" + client->getNickname()  + " MODE " + _nameChannel + " o\r\n";
			sendToAllClients(msgMode);

			std::string msgNamReply = ":127.0.0.1 353 " + client->getNickname() + " = " + _nameChannel + " :@" + client->getNickname() + "\r\n";
			sendToAllClients(msgNamReply);

			std::string msgEndOfList = ":127.0.0.1 366 " + client->getNickname() + " " + _nameChannel + " :End of /NAMES list.\r\n";
			sendToAllClients(msgEndOfList);

			std::cout << "Client " << client->getFd() << " created channel " + _nameChannel + ".\r\n"; // server log
			broadcastMsg(client->getNickname() + " created the channel. " + capacity() + "\r\n", -1);
		} else
		{
			_clientsCha[sFd] = client; // Store the pointer
			std::string	msgJoin = ":" +client->getNickname() + " JOIN " + _nameChannel + "\r\n";
			sendToAllClients(msgJoin);

			std::string msgNamReply = ":127.0.0.1 353 " + client->getNickname() + " = " + _nameChannel + " :" + getClist() + "\r\n";
			sendToAllClients(msgNamReply);

			std::string msgEndOfList = ":127.0.0.1 366 " + client->getNickname() + " " + _nameChannel + " :End of /NAMES list.\r\n";
			sendToAllClients(msgEndOfList);

			//send a messagem to all the members of the channel saying that someone joined the channel
			std::string msgJoinBroadcast = ":" + client->getNickname() + " JOIN :" + _nameChannel + "\r\n";

			broadcastMsg(client->getNickname() + " joined your channel. " + capacity() + "\r\n", -1);
		}
		std::cout << "Client " << sFd << " added to channel." << std::endl; // server log
	}
	else
		std::cerr << "Error: Attempted to add a null client to the channel." << std::endl; // server log
}

/*void Channel::removeClient(int fd) {
*//*	if (isOperator(fd))
		_operatorsCha.;*//*
	_clientsCha.erase(fd);
}*/

bool Channel::hasClient(int fd) const {
	return _clientsCha.find(fd) != _clientsCha.end();
}

int Channel::hasClient(const std::string& name) const {
	// std::cout << "Checking if client exists: " << name << "\r\n"; // DEBUG
	std::map<int, Client*>::const_iterator it = _clientsCha.begin();
	while (it != _clientsCha.end())
	{
		if (it->second->getNickname() == name)
			return it->first;
		++it;
	}
	return -1;
}

// --------- CHANNEL MANAGEMENT ---------

void Channel::kickClient(std::vector<std::string> &rest, int sFd) {
	std::string kicker = _clientsCha.find(sFd)->second->getNickname();
	if (!hasClient(sFd))
		sendMsg(sFd, "Error: you need to be in the channel to kick someone.\r\n");
	else if (isOwner(sFd) || isOperator(sFd))
	{
		int kFd = hasClient(rest[2]);
		if (kFd == sFd)
			sendMsg(sFd, "Error: you cannot kick yourself.\r\n");
		else if (kFd != -1)
		{
			std::string kick = ":" + kicker + " KICK " + _nameChannel + " " + _clientsCha.find(kFd)->second->getNickname() + "\r\n";
			//broadcastMsg(rest[2] + " has been kicked by " + kicker + ". " + capacity() + "\r\n", -1);
			sendToAllClients(kick);
			_clientsCha.erase(kFd);
			//sendMsg(kFd, "You have been kicked from the channel.\r\n");
			std::cout << kicker << " kicked " << rest[2] << " from " << _nameChannel << ".\r\n";
		}
		else
			sendMsg(sFd, "Error: client not in channel\r\n");
	}
	else
		sendMsg(sFd, "Error: you must be an operator to kick someone.");
}

void Channel::changeTopic(std::vector<std::string> &rest, int sFd) {
	std::string tmp = "Channel topic was changed successfully.\r\n";
	if (isOwner(sFd))
		_topicChannel = rest[2];
	else if (isOperator(sFd))
	{
		if (isTopicRestr())
			tmp = "Error: channel topic cannot be changed.\r\n";
		else
		{
			_topicChannel = rest[2];
			std::string topic = "TOPIC " + _nameChannel + " :" + _topicChannel + "\r\n";
			sendToAllClients(topic);
		}
	}
	else
		tmp = "Error: only operators or owners can change the topic.\r\n";
	sendMsg(sFd, tmp);
}

void Channel::inviteClient(int sFd, Client *client) {
	success = false;
	if (!hasClient(sFd))
		sendMsg(sFd, "You need to be in a channel to invite someone.\r\n");
	else if (hasClient(client->getFd()))
		sendMsg(sFd, "Client already in channel.\r\n");
	else
	{
		if (isOperator(sFd) || isOwner(sFd))
		{
			/*broadcastMsg(_clientsCha.find(sFd)->second->getNickname() + " invited " + client->getNickname() + " to this channel. " + capacity() + "\r\n", -1);
			sendMsg(client->getFd(), _clientsCha.find(sFd)->second->getNickname() + " added you to this channel.\r\n");*/
			//_clientsCha[client->getFd()] = client; // Store the pointer
			addClient(client->getFd(), client);
			success = true;
		}
		else
			sendMsg(sFd, "You don't have permission to invite clients.\r\n");
	}
}

void Channel::inviteClient(std::vector<std::string> &args, int sFd) {
	int aux = hasClient(args[1]);
	if (aux > 0 && success)
	{
		std::string inviter = ":127.0.0.1 341 " + _clientsCha.find(sFd)->second->getNickname() + " " + _clientsCha.find(aux)->second->getNickname() + " " + _nameChannel + "\r\n";
		sendMsg(sFd, inviter);

		std::string	invited = ":" + _clientsCha.find(sFd)->second->getNickname() + " INVITE " + _clientsCha.find(aux)->second->getNickname() + " " + _nameChannel + "\r\n";
		sendMsg(aux, invited);

		std::cout << _clientsCha.find(sFd)->second->getNickname() << " invited " + _clientsCha.find(aux)->second->getNickname() + " to " + _nameChannel + "\r\n"; // Server log
		//broadcastMsg(_clientsCha.find(sFd)->second->getNickname() + " invited " + _clientsCha.find(aux)->second->getNickname() + " to this channel. " + capacity() + "\r\n", -1);
		//sendMsg(_clientsCha.find(aux)->second->getFd(), _clientsCha.find(sFd)->second->getNickname() + " added you to this channel.\r\n");
		success = false;
	}
	else
	{
		std::cout << _clientsCha.find(sFd)->second->getNickname() << "'s invitation to " + _nameChannel + " failed.\r\n"; // Server log
		sendMsg(sFd, "Error: your invitation failed.\r\n");
		sendMsg(aux, _clientsCha.find(sFd)->second->getNickname() + " was unable to invite you.\r\n");
	}
}

void Channel::removeClient(int fd) {
	if (hasClient(fd))
	{
		std::string part = ":" + _clientsCha.find(fd)->second->getNickname() + " PART " + _nameChannel + "\r\n";
		sendToAllClients(part);
		std::stringstream ss;
		ss << _clientsCha.find(fd)->second->getNickname() << " left the channel.";
		_clientsCha.erase(fd);
		ss << capacity() + "\r\n";
		broadcastMsg(ss.str(), -1);
	}else
		sendMsg(fd, "Error: unable to leave channel.\r\n");
}

void Channel::changeMode(std::vector<std::string> &rest, int sFd) {
	if (isOwner(sFd) || isOperator(sFd))
	{
		if (rest.size() < 3 || rest[2].size() != 1)
			return sendMsg(sFd, "Error: mode must be a single character.\r\n");
		char mode = rest[2][0];
		switch (mode) {
			case 'i':
				setInvOnly();
				break;
			case 't':
				setTopicRst(sFd);
				break;
			case 'k':
				if (rest.size() == 4)
					setKey(sFd, rest[3]);
				break;
			case 'o':
				if (rest.size() == 4)
					setOp(sFd, rest[3]);
				break;
			case 'l':
				if (rest.size() == 4)
					setLimit(sFd, rest[3]);
				break;
			default:
				return sendMsg(sFd, "Error: channel mode not found.\r\n");
		}
	}
	else
		sendMsg(sFd, "Error: you don't have the privileges required to execute that operation.\r\n");
}
// ------ Modes ------
void Channel::setInvOnly() {
	if (!_invOnly)
	{
		_invOnly = true;
		broadcastMsg("This channel was set to invite-only.\r\n\"", -1);
	}
	else
	{
		_invOnly = false;
		broadcastMsg("Users can now join this channel.\r\n\"", -1);
	}
}

void Channel::setTopicRst(int sFd) {
	if (isOwner(sFd))
	{
		if (!_topicRestr)
		{
			_topicRestr = true;
			broadcastMsg("This channel's topic was locked by the owner.\r\n", -1);
			std::string topicLock = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " +t\r\n";
			sendToAllClients(topicLock);
		}
		else
		{
			_topicRestr = false;
			broadcastMsg("This channel's topic is now unlocked.\r\n", -1);
			std::string topicUnlock = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " -t\r\n";
			sendToAllClients(topicUnlock);
		}
	} else
		sendMsg(sFd, "Error: only the owner can lock/unlock channel topics.\r\n");

}

void Channel::setKey(int sFd, std::string &key) {
	if (_keyCha.empty())
	{
		_keyCha = key;
		std::string keyOn = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " +k " + key + "\r\n";
		sendToAllClients(keyOn);
		broadcastMsg("This channel is now key-protected.\r\n", -1);
	}
	else if (!_keyCha.empty() && _keyCha == key)
	{
		_keyCha.clear();
		broadcastMsg("The channel's key was removed.\r\n", -1);
		std::string keyOff = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " -k\r\n";
		sendToAllClients(keyOff);
	} else if (!compareKey(key))
	{
		sendMsg(sFd, "Error: the given key does not match.\r\n");
		std::string wrongKey = ":127.0.0.1 525 " + _clientsCha.find(sFd)->second->getNickname() + " :Key does not match!\r\n";
		sendMsg(sFd, wrongKey);
	}
	else
		sendMsg(sFd, "Error: unknown. Please try again\r\n");
}

void Channel::setOp(int sFd, std::string &name) {
	int fd = hasClient(name);

	if (fd < 0)
		sendMsg(sFd, "Error: client not in channel.\r\n");
	else if (isOperator(fd))
	{
		std::string remOp = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " -o " + name + "\r\n";
		sendToAllClients(remOp);
		_operatorsCha.erase(std::find(_operatorsCha.begin(), _operatorsCha.end(), fd));
		broadcastMsg(name + " lost his operator permissions.\r\n", -1);
	}
	else
	{
		std::string giveOp = ":" + _clientsCha.find(sFd)->second->getNickname() + " MODE " + _nameChannel + " +o " + name + "\r\n";
		sendToAllClients(giveOp);
		_operatorsCha.push_back(fd);
		broadcastMsg(name + " was made a channel operator.\r\n", -1);
	}
}

void Channel::setLimit(int sFd, std::string& arg) {
	int lim;
	std::stringstream ss2(arg);

	if ((ss2 >> lim).fail() || lim < 1)
		return sendMsg(sFd, "Error: invalid limit.\r\n");
	if (activeUsers() > (size_t)lim)
		sendMsg(sFd, "Error: can't change limit, too many clients in channel.\r\n");
	else if (lim > 100)
		sendMsg(sFd, "Error: why the F*CK would you need more than 100 clients? Bandwidth isn't free...\r\n");
	else
	{
		_usrLimit = lim;
		std::stringstream ss;
		ss << "User limit was changed to " << lim << ". " << capacity() << "\r\n";
		broadcastMsg(ss.str(), -1);
	}
}

// --------- CLIENT INFO ---------

bool Channel::canAddUsr() {
	return (activeUsers() < _usrLimit);
}

std::string Channel::capacity() {
	std::stringstream ss;
	ss << "(" << activeUsers() << "/" << _usrLimit << ")";
	return ss.str();
}

std::string Channel::getClist() {
	std::string output;

	for (std::map<int, Client*>::iterator it = _clientsCha.begin(); it != _clientsCha.end(); ++it) {
		if (isOperator(it->first) || isOwner(it->first))
			output += "@";
		output += it->second->getNickname();
		output += " ";
	}
	return output;
}

/*void Channel::removeOperator(int fd) {
	_operatorsCha.erase(std::remove(_operatorsCha.begin(), _operatorsCha.end(), fd), _operatorsCha.end());
}*/

/*void Channel::inviteClient(Client *client) {
	addClient(client);
}*/

// --------- MESSAGE ---------

void Channel::sendToAllClients(const std::string &msg)
{
	for (std::map<int, Client*>::iterator it = _clientsCha.begin(); it != _clientsCha.end(); ++it) {
		int fd = it->first; //gets the file descriptor
		sendMsg(fd, msg);
	}
}

void Channel::broadcastMsg(const std::string &msg, int sFd) { // Use sFd=-1 if making a broadcast from the server
	std::string assembled;
	if (sFd == -1)
		assembled = "Server: " + msg;
	// else
	// {
	// 	//assembled = _clientsCha.find(sFd)->second->getNickname() + ": " + msg;
	// 	assembled = msg;
	// }
	for (std::map<int, Client*>::iterator it = _clientsCha.begin(); it != _clientsCha.end(); ++it) {
		int fd = it->first; //gets the file descriptor
		if (fd != sFd) //does not send the msg to the sender --- why?
			sendMsg(fd, msg);
	}
}

void Channel::sendMsg(int fd, const std::string &msg) {
	send(fd, msg.c_str(), msg.size(), 0);
}

// --------- PARSING ---------

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
			Channel::inviteClient(args, sFd);
			break;
		case 2:
			Channel::changeTopic(args, sFd);
			break;
		case 3:
			Channel::changeMode(args, sFd);
			break;
		default:
			return false;
	}
	return true;
}