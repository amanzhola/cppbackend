CREATE UNIQUE INDEX movies_director_title_idx
ON movies (director_id, title);

SELECT *
FROM movies
WHERE director_id = 42
LIMIT 1;
