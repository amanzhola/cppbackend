SELECT id
FROM users
WHERE login='Ivan'
AND pass=crypt('qwerty', pass);

SELECT id
FROM users
WHERE login='Ivan'
AND pass=crypt('123456', pass);
