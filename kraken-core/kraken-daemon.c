#include "kraken-daemon.h"

int main(int argc, char** argv) {
    kraken_daemon_t* daemon = kraken_daemon_new();
    
    if (daemon) {
        kraken_daemon_run(daemon);
        kraken_daemon_free(daemon);
    }
    
    return 0;
}
