inline void bnLogInfo(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
    va_end(args);
}

inline void bnLogError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
    va_end(args);
}

inline void bnLogCritical(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, fmt, args);
    va_end(args);
}