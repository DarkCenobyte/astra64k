/* A short, flushed startup log survives a failure before audio playback. */
#ifdef _WIN32
static void failStartup(const char *message,UINT code){
    char detail[64];
    wsprintfA(detail,"Win32 last error: %lu",GetLastError());
    MessageBoxA(0,message,"Astra v" ASTRA_VERSION " - startup error",MB_OK|MB_ICONERROR);
    ExitProcess(code);
}
#endif
