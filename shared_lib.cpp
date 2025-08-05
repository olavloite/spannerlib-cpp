#include "shared_lib.h"
#include "google/cloud/spanner/client.h"

#include <iostream>

#include "library.h"

void hello() {
    namespace spanner = ::google::cloud::spanner;

    auto database = spanner::Database("appdev-soda-spanner-staging", "knut-test-ycsb", "knut-test-db");
    auto connection = MakeConnection(database);
    auto client = spanner::Client(connection);

    auto select = spanner::SqlStatement("SELECT col_bigint, col_varchar FROM all_types limit 10");
    using RowType = std::tuple<std::int64_t, std::optional<std::string>>;
    auto rows = client.ExecuteQuery(std::move(select));
    for (auto& row : spanner::StreamOf<RowType>(rows)) {
        if (!row) {
            std::cout << "error: " << row.status() << "\n";
            break;
        }
        std::cout << "col_bigint: " << std::get<0>(*row) << "\t";
        std::cout << "col_varchar: " << std::get<1>(*row).value_or("null") << "\n";
    }
}

auto pinner_counter = std::atomic_int64_t(1);
auto pinners_mutex = absl::Mutex();
auto pinners = absl::flat_hash_map<int64_t, const u_int8_t*>();

int32_t Release(const int64_t pinner_id) {
    if (pinner_id <= 0) {
        return 0;
    }
    absl::MutexLock lock(&pinners_mutex);
    const auto pointer = pinners.find(pinner_id);
    if (pointer == pinners.end()) {
        return 1;
    }
    pinners.erase(pointer);
    delete[] pointer->second;
    return 0;
}

void pin(Message& message) {
    if (message.length == 0) {
        return;
    }
    message.pinner = pinner_counter++;
    absl::MutexLock lock(&pinners_mutex);
    pinners.insert(std::pair(message.pinner, message.data));
}

Message CreatePool(GoString dsn) {
    auto message = google::cloud::spanner::lib::CreatePool(dsn);
    pin(message);
    return message;
}

Message ClosePool(const int64_t pool_id) {
    auto message = google::cloud::spanner::lib::ClosePool(pool_id);
    pin(message);
    return message;
}

Message CreateConnection(const int64_t pool_id) {
    auto message = google::cloud::spanner::lib::CreateConnection(pool_id);
    pin(message);
    return message;
}

Message CloseConnection(const int64_t pool_id, const int64_t connection_id) {
    auto message = google::cloud::spanner::lib::CloseConnection(pool_id, connection_id);
    pin(message);
    return message;
}

Message Execute(const int64_t pool_id, const int64_t connection_id, GoBytes request) {
    auto message = google::cloud::spanner::lib::Execute(pool_id, connection_id, request);
    pin(message);
    return message;
}

Message Metadata(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
    auto message = google::cloud::spanner::lib::Metadata(pool_id, connection_id, rows_id);
    pin(message);
    return message;
}

Message Next(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
    auto message = google::cloud::spanner::lib::Next(pool_id, connection_id, rows_id);
    pin(message);
    return message;
}

Message ResultSetStats(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
    auto message = google::cloud::spanner::lib::ResultSetStats(pool_id, connection_id, rows_id);
    pin(message);
    return message;
}

Message CloseRows(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
    auto message = google::cloud::spanner::lib::CloseRows(pool_id, connection_id, rows_id);
    pin(message);
    return message;
}
