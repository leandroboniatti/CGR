#include "App.h"

int main() {
    App app;
    if(!app.init(1280,720,"OBJ Viewer")) return -1;
    app.run();
    app.shutdown();
    return 0;
}
