SELECT
    movies.genre,
    count(awards.id) AS awards_count
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.year < 2000
GROUP BY movies.genre
ORDER BY awards_count DESC, movies.genre ASC;
