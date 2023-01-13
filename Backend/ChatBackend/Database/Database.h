#include "../Debug/Debug.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <future>

#define SQL_FILENAME "info.db3"

class db
{
public:
	db();

	static std::shared_ptr<db> get();

	static bool registerUser(std::string& username, std::string& password);
private:
	static std::shared_ptr<db> dbPtr;

	SQLite::Database sqlDb = SQLite::Database(SQL_FILENAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

	bool createTables();
	bool integrityCheck();
	void optimizeDatabase();
};