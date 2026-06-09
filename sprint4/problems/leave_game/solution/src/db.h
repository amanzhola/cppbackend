#pragma once

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <string>
#include <utility>
#include <vector>

namespace db {

class ConnectionPool {
    using ConnectionPtr = std::shared_ptr<pqxx::connection>;

public:
    class ConnectionWrapper {
    public:
        ConnectionWrapper(ConnectionPtr&& conn, ConnectionPool& pool) noexcept
            : conn_{std::move(conn)}
            , pool_{&pool} {
        }

        ConnectionWrapper(const ConnectionWrapper&) = delete;
        ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;

        ConnectionWrapper(ConnectionWrapper&&) = default;
        ConnectionWrapper& operator=(ConnectionWrapper&&) = default;

        pqxx::connection& operator*() const noexcept {
            return *conn_;
        }

        pqxx::connection* operator->() const noexcept {
            return conn_.get();
        }

        ~ConnectionWrapper() {
            if (conn_) {
                pool_->ReturnConnection(std::move(conn_));
            }
        }

    private:
        ConnectionPtr conn_;
        ConnectionPool* pool_;
    };

    template <typename Factory>
    ConnectionPool(size_t capacity, Factory&& factory) {
        pool_.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            pool_.push_back(factory());
        }
    }

    ConnectionWrapper GetConnection() {
        std::unique_lock lock{mutex_};

        cond_var_.wait(lock, [this] {
            return used_connections_ < pool_.size();
        });

        return {std::move(pool_[used_connections_++]), *this};
    }

private:
    void ReturnConnection(ConnectionPtr&& conn) {
        {
            std::lock_guard lock{mutex_};
            assert(used_connections_ > 0);
            pool_[--used_connections_] = std::move(conn);
        }

        cond_var_.notify_one();
    }

    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::vector<ConnectionPtr> pool_;
    size_t used_connections_ = 0;
};

struct Record {
    std::string name;
    int score = 0;
    std::int64_t play_time_ms = 0;
};

class RetiredPlayersRepository {
public:
    explicit RetiredPlayersRepository(ConnectionPool& pool)
        : pool_{pool} {
    }

    void Init() {
        auto conn = pool_.GetConnection();
        pqxx::work tx{*conn};

        tx.exec(R"(
            CREATE EXTENSION IF NOT EXISTS pgcrypto;

            CREATE TABLE IF NOT EXISTS retired_players (
                id uuid PRIMARY KEY DEFAULT gen_random_uuid(),
                name text NOT NULL,
                score integer NOT NULL CHECK (score >= 0),
                play_time_ms bigint NOT NULL CHECK (play_time_ms >= 0)
            );

            CREATE INDEX IF NOT EXISTS retired_players_leaderboard_idx
            ON retired_players (score DESC, play_time_ms ASC, name ASC);
        )");

        tx.commit();
    }

    void Save(std::string name, int score, std::int64_t play_time_ms) {
        auto conn = pool_.GetConnection();
        pqxx::work tx{*conn};

        tx.exec_params(
            "INSERT INTO retired_players (name, score, play_time_ms) VALUES ($1, $2, $3)",
            name,
            score,
            play_time_ms
        );

        tx.commit();
    }

    std::vector<Record> GetRecords(size_t start, size_t max_items) {
        auto conn = pool_.GetConnection();
        pqxx::read_transaction tx{*conn};

        std::vector<Record> result;

        const std::string query =
            "SELECT name, score, play_time_ms "
            "FROM retired_players "
            "ORDER BY score DESC, play_time_ms ASC, name ASC "
            "LIMIT " + std::to_string(max_items) +
            " OFFSET " + std::to_string(start);

        for (auto [name, score, play_time_ms] :
             tx.query<std::string, int, std::int64_t>(query)) {
            result.push_back({std::move(name), score, play_time_ms});
        }

        return result;
    }

private:
    ConnectionPool& pool_;
};

}  // namespace db
