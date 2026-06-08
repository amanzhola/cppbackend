\timing on

DROP INDEX IF EXISTS movies_title_idx;

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;

CREATE INDEX movies_year_title_idx ON movies (release_year, title);

SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;

EXPLAIN
SELECT *
FROM movies
WHERE release_year = 1990
ORDER BY title
LIMIT 10 OFFSET 1000;
