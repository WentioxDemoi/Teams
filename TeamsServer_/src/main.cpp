#include "AppCompositionRoot.h"

int main() {
    try {
        AppCompositionRoot app(1, 1, 1, 1);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }
    return 0;
}