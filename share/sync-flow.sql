-- main - the db that has outdated data
-- extern -- the db that has up-to-date data

-- remove all records that exists in main db no longer exist in extern data
DELETE FROM flow WHERE
 id IN (SELECT m.id FROM flow m LEFT JOIN externdb.flow e 
   ON m.id == e.id WHERE e.id IS NULL);

-- update all records that are different; IS NOT is like '!=' but takes care of NULLs
UPDATE flow SET 
  itd = (SELECT e.itd FROM externdb.flow e WHERE e.id == flow.id),
  dtm = (SELECT e.dtm FROM externdb.flow e WHERE e.id == flow.id),
  cnt = (SELECT e.cnt FROM externdb.flow e WHERE e.id == flow.id),
  dark_time = (SELECT e.dark_time FROM externdb.flow e WHERE e.id == flow.id),
  work_time = (SELECT e.work_time FROM externdb.flow e WHERE e.id == flow.id),
  flags = (SELECT e.flags FROM externdb.flow e WHERE e.id == flow.id)
  WHERE id IN (SELECT m.id FROM flow m JOIN externdb.flow e ON m.id == e.id
    WHERE m.itd IS NOT e.itd OR m.dtm IS NOT e.dtm OR m.cnt IS NOT e.cnt
      OR m.dark_time IS NOT e.dark_time OR m.work_time IS NOT e.work_time
      OR m.flags IS NOT e.flags);

-- insert all records that are missing
INSERT INTO flow SELECT e.* FROM externdb.flow e LEFT JOIN flow m ON m.id == e.id
  WHERE m.id IS NULL;

