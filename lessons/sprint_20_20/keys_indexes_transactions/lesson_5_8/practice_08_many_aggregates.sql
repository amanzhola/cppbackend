SELECT
    count(*),
    min(year),
    max(year),
    avg(budget),
    sum(box)
FROM movies;
