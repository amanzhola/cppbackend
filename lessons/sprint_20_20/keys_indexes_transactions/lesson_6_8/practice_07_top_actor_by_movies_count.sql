SELECT
    relations.person,
    persons.name,
    count(*) AS movies_count
FROM relations
JOIN persons ON persons.id = relations.person
WHERE relations.role = 'актёр'
GROUP BY relations.person, persons.name
ORDER BY count(*) DESC
LIMIT 10;
