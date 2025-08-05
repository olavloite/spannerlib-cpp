#ifndef SPANNERLIB_LIBRARY_H
#define SPANNERLIB_LIBRARY_H

#if defined(_WIN32) && defined(BUILDING_DLL)
#    ifdef BUILDING_DLL
//       Compiling a Windows DLL
#        define DLL_EXPORT __declspec(dllexport)
#    else
//       Using a Windows DLL
#        define DLL_EXPORT __declspec(dllimport)
#    endif
// Windows or Linux static library, or Linux so
#else
#    define DLL_EXPORT
#endif
#include <string>

#include "message.h"

extern "C" {
    DLL_EXPORT void hello();

    DLL_EXPORT int32_t Release(int64_t pinner_id);
    DLL_EXPORT Message CreatePool(GoString dsn);
    DLL_EXPORT Message ClosePool(int64_t pool_id);
    DLL_EXPORT Message CreateConnection(int64_t pool_id);
    DLL_EXPORT Message CloseConnection(int64_t pool_id, int64_t connection_id);
    DLL_EXPORT Message Execute(int64_t pool_id, int64_t connection_id, GoBytes request_bytes);
    DLL_EXPORT Message Metadata(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    DLL_EXPORT Message Next(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    DLL_EXPORT Message ResultSetStats(int64_t pool_id, int64_t connection_id, int64_t rows_id);
    DLL_EXPORT Message CloseRows(int64_t pool_id, int64_t connection_id, int64_t rows_id);
}

#endif //SPANNERLIB_LIBRARY_H