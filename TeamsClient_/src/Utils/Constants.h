#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "../../includes.h"

namespace Constants {
    // Server configuration
    const QString SERVER_HOST = "http://127.0.0.1";
    const int SERVER_PORT = 8080;
    const QString BASE_URL = SERVER_HOST + ":" + QString::number(SERVER_PORT);
    
    // Application info
    const QString APP_NAME = "ChatApp";
    const QString APP_VERSION = "1.0.0";
    
    // UI Constants
    const int WINDOW_MIN_WIDTH = 800;
    const int WINDOW_MIN_HEIGHT = 600;
    const int WINDOW_DEFAULT_WIDTH = 1024;
    const int WINDOW_DEFAULT_HEIGHT = 768;
}

#endif