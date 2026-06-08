START TRANSACTION;

UPDATE accounts
SET funds = funds - 50
WHERE name = 'Ostap';

UPDATE accounts
SET funds = funds + 50
WHERE name = 'Shura';

COMMIT;
