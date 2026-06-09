SELECT year, genre, count(*) AS count
FROM movies_temp
GROUP BY year, genre;
