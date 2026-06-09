SELECT genre,
       count(*) AS movies_count
FROM movies
WHERE country = 'Rus'
GROUP BY genre
ORDER BY movies_count DESC
LIMIT 1;
