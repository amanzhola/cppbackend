SELECT year, genre, count(*) AS movies_count
FROM movies_temp
GROUP BY year, genre
HAVING count(*) > 1
ORDER BY movies_count DESC, year, genre;
