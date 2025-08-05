
TRUNCATE indie_pub.users;

TRUNCATE indie_pub.credentials;

SELECT JSON * FROM indie_pub.users WHERE email = 'lsmon714@naver.com' allow filtering;

SELECT * FROM indie_pub.users;

SELECT * FROM indie_pub.credentials;

SELECT * FROM indie_pub.events_by_venue;

SELECT JSON * FROM indie_pub.events_by_venue WHERE date >= '2025-07-07 00:00:00' AND date < '2025-07-14 00:00:00' allow filtering;

desc keyspace indie_pub;

SELECT * FROM indie_pub.venues;

SELECT * FROM indie_pub.venue_members;

SELECT * FROM indie_pub.venue_members WHERE user_id = 1dc402e2-df45-2a9b-a2b2-9427847e89e7 ALLOW FILTERING;