SELECT avg(budget / box)
FROM movies
WHERE country = 'Ind'
  AND genre = 'comedy';
