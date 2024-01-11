
#include "../Utils/Argon2.h"
#include "Database.h"

std::shared_ptr<db> db::dbPtr = nullptr;

std::shared_ptr<db> db::get()
{
	if (!dbPtr) {
		dbPtr = std::make_shared<db>();
	}
	return std::shared_ptr<db>(dbPtr);
}

db::db()
{
	LOG("DB", "SQLite Database opened, Filename: {}", sqlDb.getFilename());

	
	if(!createTables())
		FATAL("DB", "Unable to create tables for database.");

	if(!integrityCheck())
		FATAL("DB", "SQLite Integrity check failed! Delete Database and relaunch");

	optimizeThread = std::thread(&db::optimizeDatabase, this);
}

bool db::createTables()
{
	try
	{
		SQLite::Statement usersTable(sqlDb, "CREATE TABLE IF NOT EXISTS \"users\" ("
			"\"username\"	TEXT NOT NULL UNIQUE,"
			"\"password\"	TEXT,"
			"\"public_key\"	TEXT,"
			"\"notification_sub\" TEXT,"
			"PRIMARY KEY(\"username\")"
		");");

		usersTable.executeStep();

		SQLite::Statement queuedTable(sqlDb, "CREATE TABLE IF NOT EXISTS \"queued_messages\" ("
			"\"who\"	TEXT,"
			"\"msg\"	TEXT,"
			"FOREIGN KEY(\"who\") REFERENCES users(\"username\")"
		");");

		queuedTable.executeStep();
		VERBOSE("DB", "Database Tables created");

		return true;
	}
	catch(const std::exception& e)
	{
		ERR("DB", "Unable to create database tables, Error: {}", e.what());
		return false;
	}

}

bool db::integrityCheck()
{
	try {
		auto start = std::chrono::system_clock::now();
		SQLite::Statement query(sqlDb, "PRAGMA integrity_check");

		// return false is nothing is returned, i don't think this is something that is possible but its best to check for it
		if(!query.executeStep())
			return false;

		std::string result = query.getColumn(0);

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;

		VERBOSE("DB", "Integrity check: {}, took: {}ms", result, elapsed_seconds.count() * 1000);
		
		return result == "ok";
	} catch(std::exception &e) {
		ERR("DB", "SQLite Integrity check failed! Error: {}", e.what());
		return false;
	}
}

void db::optimizeDatabase()
{
    try {
		auto start = std::chrono::system_clock::now();

		SQLite::Statement query(sqlDb, "PRAGMA optimize");

		query.executeStep();

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;

		VERBOSE("DB", "Optimized Database in {}ms", elapsed_seconds.count() * 1000);
	} catch(std::exception& e) {
		WARNING("DB", "Failed to optimize database, Error: {}", e.what());
	}
	/*
		Recommended by documentation to run this command every few hours to help with long-term query performance (https://www.sqlite.org/pragma.html#pragma_optimize)
	*/
	std::this_thread::sleep_for(std::chrono::hours(2));
	optimizeDatabase();
}


bool db::checkUsername(std::string & username)
{
	try {
		SQLite::Statement query(sqlDb, "SELECT COUNT(*) AS amount FROM users WHERE username = ?");
		query.bind(1, username);

		while(query.executeStep()) {
			int amount = query.getColumn(0);

			if(amount == 0)
				return true; 
		};

		return false;
	} catch(std::exception& e) {
		WARNING("DB", "Failed to check for username, Error: {}", e.what());
		return false;
	}
}

bool db::registerUser(Types::UserInfo &User)
{
	try {
		SQLite::Statement query(sqlDb, "INSERT INTO users(username, password, public_key) VALUES(?, ?, ?)");

		query.bind(1, User.username);
		query.bind(2, User.password);
		query.bind(3, User.publicKey);

		// false = done
		return !query.executeStep();
	} catch(std::exception& e) {
		WARNING("DB", "Failed to register user into database, Error: {}", e.what());
		return false;
	}
}

bool db::login(Types::UserInfo& User)
{
	try {
		SQLite::Statement query(sqlDb, "SELECT password FROM users WHERE username = ?");

		query.bind(1, User.username);

		while(query.executeStep()) {
			std::string hashedPass = query.getColumn(0);

			return utils::argon2::verifyPasswordHash(User.password, hashedPass);
		};
		
		// this shouldn't even return
		return false;
	} catch(std::exception& e) {
		WARNING("DB", "Failed to login user into database, Error: {}", e.what());
		return false;
	}
}

bool db::updateNotificationSub(std::string& username, std::string& subscriptionInfo)
{
	try {
		SQLite::Statement query(sqlDb, "UPDATE users SET notification_sub = ? WHERE username = ?");

		query.bind(1, subscriptionInfo);
		query.bind(2, username);

		return !query.executeStep();
	}
	catch (std::exception& e) {
		ERR("DB", "Failed to update user subscription, Error: {}", e.what());
		return false;
	}
}

std::string db::getNotificationSub(std::string& username)
{
	try
	{
		SQLite::Statement query(sqlDb, "SELECT notification_sub FROM users WHERE username = ?");
		query.bind(1, username);

		while (query.executeStep()) {
			std::string notifSubJson = query.getColumn(0);

			return notifSubJson;
		};
		// this shouldn't even return
		return "";
	}
	catch (std::exception& e) {
		WARNING("DB", "Failed to get Notification Sub, Error: {}", e.what());
		return "";
	}
}

std::string db::getPublicKey(std::string& username)
{
	try
	{
		SQLite::Statement query(sqlDb, "SELECT public_key FROM users WHERE username = ?");
		query.bind(1, username);

		while (query.executeStep()) {
			std::string publicKeyHex = query.getColumn(0);
			return publicKeyHex;
		};
		// this shouldn't even return
		return "";
	}
	catch (std::exception& e) {
		WARNING("DB", "Failed to get users public key, Error: {}", e.what());
		return "";
	}
}

bool db::deleteQueuedMessages(std::string & username)
{
	try {
		SQLite::Statement query(sqlDb, "DELETE FROM queued_messages WHERE who = ?");

		query.bind(1, username);

		// false = done
		return !query.executeStep();
	} catch(std::exception& e) {
		ERR("Queue", "Unable to delete queued messages, Error: {}", e.what());
		return false;
	}
}

bool db::queueMessage(Types::QueuedMessage & qMessage)
{
	try {
		SQLite::Statement query(sqlDb, "INSERT INTO queued_messages(who, msg) VALUES(?, ?)");

		query.bind(1, qMessage.who);
		query.bind(2, qMessage.message);

		// false = done
		return !query.executeStep();
	} catch(std::exception& e) {
		ERR("Queue", "Unable to insert queued message into DB, Error: {}", e.what());
		return false;
	}
}

std::vector<Types::QueuedMessage> db::getQueuedMessages(std::string & username)
{
	std::vector<Types::QueuedMessage> ret;
	try {
		SQLite::Statement query(sqlDb, "SELECT who, msg FROM queued_messages WHERE who = ?");

		query.bind(1, username);

		while(query.executeStep()) {
			ret.push_back(Types::QueuedMessage(query.getColumn(0 /*who*/), query.getColumn(1 /*message*/)));
		}

		return ret;
	} catch(std::exception& e) {
		ERR("Queue", "Unable to get queued messages from Database, Error: {}", e.what());
		return ret;
	}
}