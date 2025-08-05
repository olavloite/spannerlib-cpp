#ifndef CONNECTION_H
#define CONNECTION_H
#include <cstdint>
#include <google/spanner/v1/spanner.pb.h>

#include "rows.h"


namespace google::cloud::spanner::lib {

class Pool;
class Connection {
public:
    explicit Connection(const int64_t id, const Pool* pool) : id_(id), pool_(pool) {
    }
    /// No default construction.
    Connection() = delete;

    void Close();
    Rows* Execute(const google::spanner::v1::ExecuteSqlRequest* request);
    Rows* GetRows(int64_t rows_id) const;
    const int64_t id_;
    const Pool* pool_;

private:
    std::atomic_int64_t rows_id_counter_;
    absl::flat_hash_map<int64_t, Rows*> rows_ = absl::flat_hash_map<int64_t, Rows*>();
    static SqlStatement::ParamType CreateParams(
        const protobuf::Struct& proto, const protobuf::Map<std::string, google::spanner::v1::Type>& param_types);
};

}

#endif //CONNECTION_H
