SELECT year, genre, sum(box) AS total_box
FROM movies_temp
GROUP BY year, genre
ORDER BY year, genre;
