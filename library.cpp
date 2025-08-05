
#include <string>

#include "message.h"
#include "pool.h"

namespace google::cloud::spanner::lib {
    Message CreatePool(const GoString& dsn_go_string) {
        const std::string dsn(static_cast<const char*>(dsn_go_string.data), static_cast<int>(dsn_go_string.len));

        const auto& pool = Pool::create(dsn);
        if (pool.ok()) {
            return Message{
                .pinner = 0,
                .code = 0,
                .object_id = pool.value()->id_,
                .length = 0,
                .data = nullptr,
            };
        }
        return Message{
            .pinner = 0,
            .code = pool.status().raw_code(),
            .object_id = 0,
            .length = 0,
            .data = nullptr,
        };
    }

    Message ClosePool(const int64_t pool_id) {
        if (const auto& pool = Pool::get(pool_id); !pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        } else {
            pool.value()->Close();
            delete pool.value();
        }
        return Message{};
    }

    Message CreateConnection(const int64_t pool_id) {
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->CreateConnection();
        const auto result = Message{
            .pinner = 0,
            .code = 0,
            .object_id = connection->id_,
            .length = 0,
            .data = nullptr,
        };
        return result;
    }

    Message CloseConnection(const int64_t pool_id, const int64_t connection_id) {
        if (const auto& pool = Pool::get(pool_id); !pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        } else {
            auto result = pool.value()->CloseConnection(connection_id);
            if (result.ok()) {
                return Message{};
            }
            return Message{.code = pool.status().raw_code(),};
        }
    }

    Message Execute(const int64_t pool_id, const int64_t connection_id, GoBytes request_bytes) {
        google::spanner::v1::ExecuteSqlRequest request;
        const auto len = static_cast<int32_t>(request_bytes.len);
        if (!request.ParseFromArray(request_bytes.data, len)) {
            const std::string request_string((char*) request_bytes.data, len);
            auto const message = "Failed to parse request: " + request_string;
            auto const message_len = (int) message.length();
            const unsigned char *c = (unsigned char*) message.c_str();
            return Message{.code = 1, .length = message_len, .data = c};
        }
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->GetConnection(connection_id);
        const auto rows = connection->Execute(&request);
        const auto result = Message{
            .pinner = 0,
            .code = 0,
            .object_id = rows->id_,
            .length = 0,
            .data = nullptr,
        };
        return result;
    }

    Message Metadata(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->GetConnection(connection_id);
        const auto rows = connection->GetRows(rows_id);
        if (rows == nullptr) {
            return Message{.code = 2};
        }
        const auto metadata = rows->Metadata();
        const auto size = static_cast<int32_t>(metadata->ByteSizeLong());
        const auto metadata_bytes = new u_int8_t[size];
        if (!metadata->SerializeToArray(metadata_bytes, size)) {
            return Message{};
        }
        const auto result = Message{
            .pinner = 0,
            .code = 0,
            .object_id = 0,
            .length = size,
            .data = metadata_bytes,
        };
        return result;
    }

    Message Next(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->GetConnection(connection_id);
        const auto rows = connection->GetRows(rows_id);
        if (rows == nullptr) {
            return Message{.code = 2};
        }
        const auto row = rows->Next();
        if (row == nullptr) {
            return Message{};
        }
        const auto size = static_cast<int32_t>(row->ByteSizeLong());
        const auto row_data = new u_int8_t[size];
        if (!row->SerializeToArray(row_data, size)) {
            return Message{};
        }
        const auto result = Message{
            .pinner = 0,
            .code = 0,
            .object_id = 0,
            .length = size,
            .data = row_data,
        };
        return result;
    }

    Message ResultSetStats(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->GetConnection(connection_id);
        const auto rows = connection->GetRows(rows_id);
        if (rows == nullptr) {
            return Message{.code = 2};
        }
        const auto stats = rows->ResultSetStats();
        const auto size = static_cast<int32_t>(stats->ByteSizeLong());
        const auto stats_bytes = new u_int8_t[size];
        if (!stats->SerializeToArray(stats_bytes, size)) {
            return Message{};
        }
        const auto result = Message{
            .pinner = 0,
            .code = 0,
            .object_id = 0,
            .length = size,
            .data = stats_bytes,
        };
        return result;
    }

    Message CloseRows(const int64_t pool_id, const int64_t connection_id, const int64_t rows_id) {
        const auto pool = Pool::get(pool_id);
        if (!pool.ok()) {
            return Message{.code = pool.status().raw_code(),};
        }
        const auto connection = pool.value()->GetConnection(connection_id);
        const auto rows = connection->GetRows(rows_id);
        rows->Close();
        delete rows;
        return Message{};
    }
}
