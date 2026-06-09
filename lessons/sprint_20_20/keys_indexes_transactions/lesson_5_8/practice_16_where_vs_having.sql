SELECT year,
       sum(box) AS total_box
FROM movies
WHERE year BETWEEN 2000 AND 2020
GROUP BY year
HAVING sum(box) > 25000000000
ORDER BY year;
