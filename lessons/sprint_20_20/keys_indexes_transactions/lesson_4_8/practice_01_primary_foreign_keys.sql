DROP TABLE IF EXISTS films;
DROP TABLE IF EXISTS persons;

CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TABLE persons (
    first_name text,
    last_name text,
    id uuid PRIMARY KEY DEFAULT gen_random_uuid()
);

CREATE TABLE films (
    title text NOT NULL,
    director uuid REFERENCES persons(id) NOT NULL,
    id SERIAL PRIMARY KEY
);

INSERT INTO persons (first_name, last_name)
VALUES ('Christopher', 'Nolan');

INSERT INTO films (title, director)
SELECT 'Inception', id
FROM persons
WHERE last_name = 'Nolan';

SELECT * FROM persons;
SELECT * FROM films;
