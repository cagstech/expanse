#ifndef gamestate_h
#define gamestate_h
#include "conn.h"

struct _gamestate
{
    struct _conn conn;
    struct _client client;
};

#endif