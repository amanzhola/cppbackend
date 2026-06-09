SELECT country,
       sum(box - budget) AS total_profit
FROM movies
GROUP BY country
ORDER BY total_profit DESC
LIMIT 1;
