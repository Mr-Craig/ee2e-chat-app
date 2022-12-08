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
}
