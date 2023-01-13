CREATE TABLE IF NOT EXISTS "users" (
	"username"	TEXT NOT NULL UNIQUE,
	"password"	TEXT NOT NULL,
	"public_key"	TEXT UNIQUE,
	PRIMARY KEY("username")
);