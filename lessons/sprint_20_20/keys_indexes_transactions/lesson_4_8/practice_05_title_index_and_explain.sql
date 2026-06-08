\timing on

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

CREATE INDEX movies_title_idx ON movies (title);

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY votes_count
LIMIT 10;
