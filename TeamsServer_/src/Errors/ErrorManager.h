#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include "../includes.h"

class ErrorManager {

    public : 
    void handle_error(std::string_view where, const boost::system::error_code& ec);

};

#endif