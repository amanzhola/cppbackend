SELECT sum(budget)::numeric / sum(box) AS ratio
FROM movies
WHERE country = 'Ind'
  AND genre = 'comedy';
