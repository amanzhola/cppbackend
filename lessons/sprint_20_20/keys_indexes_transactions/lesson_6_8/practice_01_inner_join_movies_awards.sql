SELECT *
FROM movies
JOIN awards ON movies.id = awards.movie
WHERE movies.country = 'Fin'
ORDER BY movies.name;
