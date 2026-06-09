SELECT
    name,
    score,
    play_time_ms
FROM retired_players
ORDER BY
    score DESC,
    play_time_ms ASC,
    name ASC;
