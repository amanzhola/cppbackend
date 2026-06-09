SELECT count(*) AS countries_count
FROM (
    SELECT country
    FROM movies
    WHERE year >= 2000
    GROUP BY country
    HAVING count(*) >= 20
) AS countries;
