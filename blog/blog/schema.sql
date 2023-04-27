PRAGMA journal_mode=WAL;
PRAGMA foreign_keys=ON;
PRAGMA strict=ON;

CREATE TABLE IF NOT EXISTS users (
	username TEXT NOT NULL UNIQUE,
	passhash TEXT NOT NULL,
	avatar_url TEXT
);

CREATE TABLE IF NOT EXISTS articles (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	author TEXT NOT NULL REFERENCES users(username) ON DELETE CASCADE,
	title TEXT NOT NULL,
	body TEXT NOT NULL,
	created_at INTEGER NOT NULL DEFAULT (unixepoch()),
	updated_at INTEGER
);
