#ifndef   _PATH_H
#define   _PATH_H

#include <string.h>

class Path {

    public:
        int initialise();
        int initialise(int debugLevel);
        void resolve(char path[], char directory[], char filename[]);
        void resolve(char path[], char directory[]);
    private:
        int _debugLevel = 0;
        void resetPath(char path[]);
};

#endif // _PATH_H
