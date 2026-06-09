SELECT *
FROM relations
JOIN movies ON movies.id = relations.movie
JOIN persons ON persons.id = relations.person
WHERE movies.country = 'Fin'
  AND movies.year >= 2016
  AND relations.role = 'актёр';
