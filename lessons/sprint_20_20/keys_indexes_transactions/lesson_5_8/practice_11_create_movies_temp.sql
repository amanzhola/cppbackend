DROP TABLE IF EXISTS movies_temp;

CREATE TEMPORARY TABLE movies_temp
(
    LIKE movies INCLUDING ALL
);

SELECT *
FROM movies_temp;

INSERT INTO movies_temp
SELECT *
FROM movies
LIMIT 50;

SELECT *
FROM movies_temp;
