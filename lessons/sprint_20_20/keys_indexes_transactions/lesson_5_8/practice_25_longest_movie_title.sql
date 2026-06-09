SELECT name,
       length(name) AS title_length
FROM movies
ORDER BY title_length DESC
LIMIT 1;
