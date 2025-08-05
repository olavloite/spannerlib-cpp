
#ifndef LIBRARY_H
#define LIBRARY_H
#include "message.h"

namespace google::cloud::spanner::lib {
    Message CreatePool(const GoString& dsn);
    Message ClosePool(int64_t pool_id);
    Message CreateConnection(int64_t pool_id);
    Message CloseConnection(int64_t pool_id, int64_t connection_id);
    Message Execute(int64_t pool_id, int64_t connection_id, GoBytes request_bytes);
    Message Metadata(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    Message Next(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    Message ResultSetStats(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    Message CloseRows(int64_t pool_id, int64_t connection_id, int64_t rows_id);
}

#endif //LIBRARY_H
