#include "rows.h"

namespace google::cloud::spanner::lib {
    google::spanner::v1::ResultSetMetadata* Rows::Metadata() {
        if (metadata_ != nullptr) {
            return metadata_.get();
        }
        if (!has_rows_) {
            metadata_ = std::make_unique<google::spanner::v1::ResultSetMetadata>();
            return metadata_.get();
        }
        if (before_first_) {
            pending_row_ = Next();
        }
        metadata_ = std::make_unique<google::spanner::v1::ResultSetMetadata>();
        const auto& status_or_row = *current_;
        if (!status_or_row.ok()) {
            // TODO: Return error
            return metadata_.get();
        }
        const auto& row = status_or_row.value();
        for (auto col : row.columns()) {
            const auto field = metadata_->mutable_row_type()->add_fields();
            field->set_name(col);
        }
        return metadata_.get();
    }

    protobuf::ListValue* Rows::Next() {
        if (!has_rows_) {
            return nullptr;
        }
        if (pending_row_ != nullptr) {
            const auto result = pending_row_;
            pending_row_ = nullptr;
            return result;
        }
        if (before_first_) {
            before_first_ = false;
            current_ = row_stream_.begin();
        } else {
            ++current_;
        }
        if (current_ == row_stream_.end()) {
            return nullptr;
        }
        const auto& status_or_row = *current_;
        if (!status_or_row.ok()) {
            // TODO: Return error
        }
        const auto& row = status_or_row.value();
        const auto list_value = new protobuf::ListValue();
        for (auto& value : row.values()) {
            const auto [_, proto_value] = spanner_internal::ToProto(value);
            *list_value->mutable_values()->Add() = proto_value;
        }
        return list_value;
    }

    google::spanner::v1::ResultSetStats* Rows::ResultSetStats() {
        if (stats_ != nullptr) {
            return stats_.get();
        }
        stats_ = std::make_unique<google::spanner::v1::ResultSetStats>();
        stats_->set_row_count_exact(rows_affected_);
        return stats_.get();
    }

    void Rows::Close() {
    }

}