#include "client.hpp"

Client::Client() {
	_username = "";
	_nickname = "";
	_hostname = "";
	_realName = "";
	_buffer = "";
	_fd = -1;
	_authState = false;
	_socketClient = -1;
}

Client::~Client() {
	//Destructor
}

int Client::getFd() const {
	return _fd;
}

const std::string Client::getUsername() const{
	return _username;
}

const std::string Client::getNickname() const{
	return _nickname;
}

const std::string Client::getHostname() const{
	return _hostname;
}

const std::string Client::getRealName() const{
	return _realName;
}

const std::string Client::buffer() const{
	return _buffer;
}

bool Client::getAutheticated() const {
	return _authState;
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
	_buffer += data;
}

void Client::clearBuffer() {
	_buffer.clear();
}

//determines if client is readu to fully register on server
bool Client::isReadyToRegister() const {
	return !_nickname.empty() && !_username.empty();
}