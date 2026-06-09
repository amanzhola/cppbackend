SELECT genre,
       count(*) AS movies_count
FROM movies
GROUP BY genre
ORDER BY movies_count DESC
LIMIT 1;
