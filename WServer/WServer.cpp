#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <functional>

//using namespace std приводит к конфликту
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::invalid_argument;
using std::exception;
using namespace std::placeholders;

/*
REGISTER_USER — Регистрация нового пользователя.
LOGIN_USER — Авторизация пользователя.
LOGOUT_USER — Выход пользователя из системы.
GET_TRAVEL_GROUPS — Получение списка доступных групп путешествий.
GET_GROUP_DETAILS — Получение подробной информации о конкретной группе путешествий.
GET_TRAVEL_CARDS — Получение списка рекомендованных путешествий в группе.
GET_TRAVEL_CARD_DETAILS — Получение подробной информации о конкретном маршруте путешествия.
RATE_TRAVEL_CARD — Оценка маршрута путешествия.
CREATE_TRAVEL_ROUTE — Создание нового маршрута путешествия.
UPDATE_TRAVEL_ROUTE — Обновление существующего маршрута путешествия.
DELETE_TRAVEL_ROUTE — Удаление маршрута путешествия.
GET_USER_PROFILE — Получение информации о профиле пользователя.
UPDATE_USER_PROFILE — Обновление информации профиля пользователя.
GET_USER_ROUTES — Получение списка маршрутов, созданных пользователем.
EDIT_USER_ROUTE — Редактирование маршрута пользователя.
CREATE_TRAVEL_GROUP — Создание новой группы путешественников.
JOIN_TRAVEL_GROUP — Присоединение к группе путешественников.
LEAVE_TRAVEL_GROUP — Выход из группы путешественников.
ORGANIZE_TRIP — Организация поездки для группы путешественников.
SEND_MESSAGE — Отправка сообщения в чат группы.
RECEIVE_MESSAGE — Получение новых сообщений из чата.
GET_CHAT_HISTORY — Получение истории сообщений в чате.
CREATE_CHAT_GROUP — Создание новой группы для общения.
*/

// see: obsidian://open?vault=Obsidian_data&file=Cpp%2Fwebsockets%2FExamples

#define CMD_LOGIN_USER "LOGIN_USER"
#define CMD_CHECK_USER "CHECK_USER"
#define CMD_SOME_OTHER_COMMAND "OTHER_COMMAND"
#define CMD_REGISTER_USER "REGISTER_USER"
#define CMD_LOGOUT_USER "LOGOUT_USER"
#define CMD_GET_TRAVEL_GROUPS "GET_TRAVEL_GROUPS"
#define CMD_CREATE_TRAVEL_GROUP "CREATE_TRAVEL_GROUP"
#define CMD_GET_GROUP_DETAILS "GET_GROUP_DETAILS"
#define CMD_GET_TRAVEL_CARDS "GET_TRAVEL_CARDS"
#define CMD_GET_TRAVEL_CARD_DETAILS "GET_TRAVEL_CARD_DETAILS"
#define CMD_RATE_TRAVEL_CARD "RATE_TRAVEL_CARD"
#define CMD_CREATE_TRAVEL_ROUTE "CREATE_TRAVEL_ROUTE"
#define CMD_UPDATE_TRAVEL_ROUTE "UPDATE_TRAVEL_ROUTE"
#define CMD_DELETE_TRAVEL_ROUTE "DELETE_TRAVEL_ROUTE"
#define CMD_GET_USER_PROFILE "GET_USER_PROFILE"
#define CMD_UPDATE_USER_PROFILE "UPDATE_USER_PROFILE"
#define CMD_GET_USER_ROUTES "GET_USER_ROUTES"
#define CMD_EDIT_USER_ROUTE "EDIT_USER_ROUTE"
#define CMD_JOIN_TRAVEL_GROUP "JOIN_TRAVEL_GROUP"
#define CMD_ORGANIZE_TRIP "ORGANIZE_TRIP"
#define CMD_LEAVE_TRAVEL_GROUP "LEAVE_TRAVEL_GROUP"
#define CMD_SEND_MESSAGE "SEND_MESSAGE"
#define CMD_RECEIVE_MESSAGE "RECEIVE_MESSAGE"
#define CMD_GET_CHAT_HISTORY "GET_CHAT_HISTORY"
#define CMD_CREATE_CHAT_GROUP "CREATE_CHAT_GROUP"

typedef websocketpp::server<websocketpp::config::asio> server;

// Прототип функции handle_command
void handle_command(const string& command, const nlohmann::json& data, pqxx::connection& C);

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
	try {
		// Парсим сообщение JSON
		nlohmann::json json_msg = nlohmann::json::parse(msg->get_payload());

		// Проверяем наличие поля "command"
		if (!json_msg.contains("command") || !json_msg["command"].is_string()) {
			throw invalid_argument("Invalid command format");
		}

		string command = json_msg["command"];

		// Проверяем наличие поля "data"
		nlohmann::json data = json_msg.contains("data") ? json_msg["data"] : nlohmann::json{};

		//pqxx::connection C("dbname=travel_service user=your_username password=your_password hostaddr=127.0.0.1 port=5432");
		pqxx::connection C;
		// Обработка команды
		handle_command(command, data, C);

		// Отправляем ответ об успешной обработке
		s->send(hdl, "Command processed", websocketpp::frame::opcode::text);
	}
	catch (const nlohmann::json::parse_error& e) {
		cerr << "JSON parse error: " << e.what() << endl;
		s->send(hdl, "JSON parse error", websocketpp::frame::opcode::text);
	}
	catch (const invalid_argument& e) {
		cerr << "Invalid argument: " << e.what() << endl;
		s->send(hdl, "Invalid command", websocketpp::frame::opcode::text);
	}
	catch (const exception& e) {
		cerr << "Failed to process message: " << e.what() << endl;
		s->send(hdl, "An error occurred", websocketpp::frame::opcode::text);
	}
}

void handle_command(const string& command, const nlohmann::json& data, pqxx::connection& C) {
	if (command == CMD_CHECK_USER) {
		// Логика обработки команды CMD_CHECK_USER
		string username = data["username"];
		cout << "Checking user: " << username << endl;
		// Дополнительная логика обработки
	}

	else if (command == CMD_REGISTER_USER) {
		cout << "Handling registration of new user" << endl;
		// Логика регистрации пользователя
		string username = data["username"];
		string password = data["password"];
		string email = data["email"]; // Например, если нужно также собрать email

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			// Проверка, существует ли пользователь с таким же именем пользователя или email
			string check_user_query = "SELECT * FROM users WHERE username = " + W.quote(username) + " OR email = " + W.quote(email) + ";";
			pqxx::result R = W.exec(check_user_query);

			if (R.size() == 0) {
				// Пользователь не существует, создаем нового пользователя
				string insert_query = "INSERT INTO users (username, password, email) VALUES (" + W.quote(username) + ", " + W.quote(password) + ", " + W.quote(email) + ");";
				W.exec(insert_query);
				W.commit();
				cout << "User registered successfully" << endl;
				// Логика успешной регистрации (например, отправка подтверждения)
			}
			else {
				cout << "User with this username or email already exists" << endl;
				// Логика ошибки при регистрации (например, возврат ошибки клиенту)
			}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_LOGIN_USER) {
		cout << "Handling user login" << endl;
		// Логика авторизации пользователя
		string username = data["username"];
		string password = data["password"];

		try {
			// Подключение к базе данных PostgreSQL
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;

			// Выполнение запроса на проверку пользователя
			string query = "SELECT * FROM users WHERE username = " + W.quote(username) + " AND password = " + W.quote(password) + ";";
			pqxx::result R = W.exec(query);

			if (R.size() == 1) {
				cout << "User authenticated successfully" << endl;
				// Логика успешной авторизации (например, генерация токена сессии)
			}
			else {
				cout << "Invalid username or password" << endl;
				// Логика неуспешной авторизации
			}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_LOGOUT_USER) {
		cout << "Handling user logout" << endl;

		// Логика выхода пользователя
		string username = data["username"];

		// Обновление статуса пользователя в базе данных или удаление сессии
		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "UPDATE users SET status='offline' WHERE username = " + W.quote(username) + ";";
			W.exec(query);
			W.commit();
			cout << "User logged out successfully" << endl;
			// Дополнительная логика выхода пользователя (например, удаление токена сессии)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_GET_TRAVEL_GROUPS) {
		cout << "Handling retrieval of travel groups" << endl;

		// Логика получения групп путешествий
		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;

			string query = "SELECT * FROM travel_groups;";
			pqxx::result R = W.exec(query);

			nlohmann::json response;

			for (auto row : R) {
				nlohmann::json group;
				group["group_id"] = row["group_id"].as<int>();
				group["group_name"] = row["group_name"].as<string>();
				response.push_back(group);
			}

			cout << "Travel groups retrieved successfully" << endl;
			// Отправка ответа клиенту с группами путешествий
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_CREATE_TRAVEL_GROUP) {
		cout << "Handling creation of travel group" << endl;

		// Логика создания новой группы путешествий
		string group_name = data["group_name"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;

			string query = "INSERT INTO travel_groups (group_name) VALUES (" + W.quote(group_name) + ");";
			W.exec(query);
			W.commit();
			cout << "Travel group created successfully" << endl;
			// Дополнительная логика создания группы (например, отправка подтверждения клиенту)
		}

		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_GET_GROUP_DETAILS) {
		cout << "Handling retrieval of group details" << endl;

		// Логика получения подробной информации о группе
		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;

			string query = "SELECT * FROM travel_groups WHERE group_id = " + W.quote(group_id) + ";";
			pqxx::result R = W.exec(query);

			if (R.size() == 1) {
				nlohmann::json response;
				response["group_id"] = R[0]["group_id"].as<int>();
				response["group_name"] = R[0]["group_name"].as<string>();
				// Добавление другой информации, если необходимо
				cout << "Group details retrieved successfully" << endl;
				// Отправка ответа клиенту с деталями группы
			}
			else {
				cout << "Group not found" << endl;
				// Логика обработки случая, когда группа не найдена
			}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_GET_TRAVEL_CARDS) {
		cout << "Handling retrieval of travel cards" << endl;

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;

			string query = "SELECT * FROM travel_cards;";
			pqxx::result R = W.exec(query);

			nlohmann::json response;
			for (auto row : R) {
				nlohmann::json card;
				card["card_id"] = row["card_id"].as<int>();
				card["title"] = row["title"].as<string>();
				card["description"] = row["description"].as<string>();
				card["rating"] = row["rating"].as<float>();
				response.push_back(card);
			}

			cout << "Travel cards retrieved successfully" << endl;
			// Отправка ответа клиенту с карточками путешествий
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_GET_TRAVEL_CARD_DETAILS) {
		cout << "Handling retrieval of travel card details" << endl;

		int card_id = data["card_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "SELECT * FROM travel_cards WHERE card_id = " + W.quote(card_id) + ";";
			pqxx::result R = W.exec(query);

			if (R.size() == 1) {
				nlohmann::json response;
				response["card_id"] = R[0]["card_id"].as<int>();
				response["title"] = R[0]["title"].as<string>();
				response["description"] = R[0]["description"].as<string>();
				response["rating"] = R[0]["rating"].as<float>();
				// Добавление другой информации, если необходимо

				W.commit();
				cout << "Travel card details retrieved successfully" << endl;
				// Отправка ответа клиенту с деталями карточки путешествия
			}
			else {
				cout << "Travel card not found" << endl;
				// Логика обработки случая, когда карточка не найдена
			}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_RATE_TRAVEL_CARD) {
		cout << "Handling rating of travel card" << endl;

		int card_id = data["card_id"];
		float rating = data["rating"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "UPDATE travel_cards SET rating = " + W.quote(rating) + " WHERE card_id = " + W.quote(card_id) + ";";
			W.exec(query);
			W.commit();
			cout << "Travel card rated successfully" << endl;
			// Дополнительная логика оценки маршрута (например, обновление рейтинга)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_CREATE_TRAVEL_ROUTE) {
		cout << "Handling creation of travel route" << endl;

		string title = data["title"];
		string description = data["description"];
		float rating = data["rating"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO travel_routes (title, description, rating) VALUES (" + W.quote(title) + ", " + W.quote(description) + ", " + W.quote(rating) + ");";
			W.exec(query);
			W.commit();
			cout << "Travel route created successfully" << endl;
			// Дополнительная логика создания маршрута (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_UPDATE_TRAVEL_ROUTE) {
		cout << "Handling update of travel route" << endl;

		int route_id = data["route_id"];
		string title = data["title"];
		string description = data["description"];
		float rating = data["rating"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "UPDATE travel_routes SET title = " + W.quote(title) + ", description = " + W.quote(description) + ", rating = " + W.quote(rating) + " WHERE route_id = " + W.quote(route_id) + ";";
			W.exec(query);
			W.commit();
			cout << "Travel route updated successfully" << endl;
			// Дополнительная логика обновления маршрута (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_DELETE_TRAVEL_ROUTE) {
		cout << "Handling deletion of travel route" << endl;

		int route_id = data["route_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "DELETE FROM travel_routes WHERE route_id = " + W.quote(route_id) + ";";
			W.exec(query);
			W.commit();
			cout << "Travel route deleted successfully" << endl;
			// Дополнительная логика удаления маршрута (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else  if (command == CMD_GET_USER_PROFILE) {
		cout << "Handling retrieval of user profile" << endl;

		string username = data["username"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "SELECT * FROM users WHERE username = " + W.quote(username) + ";";
			pqxx::result R = W.exec(query);

			if (R.size() == 1) {
				nlohmann::json response;
				response["username"] = R[0]["username"].as<string>();
				response["email"] = R[0]["email"].as<string>();
				response["status"] = R[0]["status"].as<string>();
				// Добавление другой информации, если необходимо

				W.commit();
				cout << "User profile retrieved successfully" << endl;
				// Отправка ответа клиенту с профилем пользователя
			}
			else {
				cout << "User not found" << endl;
				// Логика обработки случая, когда пользователь не найден
			}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_UPDATE_USER_PROFILE) {
		cout << "Handling update of user profile" << endl;

		string username = data["username"];
		string email = data["email"];
		string status = data["status"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "UPDATE users SET email = " + W.quote(email) + ", status = " + W.quote(status) + " WHERE username = " + W.quote(username) + ";";
			W.exec(query);
			W.commit();
			cout << "User profile updated successfully" << endl;
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_GET_USER_ROUTES) {
		cout << "Handling retrieval of user's routes" << endl;

		string username = data["username"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "SELECT * FROM travel_routes WHERE username = " + W.quote(username) + ";";
			pqxx::result R = W.exec(query);

			nlohmann::json response;
			for (auto row : R) {
				nlohmann::json route;
				route["route_id"] = row["route_id"].as<int>();
				route["title"] = row["title"].as<string>();
				route["description"] = row["description"].as<string>();
				route["rating"] = row["rating"].as<float>();
				response.push_back(route);
			}
			cout << "User routes retrieved successfully" << endl;
			// Отправка ответа клиенту с маршрутами пользователя
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_EDIT_USER_ROUTE) {
		cout << "Handling edit of user's route" << endl;

		int route_id = data["route_id"];
		string title = data["title"];
		string description = data["description"];
		float rating = data["rating"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "UPDATE travel_routes SET title = " + W.quote(title) + ", description = " + W.quote(description) + ", rating = " + W.quote(rating) + " WHERE route_id = " + W.quote(route_id) + ";";
			W.exec(query);
			W.commit();
			cout << "User route edited successfully" << endl;
			// Дополнительная логика редактирования маршрута пользователя (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_JOIN_TRAVEL_GROUP) {
		cout << "Handling joining travel group" << endl;

		string username = data["username"];
		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO group_members (group_id, username) VALUES (" + W.quote(group_id) + ", " + W.quote(username) + ");";
			W.exec(query);
			W.commit();
			cout << "User joined travel group successfully" << endl;
			// Дополнительная логика присоединения к группе путешественников (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}
	else if (command == CMD_JOIN_TRAVEL_GROUP) {
		cout << "Handling joining travel group" << endl;

		string username = data["username"];
		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO group_members (group_id, username) VALUES (" + W.quote(group_id) + ", " + W.quote(username) + ");";
			W.exec(query);
			W.commit();
			cout << "User joined travel group successfully" << endl;
			// Дополнительная логика присоединения к группе путешественников (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_ORGANIZE_TRIP) {
		cout << "Handling organization of trip" << endl;

		int group_id = data["group_id"];
		string trip_details = data["trip_details"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO trips (group_id, details) VALUES (" + W.quote(group_id) + ", " + W.quote(trip_details) + ");";
			W.exec(query);
			W.commit();
			cout << "Trip organized successfully" << endl;
			// Дополнительная логика организации поездки (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_LEAVE_TRAVEL_GROUP) {
		cout << "Handling leaving travel group" << endl;

		string username = data["username"];
		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "DELETE FROM group_members WHERE group_id = " + W.quote(group_id) + " AND username = " + W.quote(username) + ";";
			W.exec(query);
			W.commit();
			cout << "User left travel group successfully" << endl;
			// Дополнительная логика выхода из группы путешественников (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_SEND_MESSAGE) {
		cout << "Handling sending message" << endl;

		string username = data["username"];
		string message = data["message"];
		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO messages (group_id, username, message) VALUES (" + W.quote(group_id) + ", " + W.quote(username) + ", " + W.quote(message) + ");";
				W.exec(query);
				W.commit();
				cout << "Message sent successfully" << endl;
				// Дополнительная логика отправки сообщения (например, уведомление участников группы)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_RECEIVE_MESSAGE) {
		cout << "Handling receiving message" << endl;

		int group_id = data["group_id"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "SELECT * FROM messages WHERE group_id = " + W.quote(group_id) + " ORDER BY timestamp DESC LIMIT 1;";
				pqxx::result R = W.exec(query);

				if (!R.empty()) {
					nlohmann::json response;
					response["username"] = R[0]["username"].as<string>();
					response["message"] = R[0]["message"].as<string>();
					response["timestamp"] = R[0]["timestamp"].as<string>();
					cout << "Message received successfully" << endl;
					// Отправка ответа клиенту с последним сообщением
				}
				else {
					cout << "No messages found for the given group" << endl;
					// Логика обработки случая, когда сообщений нет
				}
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_GET_CHAT_HISTORY) {
		cout << "Handling retrieval of chat history" << endl;

		int group_id = data["group_id"];
		int limit = data["limit"];  // Количество сообщений для получения

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "SELECT * FROM messages WHERE group_id = " + W.quote(group_id) + " ORDER BY timestamp DESC LIMIT " + W.quote(limit) + ";";
				pqxx::result R = W.exec(query);

				nlohmann::json response;
				for (auto row : R) {
					nlohmann::json message;
					message["username"] = row["username"].as<string>();
					message["message"] = row["message"].as<string>();
					message["timestamp"] = row["timestamp"].as<string>();
					response.push_back(message);
				}
				cout << "Chat history retrieved successfully" << endl;
				// Отправка ответа клиенту с историей сообщений
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else if (command == CMD_CREATE_CHAT_GROUP) {
		cout << "Handling creation of chat group" << endl;

		string group_name = data["group_name"];

		try {
			pqxx::work W(C);
			cout << "Connected to database successfully" << endl;
			string query = "INSERT INTO chat_groups (group_name) VALUES (" + W.quote(group_name) + ");";
				W.exec(query);
				W.commit();
				cout << "Chat group created successfully" << endl;
				// Дополнительная логика создания группы для общения (например, отправка подтверждения клиенту)
		}
		catch (const exception& e) {
			cerr << e.what() << endl;
		}
	}

	else {
		cout << "Unknown command: " << command << endl;
	}
}

int main() {
	server echo_server;
	echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));
	echo_server.init_asio();
	echo_server.listen(8080);
	echo_server.start_accept();
	echo_server.run();
}
