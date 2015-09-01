#ifdef WIN32
#include "msys/win/ms_mutex_win.c"
#else
#include "msys/posix/ms_mutex_posix.c"
#endif
