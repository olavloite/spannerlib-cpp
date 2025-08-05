#ifndef POOL_H
#define POOL_H
#include <cstdint>
#include <string>
#include <utility>
#include <absl/container/flat_hash_map.h>
#include <google/cloud/spanner/client.h>

#include "connection.h"


namespace google::cloud::spanner::lib {

class Pool {
public:
    static absl::StatusOr<Pool*> create(const std::string& dsn);
    static absl::StatusOr<Pool*> get(int64_t id);

    explicit Pool(const int64_t id, std::string   dsn, Client client) :
    id_(id), dsn_(std::move(dsn)), client_(std::move(client)) {
    }
    /// No default construction.
    Pool() = delete;

    int64_t id_;
    std::string dsn_;
    Client client_;

    void Close() const;
    Connection* CreateConnection();
    Connection* GetConnection(int64_t id) const;
    absl::Status CloseConnection(int64_t id);

private:
    static std::atomic_int64_t POOL_ID_COUNTER_;
    static absl::flat_hash_map<int64_t, Pool*> POOLS;

    std::atomic_int64_t connection_id_counter_;
    absl::flat_hash_map<int64_t, Connection*> connections_ = absl::flat_hash_map<int64_t, Connection*>();
};

}
#endif //POOL_H
