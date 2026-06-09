EXPLAIN
SELECT year
FROM movies
WHERE year BETWEEN 2000 AND 2020
GROUP BY year
HAVING sum(box) =
(
    SELECT max(box_sum)
    FROM
    (
        SELECT sum(box) AS box_sum
        FROM movies
        WHERE year BETWEEN 2000 AND 2020
        GROUP BY year
    ) AS sum_tab
);
