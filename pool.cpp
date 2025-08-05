#include "pool.h"


namespace google::cloud::spanner::lib {
    std::atomic_int64_t Pool::POOL_ID_COUNTER_ = std::atomic_int64_t(1);
    absl::flat_hash_map<int64_t, Pool*> Pool::POOLS = absl::flat_hash_map<int64_t, Pool*>();

    absl::StatusOr<Pool*> Pool::create(const std::string& dsn) {
        const auto database = MakeDatabase(dsn).value();
        const auto connection = MakeConnection(database);
        auto client = Client(connection);
        const int64_t id = POOL_ID_COUNTER_++;
        auto pool = new Pool(id, dsn, std::move(client));
        POOLS.insert(std::make_pair(id, pool));
        return pool;
    }

    absl::StatusOr<Pool*> Pool::get(const int64_t id) {
        const auto result = POOLS.find(id);
        if (result == POOLS.end()) {
            return absl::NotFoundError(absl::StrCat("No pool with id: ", id));
        }
        return result->second;
    }

    void Pool::Close() const {
        for (const auto connection: connections_ | std::views::values) {
            connection->Close();
            delete connection;
        }
        POOLS.erase(id_);
    }

    Connection* Pool::CreateConnection() {
        auto id = ++connection_id_counter_;
        auto connection = new Connection(id, this);
        connections_.insert(std::make_pair(id, connection));
        return connection;
    }

    Connection* Pool::GetConnection(const int64_t id) const {
        if (connections_.contains(id)) {
            return connections_.at(id);
        }
        throw std::runtime_error(absl::StrCat("No pool with id: ", id));
    }

    absl::Status Pool::CloseConnection(const int64_t id) {
        const auto result = connections_.find(id);
        if (result == connections_.end()) {
            return absl::NotFoundError(absl::StrCat("No connection with id: ", id));
        }
        result->second->Close();
        connections_.erase(result);
        delete result->second;
        return absl::OkStatus();
    }

}