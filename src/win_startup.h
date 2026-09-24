/* A short, flushed startup log survives a failure before audio playback. */
#ifdef _WIN32
static HANDLE startupLog=INVALID_HANDLE_VALUE;
static void trace(const char *message){
    if(!message)message="(information unavailable)";
    if(startupLog!=INVALID_HANDLE_VALUE){
        DWORD written;
        WriteFile(startupLog,message,(DWORD)strlen(message),&written,0);
        WriteFile(startupLog,"\r\n",2,&written,0);
        FlushFileBuffers(startupLog);
    }
}
static void startLog(void){
    char path[MAX_PATH];
    DWORD n=GetModuleFileNameA(0,path,MAX_PATH);
    if(n&&n<MAX_PATH-5){
        lstrcatA(path,".log");
        startupLog=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    }
    if(startupLog==INVALID_HANDLE_VALUE){
        n=GetTempPathA(MAX_PATH,path);
        if(n&&n<MAX_PATH-24){
            lstrcatA(path,"Astra-startup.log");
            startupLog=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
        }
    }
    trace("ASTRA v" ASTRA_VERSION " / Windows x64 / startup");
}
static void failStartup(const char *message,UINT code){
    char detail[64];
    wsprintfA(detail,"Win32 last error: %lu",GetLastError());
    trace(message);trace(detail);
    MessageBoxA(0,message,"Astra v" ASTRA_VERSION " - startup error",MB_OK|MB_ICONERROR);
    ExitProcess(code);
}
#endif
