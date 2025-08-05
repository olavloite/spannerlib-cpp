#include <google/spanner/v1/spanner.pb.h>

#include "shared_lib.h"

int main() {
    const auto dsn = "projects/appdev-soda-spanner-staging/instances/knut-test-ycsb/databases/knut-test-db";
    const auto dsn_go_string = GoString(dsn, static_cast<int64_t>(std::strlen(dsn)));
    auto pool = CreatePool(dsn_go_string);
    auto connection = CreateConnection(pool.object_id);
    auto request = google::spanner::v1::ExecuteSqlRequest();
    request.set_sql("select 'hello'");
    const auto size = request.ByteSize();
    uint8_t data[size];
    request.SerializeToArray(data, size);
    auto request_bytes = GoBytes(data, size);
    auto rows = Execute(pool.object_id, connection.object_id, request_bytes);
    auto row = Next(pool.object_id, connection.object_id, rows.object_id);
    if (row.length) {
        google::protobuf::ListValue row_values;
        row_values.ParseFromArray(row.data, row.length);
        for (const auto& col : row_values.values()) {
            std::cout << col.string_value() << std::endl;
        }
    }
}