
#include "../Debug/Debug.h"
#include <SQLiteCpp/SQLiteCpp.h>

class db
{
public:
	db();

	static std::shared_ptr<db> get();
private:
	static std::shared_ptr<db> dbPtr;

	SQLite::Database sqlDb = SQLite::Database("info.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
};