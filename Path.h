#include <string.h>

#ifndef   _PATH_H
#define   _PATH_H
class Path {

    public:
        void resolve(char path[], char directory[], char filename[]);
        void resolve(char path[], char directory[]);
    private:
        int _debugLevel = 0;
        void resetPath(char path[]);
};

#endif // _PATH_H
