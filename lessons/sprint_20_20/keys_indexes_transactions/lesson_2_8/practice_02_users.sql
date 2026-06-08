CREATE EXTENSION IF NOT EXISTS pgcrypto;

DROP TABLE IF EXISTS users;

CREATE TABLE users(
    id SERIAL PRIMARY KEY,
    login varchar(100) NOT NULL,
    pass text NOT NULL,
    registered timestamp DEFAULT now()
);

INSERT INTO users(login, pass)
VALUES(
    'Ivan',
    crypt('qwerty', gen_salt('bf'))
);

INSERT INTO users(login, pass)
VALUES(
    'Petr',
    crypt('qwerty', gen_salt('bf'))
);

SELECT * FROM users;
