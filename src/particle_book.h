/* An open codex made only of points, gathered from the surrounding stars.
   Page coordinates also carry the quotation, so the lettering really folds. */
static V bookPage(float x,float y,float layer){
    float u=fabs(x)/4.25f;
    float opening=1-smooth(21+layer*.12f,24.5f+layer*.12f,T);
    float fold=opening*1.35f+.13f+layer*.045f;
    float curl=(.52f+layer*.035f)*ss(u*PI)-.16f*exp(-u*18);
    return v(x*cc(fold),y+.13f*ss(u*PI)-layer*.035f,
             fabs(x)*ss(fold)+curl-layer*.115f+.018f*ss(y*1.4f+T*.65f)*u);
}
static void bookDot(float x,float y,float layer,int id,float amount){
    V p=bookPage(x,y,layer);
    V dust=mul(stars[id%16000],.32f);dust.z-=4;
    V q=lerp(dust,p,amount);
    float flow=smooth(32.4f,36,T)*(1-smooth(35.2f,36,T));
    q=add(q,v(flow*1.2f*ss(id*.017f+T),flow*(.6f+.7f*ss(id*.012f)),0));
    vertex(q);
}
static void particleBook(void){
    if(T<20||T>=36)return;
    float enter=smooth(20,21.4f,T),leave=smooth(32.4f,36,T),a=enter*(1-leave);
    float gathered=enter*(1-leave*leave);
    glPushMatrix();glLoadIdentity();glTranslatef(.10f*ss(T*.4f),-.25f,-10.4f);
    glRotatef(-12+1.5f*ss(T*.15f),1,0,0);glRotatef(4+2*ss(T*.13f),0,1,0);
    glRotatef(1.5f*ss(T*.17f),0,0,1);
    transparent();pglUseProgram(geometryProgram);
    // A very dark, translucent page veil keeps distant engravings out of the ink.
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);color(v(.002f,.008f,.020f),a*(1-leave)*.86f);
    glBegin(GL_QUADS);
    for(int i=0;i<48;i++){
        float x=-4.25f+i*8.5f/48,xx=x+8.5f/48;
        vertex(bookPage(x,-2.35f,0));vertex(bookPage(xx,-2.35f,0));
        vertex(bookPage(xx,2.35f,0));vertex(bookPage(x,2.35f,0));
    }
    glEnd();glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    pglUseProgram(pointProgram);glPointSize(2.3f*H/720.0f);
    glBegin(GL_POINTS);
    // Dense, quiet paper and bright cover edges. The shadow leaves fan open.
    for(int layer=3;layer>=0;layer--)for(int iy=0;iy<80;iy++)for(int ix=0;ix<140;ix++){
        int border=iy<2||iy>77||ix<2||ix>137;
        if(layer&&!border&&iy%26!=0)continue;
        int id=iy*140+ix;
        float x=(ix/139.0f*2-1)*4.25f,y=(iy/79.0f*2-1)*2.35f;
        if(!border){x+=(hashf(id)-.5f)*.021f;y+=(hashf(id+43)-.5f)*.021f;}
        float spine=exp(-fabs(x)*15),brightness=border?1.20f: .16f+.08f*hashf(id+19)+spine*.50f;
        if(layer&&!border)brightness*=.42f;
        V c=border?lerp(gold,pink,layer*.16f):lerp(cyan,v(.25f,.33f,.85f),.25f+.25f*ss(x*.7f));
        color(c,a*brightness*(1-layer*.19f));bookDot(x,y,layer,id,gathered);
    }
    glEnd();
    // The vector font is sampled into luminous particles, not overlaid text.
    static const char *quote[]={"Curiosity has","its own reason","for existence.","Einstein"};
    float ink=smooth(23.4f,25.2f,T)*(1-smooth(31.8f,33.6f,T));
    for(int row=0;row<4;row++){
        int signature=row==3;
        const char *text=quote[row];float size=signature?.036f:.058f;
        float start=signature?1.35f:-(float)(strlen(text)*8-2)*size*.5f;
        glPointSize((signature?2.4f:3.0f)*H/720.0f);glBegin(GL_POINTS);
        for(int col=0;text[col];col++){
            int index=glyphIndex(text[col]);if(index<0)continue;
            const char *s=alphabet[index];int first=1;float px=0,py=0;
            while(*s){
                if(*s==' '){first=1;s++;continue;}
                float x=s[0]-'0',y=s[1]-'0';
                if(!first)for(int j=0;j<13;j++){
                    float q=j/12.0f,gx=mix(px,x,q),gy=mix(py,y,q);
                    if(signature&&col){gx*=.86f;gy*=.76f;}
                    float xx=start+(col*8+gx+(signature?.22f*gy:0))*size;
                    float yy=(signature?-1.44f:.72f-row*.87f)+gy*size;
                    int id=row*5000+col*91+j;
                    float opacity=ink*(signature?smooth(24.6f+col*.08f,25.3f+col*.08f,T):1);
                    color(lerp(gold,white,signature?.17f:.42f+.22f*ss(col*.4f+T*.6f)),opacity*(1.05f+.15f*hashf(id)));
                    V p=bookPage(xx,yy,0);p.z+=.05f;
                    p=add(p,mul(v(ss(id),cc(id*3),ss(id*7)),(1-ink)*1.7f));vertex(p);
                }
                first=0;px=x;py=y;s+=2;
            }
        }
        glEnd();
    }
    // A recessed binding and the visible thickness under the outer page edges.
    glPointSize(2.2f*H/720.0f);glBegin(GL_POINTS);
    for(int j=0;j<170;j++){
        float y=-2.35f+j*4.7f/169;
        color(gold,a*.23f);bookDot(-.045f,y,0,13000+j,gathered);bookDot(.045f,y,0,13200+j,gathered);
    }
    glEnd();
    // Gold thread through the binding and delicate page-head ornaments.
    glPointSize(3.0f*H/720.0f);glBegin(GL_POINTS);
    for(int side=-1;side<=1;side+=2)for(int i=0;i<160;i++){
        float u=i/159.0f;
        color(lerp(cyan,gold,u),a*.65f);
        bookDot(side*(.3f+3.5f*u),1.7f+.10f*ss(u*PI),0,12000+i,gathered);
        bookDot(side*(.3f+3.5f*u),-1.55f-.10f*ss(u*PI),0,12500+i,gathered);
    }
    glEnd();pglUseProgram(geometryProgram);glPopMatrix();opaque();
}
