#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Server.hpp"
class Client;

class Channel {
	private:
		std::string _nameChannel;
		std::string _topicChannel;
		std::map<int, Client*> _clientsCha; // mapa dos fd's dos clientes
		std::vector<int> _operatorsChannel; // lista dos fds dos operadpres
	public:
		Channel();
		Channel(const std::string &name);
		~Channel();

		//getters
		const std::string &getNameChannel() const;
		const std::string &getTopicChannel() const;
		const std::map<int, Client*> &getClients() const;
		const std::vector<int> &getOperators() const;

		//setters
		void setTopic(const std::string  &topic);

		//client management
		void addClient(Client *client);
		void removeClient(int fd);
		bool hasClient(int fd) const;

		//operator management
		void addOperator(int fd);
		void removeOperator(int fd);
		bool isOperator(int fd) const;

		//message broadcast
		void broadcastMsg(const std::string &msg, int sender_fd);

		//operations
		void kickClient(int fd);
		void inviteClient(int fd);
		void changeTopic(std::string &topic);
		void changeMode(std::string &msg);
};

#endif //CHANNEL_HPP
