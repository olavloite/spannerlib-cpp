#ifndef ROWS_H
#define ROWS_H
#include <cstdint>
#include <google/cloud/spanner/connection.h>
#include <google/cloud/spanner/results.h>


namespace google::cloud::spanner::lib {

class Rows {
public:
    explicit Rows(const int64_t id, RowStream row_stream) :
        id_(id),
        has_rows_(true),
        metadata_(nullptr),
        pending_row_(nullptr),
        row_stream_(std::move(row_stream)),
        before_first_(true), rows_affected_(0) {
    }

    explicit Rows(const int64_t id, const int64_t rows_affected) :
        id_(id),
        has_rows_(false),
        metadata_(nullptr),
        pending_row_(nullptr),
        before_first_(true),
        rows_affected_(rows_affected) {
    }
/// No default construction.
    Rows() = delete;

    google::spanner::v1::ResultSetMetadata* Metadata();
    protobuf::ListValue* Next();
    google::spanner::v1::ResultSetStats* ResultSetStats();
    void Close();

    const int64_t id_;
private:
    bool has_rows_;
    std::unique_ptr<google::spanner::v1::ResultSetMetadata> metadata_;
    std::unique_ptr<google::spanner::v1::ResultSetStats> stats_;
    protobuf::ListValue* pending_row_;
    RowStream row_stream_;
    bool before_first_;
    RowStreamIterator current_;
    int64_t rows_affected_;
};

}

#endif //ROWS_H
