DROP TABLE IF EXISTS test_types;

CREATE TABLE test_types(
    id SERIAL PRIMARY KEY,
    age integer,
    month smallint,
    balance bigint,
    name varchar(100),
    description text,
    rating float8,
    active boolean,
    created_at timestamp DEFAULT now()
);
