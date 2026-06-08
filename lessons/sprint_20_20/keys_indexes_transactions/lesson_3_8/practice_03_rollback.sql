START TRANSACTION;

UPDATE accounts
SET funds = funds + 100
WHERE name = 'Shura';

UPDATE accounts
SET funds = funds - 100
WHERE name = 'Ostap';

ROLLBACK;
