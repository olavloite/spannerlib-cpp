#include "connection.h"
#include "pool.h"

namespace google::cloud::spanner::lib {
    void Connection::Close() {

    }

    Rows* Connection::Execute(const google::spanner::v1::ExecuteSqlRequest* request) {
        auto sql = request->sql();
        auto index = 1;
        while (sql.find('?') != std::string::npos) {
            sql.replace(sql.find('?'), 1, "$" + std::to_string(index++));
        }
        const auto params = CreateParams(request->params(), request->param_types());
        const SqlStatement statement(sql, params);
        auto client = this->pool_->client_;
        auto id = ++rows_id_counter_;
        Rows* rows;
        const auto insert_keyword = "insert";
        const auto update_keyword = "update";
        const auto delete_keyword = "delete";
        const auto is_dml =
            absl::StrContainsIgnoreCase(sql.substr(0, std::strlen(insert_keyword)), insert_keyword)
            ||
            absl::StrContainsIgnoreCase(sql.substr(0, std::strlen(update_keyword)), update_keyword)
            ||
            absl::StrContainsIgnoreCase(sql.substr(0, std::strlen(delete_keyword)), delete_keyword);
        if (is_dml) {
            int64_t rows_affected;
            client.Commit([&client, &statement, &rows_affected]
                (Transaction txn) -> StatusOr<Mutations> {
                auto dml_result = client.ExecuteDml(std::move(txn), statement);
                if (!dml_result) {
                    return std::move(dml_result).status();
                }
                rows_affected = dml_result->RowsModified();
                return Mutations{};
            });
            rows = new Rows(id, rows_affected);
        } else {
            auto stream = client.ExecuteQuery(statement);
            rows = new Rows(id, std::move(stream));
        }
        this->rows_.insert({id, rows});
        return rows;
    }

    SqlStatement::ParamType Connection::CreateParams(const protobuf::Struct& proto, const protobuf::Map<std::string, google::spanner::v1::Type> & param_types) {
        SqlStatement::ParamType params;
        for (const auto&[name, proto_value]: proto.fields()) {
            if (auto const param_type = param_types.find(name); param_type != param_types.end()) {
                params.insert(std::pair(name, spanner_internal::FromProto(param_type->second, proto_value)));
                continue;
            }
            if (proto_value.has_bool_value()) {
                params.insert(std::pair(name, Value(proto_value.bool_value())));
            } else if (proto_value.has_string_value()) {
                params.insert(std::pair(name, Value(proto_value.string_value())));
            } else if (proto_value.has_number_value()) {
                params.insert(std::pair(name, Value(proto_value.number_value())));
            } else if (proto_value.has_null_value()) {
                params.insert(std::pair(name, MakeNullValue<std::string>()));
            } else {
                google::spanner::v1::Type type;
                type.set_code(google::spanner::v1::STRING);
                params.insert(std::pair(name, spanner_internal::FromProto(type, proto_value)));
            }
        }

        return params;
    }

    Rows* Connection::GetRows(const int64_t rows_id) const {
        if (rows_.contains(rows_id)) {
            return rows_.at(rows_id);
        }
        throw std::runtime_error(absl::StrCat("No rows with id: ", rows_id));
    }
}
