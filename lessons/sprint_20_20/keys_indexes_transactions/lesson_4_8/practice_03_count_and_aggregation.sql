SELECT count(*) FROM movies;

SELECT count(title) FROM movies;

SELECT count(director_id) FROM movies;

SELECT release_year, count(*)
FROM movies
GROUP BY release_year
ORDER BY release_year DESC
LIMIT 10;

SELECT count(*)
FROM movies
WHERE director_id = 42;
