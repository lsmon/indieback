
TRUNCATE indie_pub.users;

TRUNCATE indie_pub.credentials;

SELECT JSON * FROM indie_pub.users WHERE email = 'lsmon714@naver.com' allow filtering;

SELECT * FROM indie_pub.users;

SELECT * FROM indie_pub.credentials;

SELECT JSON * FROM indie_pub.events_by_venue;

SELECT JSON * FROM indie_pub.events_by_venue WHERE date >= '2025-07-07 00:00:00' AND date < '2025-07-14 00:00:00' allow filtering;

desc keyspace indie_pub;