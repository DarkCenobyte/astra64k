/* Batch stills use the exact shared renderer and compose audio only once. */
#define main astra_renderer_main
#include "../src/astra.c"
#undef main
int main(int argc,char **argv){
    if(argc<3)return 1;
    W=1280;H=720;DW=W;DH=H;
    unsigned char *buffer=calloc(W*H,4);
    OSMesaContext ctx=OSMesaCreateContextExt(OSMESA_RGBA,24,0,0,0);
    if(!ctx||!OSMesaMakeCurrent(ctx,buffer,GL_UNSIGNED_BYTE,W,H)||!initRender())return 2;
    initArt();compose();if(!pcm)return 3;
    for(int i=2;i<argc;i++){
        float t=atof(argv[i]);char path[512];snprintf(path,sizeof(path),"%s-%.2f.ppm",argv[1],t);
        frame(t);glFinish();if(glGetError()!=GL_NO_ERROR)return 4;
        writeImage(path);fprintf(stderr,"%s\n",path);
    }
    OSMesaDestroyContext(ctx);free(buffer);free(pcm);free(vocal);return 0;
}
