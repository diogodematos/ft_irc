#include "client.hpp"

Client::Client(int fd) {
	_username = "";
	_nickname = "";
	_hostname = "";
	_realName = "";
	_buffer = "";
	_fd = fd;
	_authState = false;
	_socketClient = -1;
}

Client::~Client() {
	//Destructor
}

int Client::getFd() const {
	return _fd;
}

const std::string Client::getUsername() {
	return _username;
}

const std::string Client::getNickname() {
	return _nickname;
}

const std::string Client::getHostname() {
	return _hostname;
}

const std::string Client::getRealName() {
	return _realName;
}

const std::string Client::buffer() {
	return _buffer;
}

void Client::setUsername(const std::string &username) {
	_username = username;
}

void Client::setNickname(const std::string &nickname) {
	_nickname = nickname;
}

void Client::setHostname(const std::string &hostname) {
	_hostname = hostname;
}

void Client::setRealName(const std::string &realName) {
	_realName = realName;
}

void Client::setAuthenticated(bool state) {
	_authState = state;
}

void Client::bufferAppend(const std::string &data) {
	_buffer = data;
}

void Client::clearBuffer() {
	_buffer.clear();
}

//determines if client is readu to fully register on server
bool Client::readyToRegist() const {
	return !_nickname.empty() && !_username.empty();
}