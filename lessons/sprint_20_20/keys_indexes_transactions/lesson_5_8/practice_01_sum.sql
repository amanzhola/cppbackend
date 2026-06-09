SELECT budget
FROM movies
WHERE year = 2011
AND country = 'Arg';

SELECT sum(budget)
FROM movies
WHERE year = 2011
AND country = 'Arg';
