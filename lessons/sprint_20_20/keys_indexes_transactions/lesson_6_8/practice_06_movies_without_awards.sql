SELECT count(*) AS movies_without_awards
FROM movies
LEFT JOIN awards ON movies.id = awards.movie
WHERE awards.id IS NULL;
