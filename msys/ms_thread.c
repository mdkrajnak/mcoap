#ifdef WIN32
#include "msys/win/ms_thread_win.c"
#else
#include "msys/posix/ms_thread_posix.c"
#endif
