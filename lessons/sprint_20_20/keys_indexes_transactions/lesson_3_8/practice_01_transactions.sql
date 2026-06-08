DROP TABLE IF EXISTS accounts;

CREATE TABLE accounts(
    id SERIAL PRIMARY KEY,
    funds integer CHECK(funds >= 0),
    name text
);

INSERT INTO accounts(funds,name)
VALUES
(50,'Ostap'),
(0,'Shura'),
(0,'Panikovsky');

SELECT * FROM accounts;
