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
			"\"password\"	TEXT NOT NULL,"
			"\"public_key\"	TEXT UNIQUE,"
			"PRIMARY KEY(\"username\")"
		");");

		usersTable.executeStep();

		SQLite::Statement queuedTable(sqlDb, "CREATE TABLE IF NOT EXISTS \"queued_messages\" ("
			"\"who\"	TEXT,"
			"\"msg\"	TEXT,"
			"PRIMARY KEY(\"who\")"
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
	return false;
}

bool db::registerUser(Types::UserInfo &User)
{
    return false;
}

bool db::login(Types::UserInfo& User)
{
	return false;
}
