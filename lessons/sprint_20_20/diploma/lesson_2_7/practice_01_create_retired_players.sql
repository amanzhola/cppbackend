CREATE EXTENSION IF NOT EXISTS pgcrypto;

DROP TABLE IF EXISTS retired_players;

CREATE TABLE retired_players (
    id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    name text NOT NULL,
    score integer NOT NULL CHECK (score >= 0),
    play_time_ms bigint NOT NULL CHECK (play_time_ms >= 0)
);

CREATE INDEX retired_players_leaderboard_idx
ON retired_players (
    score DESC,
    play_time_ms ASC,
    name ASC
);

\d retired_players
