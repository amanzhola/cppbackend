SELECT *
FROM movies
LEFT OUTER JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
ORDER BY movies.name;
