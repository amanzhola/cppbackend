\timing on

SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;

SELECT count(*)
FROM movies
WHERE votes_count = 0;

CREATE INDEX movies_director_rating_idx
ON movies (director_id, (sum_votes / votes_count))
WHERE votes_count > 0;

SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;

EXPLAIN
SELECT id, title, sum_votes / votes_count AS rating
FROM movies
WHERE director_id = 42
  AND votes_count >= 10
ORDER BY sum_votes / votes_count DESC
LIMIT 10;
