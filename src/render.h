/* OpenGL 2.1 compatibility renderer: geometry, HDR scene, separable bloom. */
#define GLFN(type,name) static type p##name;
GLFN(PFNGLCREATESHADERPROC,glCreateShader)
GLFN(PFNGLSHADERSOURCEPROC,glShaderSource)
GLFN(PFNGLCOMPILESHADERPROC,glCompileShader)
GLFN(PFNGLGETSHADERIVPROC,glGetShaderiv)
GLFN(PFNGLGETSHADERINFOLOGPROC,glGetShaderInfoLog)
GLFN(PFNGLCREATEPROGRAMPROC,glCreateProgram)
GLFN(PFNGLATTACHSHADERPROC,glAttachShader)
GLFN(PFNGLLINKPROGRAMPROC,glLinkProgram)
GLFN(PFNGLGETPROGRAMIVPROC,glGetProgramiv)
GLFN(PFNGLGETPROGRAMINFOLOGPROC,glGetProgramInfoLog)
GLFN(PFNGLUSEPROGRAMPROC,glUseProgram)
GLFN(PFNGLGETUNIFORMLOCATIONPROC,glGetUniformLocation)
GLFN(PFNGLUNIFORM1FPROC,glUniform1f)
GLFN(PFNGLUNIFORM1IPROC,glUniform1i)
GLFN(PFNGLUNIFORM2FPROC,glUniform2f)
GLFN(PFNGLGENFRAMEBUFFERSPROC,glGenFramebuffers)
GLFN(PFNGLBINDFRAMEBUFFERPROC,glBindFramebuffer)
GLFN(PFNGLFRAMEBUFFERTEXTURE2DPROC,glFramebufferTexture2D)
GLFN(PFNGLGENRENDERBUFFERSPROC,glGenRenderbuffers)
GLFN(PFNGLBINDRENDERBUFFERPROC,glBindRenderbuffer)
GLFN(PFNGLRENDERBUFFERSTORAGEPROC,glRenderbufferStorage)
GLFN(PFNGLFRAMEBUFFERRENDERBUFFERPROC,glFramebufferRenderbuffer)
GLFN(PFNGLCHECKFRAMEBUFFERSTATUSPROC,glCheckFramebufferStatus)
GLFN(PFNGLACTIVETEXTUREPROC,glActiveTexture)
static unsigned sceneFB,bloomFB[2],sceneTex,bloomTex[2],depthRB,backgroundProgram,geometryProgram,pointProgram,blurProgram,postProgram;
static const char *vertScreen="#version 120\nvarying vec2 uv;void main(){gl_Position=gl_Vertex;uv=gl_Vertex.xy*.5+.5;}";
static const char *vertGeo="#version 120\nvarying vec4 col;varying float depth;void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;depth=-(gl_ModelViewMatrix*gl_Vertex).z;col=gl_Color;}";
static const char *vertPoint="#version 120\nvarying vec4 col;uniform float pointScale,sculpt;void main(){vec4 p=gl_ModelViewMatrix*gl_Vertex;gl_Position=gl_ProjectionMatrix*p;gl_PointSize=pointScale*mix(1.,clamp(24./max(-p.z,1.),.48,1.65),sculpt);col=gl_Color;}";
static const char *fragGeo="#version 120\nvarying vec4 col;varying float depth;uniform float haze;void main(){vec3 c=col.rgb*mix(vec3(1),vec3(.72,.88,1)*exp(-max(depth-28.,0.)*.019),haze);gl_FragColor=vec4(c,col.a);}";
static const char *fragPoint="#version 120\nvarying vec4 col;void main(){vec2 q=gl_PointCoord*2.-1.;float r=dot(q,q);if(r>1.)discard;gl_FragColor=vec4(col.rgb*(.42+exp(-r*12.)),col.a*(1.-r));}";
static const char *fragBG=
"#version 120\n"
"varying vec2 uv;uniform float time,act,pulse;uniform vec2 res;"
"float h(vec3 p){p=fract(p*.3183099+vec3(.1,.2,.3));p*=17.;return fract(p.x*p.y*p.z*(p.x+p.y+p.z));}"
"float n(vec3 p){vec3 i=floor(p),f=fract(p);f=f*f*(3.-2.*f);return mix(mix(mix(h(i),h(i+vec3(1,0,0)),f.x),mix(h(i+vec3(0,1,0)),h(i+vec3(1,1,0)),f.x),f.y),mix(mix(h(i+vec3(0,0,1)),h(i+vec3(1,0,1)),f.x),mix(h(i+vec3(0,1,1)),h(i+vec3(1,1,1)),f.x),f.y),f.z);}"
"float fb(vec3 p){float a=.5,s=0.;for(int i=0;i<5;i++){s+=a*n(p);p=p*2.03+vec3(4,1,7);a*=.5;}return s;}"
"void main(){vec2 p=(uv-.5)*vec2(res.x/res.y,1.);float a=time*.009;mat2 m=mat2(cos(a),-sin(a),sin(a),cos(a));p=m*p;"
"float f=fb(vec3(p*3.1,time*.008));float g=fb(vec3(p*5.+f*2.,time*.012+8.));"
"float band=exp(-pow((p.y+.23*sin(p.x*2.8)+.12)*3.5,2.));"
"vec3 c=vec3(.002,.006,.018)+vec3(.023,.095,.19)*pow(f,2.)*band;"
"c+=mix(vec3(.16,.025,.10),vec3(.025,.065,.095),.5+.5*sin(time*.024))*pow(g,3.)*band;"
"c+=vec3(.026,.043,.08)*exp(-length(p-vec2(.15,.04))*2.5)*(1.+pulse*.2);"
"gl_FragColor=vec4(c,1);}";
static const char *fragBlur=
"#version 120\nvarying vec2 uv;uniform sampler2D tex;uniform vec2 direction;uniform float threshold;"
"vec3 sampleAt(vec2 p){vec3 c=texture2D(tex,p).rgb;return max(vec3(0),c-threshold);}"
"void main(){vec3 c=sampleAt(uv)*.227027;c+=(sampleAt(uv+direction*1.384615)+sampleAt(uv-direction*1.384615))*.316216;c+=(sampleAt(uv+direction*3.230769)+sampleAt(uv-direction*3.230769))*.07027;gl_FragColor=vec4(c,1);}";
static const char *fragPost=
"#version 120\nvarying vec2 uv;uniform sampler2D tex,bloom;uniform float time,fade,pulse;uniform vec2 res;"
"void main(){vec2 d=(uv-.5)*.0008;vec3 c=texture2D(tex,uv).rgb;"
"c.r=texture2D(tex,uv+d).r;c.b=texture2D(tex,uv-d).b;"
"c+=texture2D(bloom,uv).rgb*.96;"
"c=1.-exp(-c*1.35);c=pow(c,vec3(.92));"
"float vi=clamp(1.-dot(uv-.5,uv-.5)*.72,0.,1.);"
"float grain=fract(sin(dot(uv*res+time,vec2(12.9898,78.233)))*43758.5453)-.5;"
"c=(c*vi+grain*.0025)*fade;gl_FragColor=vec4(c,1);}";
static void *getGL(const char *name){
#ifdef _WIN32
    void *p=(void*)wglGetProcAddress(name);
    if((uintptr_t)p<=3||p==(void*)(intptr_t)-1)return 0;
    return p;
#elif defined(ASTRA_SDL)
    return SDL_GL_GetProcAddress(name);
#else
    return (void*)OSMesaGetProcAddress(name);
#endif
}
static void *getFramebufferGL(const char *name){
#if defined(__APPLE__) && defined(ASTRA_SDL)
    /* Apple's legacy context exposes framebuffer objects through EXT.
       It preserves the GLSL 1.20 and immediate-mode renderer used by Astra. */
    char ext[80];snprintf(ext,sizeof(ext),"%sEXT",name);
    return getGL(ext);
#else
    return getGL(name);
#endif
}
static int extensions(void){
#ifdef _WIN32
#define LOAD(name,type) p##name=(type)getGL(#name);if(!p##name){return 0;}
#else
#define LOAD(name,type) p##name=(type)getGL(#name);if(!p##name)return 0;
#endif
    LOAD(glCreateShader,PFNGLCREATESHADERPROC) LOAD(glShaderSource,PFNGLSHADERSOURCEPROC)
    LOAD(glCompileShader,PFNGLCOMPILESHADERPROC) LOAD(glGetShaderiv,PFNGLGETSHADERIVPROC)
    LOAD(glGetShaderInfoLog,PFNGLGETSHADERINFOLOGPROC) LOAD(glCreateProgram,PFNGLCREATEPROGRAMPROC)
    LOAD(glAttachShader,PFNGLATTACHSHADERPROC) LOAD(glLinkProgram,PFNGLLINKPROGRAMPROC)
    LOAD(glGetProgramiv,PFNGLGETPROGRAMIVPROC) LOAD(glGetProgramInfoLog,PFNGLGETPROGRAMINFOLOGPROC)
    LOAD(glUseProgram,PFNGLUSEPROGRAMPROC) LOAD(glGetUniformLocation,PFNGLGETUNIFORMLOCATIONPROC)
    LOAD(glUniform1f,PFNGLUNIFORM1FPROC) LOAD(glUniform1i,PFNGLUNIFORM1IPROC) LOAD(glUniform2f,PFNGLUNIFORM2FPROC)
    LOAD(glActiveTexture,PFNGLACTIVETEXTUREPROC)
#undef LOAD
#define LOAD(name,type) p##name=(type)getFramebufferGL(#name);if(!p##name)return 0;
    LOAD(glGenFramebuffers,PFNGLGENFRAMEBUFFERSPROC) LOAD(glBindFramebuffer,PFNGLBINDFRAMEBUFFERPROC)
    LOAD(glFramebufferTexture2D,PFNGLFRAMEBUFFERTEXTURE2DPROC) LOAD(glGenRenderbuffers,PFNGLGENRENDERBUFFERSPROC)
    LOAD(glBindRenderbuffer,PFNGLBINDRENDERBUFFERPROC) LOAD(glRenderbufferStorage,PFNGLRENDERBUFFERSTORAGEPROC)
    LOAD(glFramebufferRenderbuffer,PFNGLFRAMEBUFFERRENDERBUFFERPROC) LOAD(glCheckFramebufferStatus,PFNGLCHECKFRAMEBUFFERSTATUSPROC)
#undef LOAD
    return 1;
}
static unsigned shader(unsigned type,const char *src){
    unsigned s=pglCreateShader(type);pglShaderSource(s,1,&src,0);pglCompileShader(s);int ok=0;pglGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){
        char log[2048]={0};pglGetShaderInfoLog(s,2047,0,log);
        fprintf(stderr,"Shader: %s\n",log);
        return 0;
    }
    return s;
}
static unsigned program(const char *vs,const char *fs){
    unsigned a=shader(GL_VERTEX_SHADER,vs),b=shader(GL_FRAGMENT_SHADER,fs);
    if(!a||!b)return 0;
    unsigned p=pglCreateProgram();pglAttachShader(p,a);pglAttachShader(p,b);pglLinkProgram(p);
    int ok=0;pglGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){
        char log[2048]={0};pglGetProgramInfoLog(p,2047,0,log);
        fprintf(stderr,"Program: %s\n",log);
        return 0;
    }
    return p;
}
static void uf(unsigned p,const char *n,float f){pglUniform1f(pglGetUniformLocation(p,n),f);}
static void ui(unsigned p,const char *n,int i){pglUniform1i(pglGetUniformLocation(p,n),i);}
static void u2(unsigned p,const char *n,float x,float y){pglUniform2f(pglGetUniformLocation(p,n),x,y);}
static void quad(void){glBegin(GL_QUADS);glVertex2f(-1,-1);glVertex2f(1,-1);glVertex2f(1,1);glVertex2f(-1,1);glEnd();}
static void makeTarget(unsigned *fb,unsigned *tx,int w,int h){
    glGenTextures(1,tx);glBindTexture(GL_TEXTURE_2D,*tx);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,w,h,0,GL_RGBA,GL_FLOAT,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    pglGenFramebuffers(1,fb);pglBindFramebuffer(GL_FRAMEBUFFER,*fb);pglFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,*tx,0);
}
static int initRender(void){
    if(!extensions())return 0;
    geometryProgram=program(vertGeo,fragGeo);pointProgram=program(vertPoint,fragPoint);backgroundProgram=program(vertScreen,fragBG);
    blurProgram=program(vertScreen,fragBlur);postProgram=program(vertScreen,fragPost);
    if(!geometryProgram||!pointProgram||!backgroundProgram||!blurProgram||!postProgram)return 0;
    makeTarget(&sceneFB,&sceneTex,W,H);pglGenRenderbuffers(1,&depthRB);pglBindRenderbuffer(GL_RENDERBUFFER,depthRB);
    pglRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,W,H);pglFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRB);
    if(pglCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)return 0;
    for(int j=0;j<2;j++)makeTarget(&bloomFB[j],&bloomTex[j],W/2,H/2);
    glEnable(GL_LINE_SMOOTH);glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);glEnable(GL_POINT_SPRITE);
    return 1;
}
static void camera(V eye,V at,float roll,float fov){
    float aspect=W/(float)H,top=.1f*tan(fov*PI/360);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glFrustum(-top*aspect,top*aspect,-top,top,.1f,160);
    V f=norm(sub(at,eye)),r=norm(cross(f,v(ss(roll),cc(roll),0))),u=cross(r,f);
    float m[]={r.x,u.x,-f.x,0,r.y,u.y,-f.y,0,r.z,u.z,-f.z,0,0,0,0,1};
    glMatrixMode(GL_MODELVIEW);glLoadMatrixf(m);glTranslatef(-eye.x,-eye.y,-eye.z);
}
static void line(V a,V b){vertex(a);vertex(b);}
static void edge(float x,float y,float X,float Y){glVertex3f(x,y,0);glVertex3f(X,Y,0);}
static void ring(float x,float y,float z,float rad,int n){glBegin(GL_LINE_LOOP);for(int i=0;i<n;i++){float a=TAU*i/n;glVertex3f(x+rad*cc(a),y+rad*ss(a),z);}glEnd();}
static void rect(float x,float y,float w,float h){glBegin(GL_LINE_LOOP);glVertex3f(x,y,0);glVertex3f(x+w,y,0);glVertex3f(x+w,y+h,0);glVertex3f(x,y+h,0);glEnd();}
static void cube(V p,float size,V c,float light){
    // Unlit assembly voxels must not punch opaque holes into the star field.
    if(light<.018f||size<.005f)return;
    float x=p.x,y=p.y,z=p.z,s=size*.5f;
    static const signed char faces[6][12]={{-1,-1,1,1,-1,1,1,1,1,-1,1,1},{1,-1,-1,-1,-1,-1,-1,1,-1,1,1,-1},{-1,1,1,1,1,1,1,1,-1,-1,1,-1},{-1,-1,-1,1,-1,-1,1,-1,1,-1,-1,1},{1,-1,1,1,-1,-1,1,1,-1,1,1,1},{-1,-1,-1,-1,-1,1,-1,1,1,-1,1,-1}};
    static const float shade[]={.85f,.28f,1,.18f,.49f,.34f};
    glBegin(GL_QUADS);
    for(int k=0;k<6;k++){color(mul(c,shade[k]*light),1);for(int j=0;j<4;j++)glVertex3f(x+faces[k][j*3]*s,y+faces[k][j*3+1]*s,z+faces[k][j*3+2]*s);}
    glEnd();
}
static void transparent(void){glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE);glDepthMask(GL_FALSE);}
static void opaque(void){glDisable(GL_BLEND);glDepthMask(GL_TRUE);}
static void post(void){
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);pglUseProgram(blurProgram);ui(blurProgram,"tex",0);
    glViewport(0,0,W/2,H/2);
    for(int i=0;i<4;i++){
        pglBindFramebuffer(GL_FRAMEBUFFER,bloomFB[i%2]);glBindTexture(GL_TEXTURE_2D,i==0?sceneTex:bloomTex[(i+1)%2]);
        u2(blurProgram,"direction",i%2?0:2.2f/W,i%2?2.2f/H:0);uf(blurProgram,"threshold",i==0?.13f:0);quad();
    }
    pglBindFramebuffer(GL_FRAMEBUFFER,0);glViewport(0,0,DW,DH);pglUseProgram(postProgram);
    pglActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,sceneTex);ui(postProgram,"tex",0);
    pglActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,bloomTex[1]);ui(postProgram,"bloom",1);
    uf(postProgram,"time",T);uf(postProgram,"fade",smooth(0,1,T)*(1-smooth(140,144,T)));uf(postProgram,"pulse",kick);u2(postProgram,"res",W,H);quad();pglActiveTexture(GL_TEXTURE0);
}
