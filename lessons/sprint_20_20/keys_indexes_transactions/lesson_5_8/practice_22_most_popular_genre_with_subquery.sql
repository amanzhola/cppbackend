SELECT genre,
       count(*) AS movies_count
FROM movies
GROUP BY genre
HAVING count(*) =
(
    SELECT max(genre_count)
    FROM
    (
        SELECT count(*) AS genre_count
        FROM movies
        GROUP BY genre
    ) AS genre_counts
)
ORDER BY genre;
