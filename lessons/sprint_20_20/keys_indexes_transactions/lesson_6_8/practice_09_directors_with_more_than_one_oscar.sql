SELECT
    persons.id,
    persons.name,
    count(*)
FROM persons
JOIN relations ON persons.id = relations.person
JOIN awards ON awards.movie = relations.movie
WHERE relations.role = 'режиссёр'
  AND awards.award = 'Оскар'
GROUP BY persons.id, persons.name
HAVING count(*) > 1;
