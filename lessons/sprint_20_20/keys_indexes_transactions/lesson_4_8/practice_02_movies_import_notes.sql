DROP TABLE IF EXISTS movies;

CREATE TABLE movies (
    id SERIAL PRIMARY KEY,
    director_id int,
    release_year integer,
    title text,
    votes_count integer DEFAULT(0),
    sum_votes float4 DEFAULT(0)
);
