SELECT
    movies.name,
    count(*) AS patricks_count
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE awards.award LIKE '%Патрик'
GROUP BY movies.id, movies.name
ORDER BY count(*) DESC
LIMIT 1;
