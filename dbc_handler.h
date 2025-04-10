#ifndef DBC_HANDLER_H
#define DBC_HANDLER_H

#include <string>
#include <vector>
#include <map>

class DBCHandler {
public:
    std::map<std::string, std::vector<std::pair<unsigned, std::string>>> valTables;

    friend void tag_invoke(
        can::def_val_table_cpo, DBCHandler& this_,
        std::string table_name, std::vector<std::pair<unsigned, std::string>> val_descs
    ) {
        this_.valTables[table_name] = val_descs;
    }
};

#endif // DBC_HANDLER_H