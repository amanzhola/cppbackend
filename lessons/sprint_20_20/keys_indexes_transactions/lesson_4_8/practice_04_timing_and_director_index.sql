\timing on

SELECT count(*) FROM movies WHERE director_id = 42;

CREATE INDEX movies_director_idx ON movies (director_id);

SELECT count(*) FROM movies WHERE director_id = 42;

\d movies
