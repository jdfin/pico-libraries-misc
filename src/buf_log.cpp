#include "buf_log.h"

char BufLog::_log[BufLog::line_cnt][BufLog::line_len];
int BufLog::_put = 0;
int BufLog::_get = 0;
