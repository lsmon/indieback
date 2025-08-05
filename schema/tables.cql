CREATE KEYSPACE indie_pub WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'} AND durable_writes = true;

CREATE TABLE indie_pub.band_members ( 
    band_id uuid, 
    user_id uuid, 
    PRIMARY KEY (band_id, user_id) 
) WITH CLUSTERING ORDER BY (user_id ASC);

CREATE TABLE indie_pub.bands (
    band_id uuid,
    created_at timestamp,
    description text,
    genre text,
    name text,
    PRIMARY KEY (band_id, created_at)
) WITH CLUSTERING ORDER BY (created_at DESC);

CREATE INDEX idx_bands_genre ON indie_pub.bands (genre);
CREATE INDEX idx_bands_name ON indie_pub.bands (name);
CREATE TABLE indie_pub.credentials (
    user_id uuid PRIMARY KEY,
    auth_token text,
    pw_hash text
);
CREATE TABLE indie_pub.daily_ticket_sales (
    event_id uuid,
    sale_date timestamp,
    tickets_sold counter,
    PRIMARY KEY (event_id, sale_date)
) WITH CLUSTERING ORDER BY (sale_date DESC);

CREATE TABLE indie_pub.events_by_venue (
    venue_id uuid,
    date timestamp,
    band_id uuid,
    capacity int,
    creator_id uuid,
    event_id uuid,
    name text,
    price double,
    sold int,
    PRIMARY KEY (venue_id, date)
) WITH CLUSTERING ORDER BY (date DESC);

CREATE INDEX idx_events_by_band_id ON indie_pub.events_by_venue (band_id);
CREATE INDEX idx_events_by_capacity ON indie_pub.events_by_venue (capacity);
CREATE INDEX idx_events_by_creator_id ON indie_pub.events_by_venue (creator_id);
CREATE INDEX idx_events_by_event_id ON indie_pub.events_by_venue (event_id);
CREATE INDEX idx_events_by_name ON indie_pub.events_by_venue (name);
CREATE INDEX idx_events_by_price ON indie_pub.events_by_venue (price);
CREATE INDEX idx_events_by_sold ON indie_pub.events_by_venue (sold);

CREATE TABLE indie_pub.posts_by_date (
    post_id uuid,
    created_at timestamp,
    content text,
    user_id uuid,
    PRIMARY KEY (post_id, created_at)
) WITH CLUSTERING ORDER BY (created_at DESC);

CREATE INDEX idx_posts_by_user_id ON indie_pub.posts_by_date (user_id);

CREATE TABLE indie_pub.tickets_by_event (
    event_id uuid,
    ticket_id uuid,
    purchase_date timestamp,
    user_id uuid,
    PRIMARY KEY (event_id, ticket_id, purchase_date)
) WITH CLUSTERING ORDER BY (ticket_id DESC, purchase_date DESC);

CREATE TABLE indie_pub.tickets_by_user (
    user_id uuid,
    ticket_id uuid,
    purchase_date timestamp,
    event_id uuid,
    PRIMARY KEY (user_id, ticket_id, purchase_date)
) WITH CLUSTERING ORDER BY (ticket_id DESC, purchase_date DESC);

CREATE INDEX idx_tickets_by_event_id ON indie_pub.tickets_by_user (event_id);

CREATE TABLE IF NOT EXISTS indie_pub.users (
    user_id uuid,
    created_at timestamp,
    email text,
    name text,
    role text,
    bio text,
    profile_picture text,
    social_links list<text>,
    PRIMARY KEY (user_id, created_at)
) WITH CLUSTERING ORDER BY (created_at DESC);

CREATE INDEX idx_users_email ON indie_pub.users (email);
CREATE INDEX idx_users_name ON indie_pub.users (name);
CREATE INDEX idx_users_role ON indie_pub.users (role);

CREATE TABLE indie_pub.venues (
    venue_id uuid,
    created_at timestamp,
    capacity int,
    location text,
    name text,
    owner_id uuid,
    PRIMARY KEY (venue_id, created_at)
) WITH CLUSTERING ORDER BY (created_at DESC);
CREATE INDEX idx_venues_capacity ON indie_pub.venues (capacity);
CREATE INDEX idx_venues_location ON indie_pub.venues (location);
CREATE INDEX idx_venues_name ON indie_pub.venues (name);
CREATE INDEX idx_venues_owner_id ON indie_pub.venues (owner_id);

DROP TABLE IF EXISTS indie_pub.venue_members;

CREATE TABLE IF NOT EXISTS indie_pub.venue_members (
    venue_id uuid,
    user_id uuid,
    role text,
    joined_at timestamp,
    active boolean,
    PRIMARY KEY (venue_id, user_id, joined_at)
) WITH CLUSTERING ORDER BY (user_id ASC, joined_at DESC);

CREATE INDEX idx_venue_members_role ON indie_pub.venue_members (role);
CREATE INDEX idx_venue_members_active ON indie_pub.venue_members (active);