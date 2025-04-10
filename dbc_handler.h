#ifndef DBC_HANDLER_H
#define DBC_HANDLER_H
#pragma once


#include <string>
#include <vector>
#include <map>
#include <utility>
#include "libs/can-utils/dbc/dbc_parser.h"

struct DBCHandler {
    std::map<std::string, std::vector<std::pair<unsigned, std::string>>> valTables;
    std::map<uint32_t, std::string> messages;  // ID → name

};

namespace can {
    void tag_invoke(
        def_val_table_cpo, DBCHandler &this_,
        std::string table_name, std::vector<std::pair<unsigned, std::string>> val_descs
    ) {
        this_.valTables[table_name] = val_descs;
    }

    void tag_invoke(
        def_bo_cpo, DBCHandler &this_,
        uint32_t msg_id, std::string msg_name,
        size_t msg_size, size_t /* transmitter_ord */
    ) {
        this_.messages[msg_id] = msg_name;
    }
}

#endif // DBC_HANDLER_H