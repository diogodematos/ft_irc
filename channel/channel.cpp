#include "channel.hpp"

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
	return _clients;
}

const std::vector<int> &Channel::getOperators() const {
	return _operatorsChannel;
}

void Channel::setTopic(const std::string &topic) {
	_topicChannel = topic;
}

void Channel::addClient(Client &client) {
	_clients[client.getFd()] = &client;
}

void Channel::removeClient(int fd) {
	_clients.erase(fd);
}

bool Channel::hasClient(int fd) const {
	return _clients.find(fd) != _clients.end();
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

void Channel::broadcastMsg(const std::string &msg, int sender_fd) {
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		int fd = it->first; //gets the file descriptor
		if (fd != sender_fd) { //does not send the msg to the sender
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
}