SELECT *
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
  AND awards.award LIKE '%кот'
ORDER BY movies.name;
