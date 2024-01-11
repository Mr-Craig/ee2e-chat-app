#include "../Debug/Debug.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <future>
#include "../Types/Types.h"

#define SQL_FILENAME "info.db3"

class db
{
public:
	db();

	static std::shared_ptr<db> get();

	bool registerUser(Types::UserInfo& User);

	bool checkUsername(std::string& username);
	bool login(Types::UserInfo& User);

	bool updateNotificationSub(std::string& username, std::string& subscriptionInfo);
	std::string getNotificationSub(std::string& username);

	std::string getPublicKey(std::string& username);

	std::vector<Types::QueuedMessage> getQueuedMessages(std::string& username);
	bool deleteQueuedMessages(std::string& username);
	bool queueMessage(Types::QueuedMessage& qMessage);
private:
	static std::shared_ptr<db> dbPtr;

	SQLite::Database sqlDb = SQLite::Database(SQL_FILENAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

	bool createTables();
	bool integrityCheck();
	void optimizeDatabase();

	std::thread optimizeThread;
};