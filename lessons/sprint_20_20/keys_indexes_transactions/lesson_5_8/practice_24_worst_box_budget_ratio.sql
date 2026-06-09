SELECT name,
       box,
       budget,
       box::float / budget AS box_budget_ratio
FROM movies
WHERE box > 0
ORDER BY box_budget_ratio ASC
LIMIT 1;
