/* All forms below are authored geometry. No meshes, pictures or fonts are read. */
static V stars[16000],artEye,artAt;
static void initArt(void){for(int i=0;i<16000;i++){float z=hashf(i*5+1)*2-1,a=hashf(i*5+2)*TAU,r=48+hashf(i*5+3)*35;stars[i]=v(r*sqrt(1-z*z)*cc(a),r*z,r*sqrt(1-z*z)*ss(a));}}
static const char *alphabet[]={
 "003860 1353", "00081868765414 14646361501000", "6878180800005060", "0008185868606000", "6808000060 0444", "680800 0444", "687818080000606434", "0008 6068 0464", "08086868 3830 00006060", "0868 68611000", "0008 680360", "080060", "0008336860", "00086068", "1001071868586761605010", "00081868765404", "1001071868586761605010 4360", "00081868765404 3460", "67785818070514546361501001", "0868 3830", "080110506168", "083068", "0810234068", "0860 0068", "083468 3430", "08680060",
 "1001071868586761605010 1553", "183830 1050", "0718686765540060", "081858676454 34646361501000", "080464 6860", "680804545361501000", "6778180701105061635404", "086830", "1001071868586761605010 1454", "6067180807146460", "0464", "3031", "0206", "3031 3536"
};
static int glyphIndex(char c){if(c>='a'&&c<='z')c-=32;if(c>='A'&&c<='Z')return c-'A';if(c>='0'&&c<='9')return 26+c-'0';if(c=='-')return 36;if(c=='.')return 37;if(c=='/')return 38;if(c==':')return 39;return -1;}
static void glyph(char c){int idx=glyphIndex(c);if(idx<0)return;const char *s=alphabet[idx];int first=1;float px=0,py=0;glBegin(GL_LINES);while(*s){if(*s==' '){first=1;s++;continue;}float x=s[0]-'0',y=s[1]-'0';if(!first){glVertex2f(px,py);glVertex2f(x,y);}px=x;py=y;first=0;s+=2;}glEnd();}
static void label(const char *s,float x,float y,float height,float spacing,V col,float alpha){
    glPushMatrix();glTranslatef(x,y,0);glScalef(height/8,height/8,1);color(col,alpha);
    for(;*s;s++){glPushMatrix();if(*s>='a'&&*s<='z')glScalef(.82f,.76f,1);glyph(*s);glPopMatrix();glTranslatef(spacing,0,0);}glPopMatrix();
}
static void centered(const char*s,float y,float height,float spacing,V col,float alpha){label(s,800-(strlen(s)*spacing-3)*height/16,y,height,spacing,col,alpha);}
static void inscription(int seed,float x,float y,float size){
    glPushMatrix();glTranslatef(x,y,0);glScalef(size,size,1);
    int kind=(seed*7+3)%10;
    glBegin(GL_LINES);
    if(kind==0){edge(-.38f,0,.4f,0);edge(-.38f,0,0,.23f);edge(0,.23f,.4f,0);edge(-.38f,0,0,-.2f);edge(0,-.2f,.4f,0);edge(.12f,-.12f,.22f,-.4f);edge(.22f,-.4f,-.05f,-.4f);}
    if(kind==1){edge(0,.20f,0,-.5f);edge(-.28f,-.1f,.28f,-.1f);}
    if(kind==2){edge(-.3f,-.25f,.3f,-.25f);edge(-.3f,-.25f,-.45f,.05f);edge(-.45f,.05f,-.12f,.05f);edge(-.12f,.05f,.05f,.45f);edge(.05f,.45f,.25f,.42f);edge(.25f,.42f,.3f,-.25f);edge(.3f,.22f,.5f,.2f);edge(-.3f,-.25f,-.35f,-.45f);edge(.15f,-.25f,.1f,-.45f);}
    if(kind==3){edge(0,-.5f,0,.25f);for(int i=0;i<4;i++){float k=.28f-i*.13f;edge(0,k,-.3f,k+.16f);edge(0,k,.3f,k+.16f);}}
    if(kind==4){for(int i=0;i<4;i++){edge(-.35f+i*.16f,-.38f,-.2f+i*.16f,.35f);edge(-.2f+i*.16f,.35f,-.3f+i*.16f,.25f);}}
    if(kind==5){for(int i=0;i<3;i++){float k=i*.22f-.2f;edge(-.45f,k,-.2f,k+.12f);edge(-.2f,k+.12f,.05f,k);edge(.05f,k,.3f,k+.12f);}}
    if(kind==6){edge(-.25f,-.15f,0,.35f);edge(0,.35f,.25f,-.15f);edge(-.25f,-.15f,.25f,-.15f);edge(0,-.15f,0,-.45f);edge(-.3f,-.45f,.3f,-.45f);}
    if(kind==7){edge(-.4f,-.2f,.35f,-.2f);edge(-.4f,-.2f,-.15f,.1f);edge(-.15f,.1f,.15f,.1f);edge(.15f,.1f,.35f,-.2f);edge(-.15f,.1f,-.15f,.4f);edge(.15f,.1f,.15f,.4f);}
    if(kind>=8){for(int i=0;i<3;i++){float x=i*.24f-.3f;edge(x,-.4f,x,.35f);edge(x,.35f,x+.15f,.2f);edge(x,.2f,x+.15f,.2f);}}
    glEnd();if(kind==0)ring(0,0,0,.085f,8);if(kind==1)ring(0,.3f,0,.16f,12);if(kind==7)ring(0,.32f,0,.17f,12);
    glPopMatrix();
}
static void rosette(float x,float y,float size){
    ring(x,y,0,size*.25f,12);ring(x,y,0,size,32);
    glBegin(GL_LINES);for(int i=0;i<12;i++){float a=i*TAU/12,b=a+.16f;edge(x+cc(a)*size*.3f,y+ss(a)*size*.3f,x+cc(b)*size*.88f,y+ss(b)*size*.88f);edge(x+cc(b)*size*.88f,y+ss(b)*size*.88f,x+cc(a+.32f)*size*.3f,y+ss(a+.32f)*size*.3f);}glEnd();
}
static void animal(float x,float y,float s,int flip){
    static const float outline[]={-1.3f,.1f,-1.0f,.43f,-.6f,.52f,.45f,.52f,.72f,.76f,.78f,1.04f,1.00f,1.08f,1.05f,.87f,1.32f,.78f,1.37f,.6f,1.07f,.48f,.88f,.17f,.78f,-.22f,.91f,-.68f,.70f,-.69f,.51f,-.15f,-.46f,-.18f,-.75f,-.36f,-.81f,-.68f,-1.03f,-.68f,-.94f,-.12f,-1.3f,.1f};
    glPushMatrix();glTranslatef(x,y,0);glScalef(s*flip,s,1);
    glBegin(GL_LINE_STRIP);for(unsigned i=0;i<sizeof(outline)/sizeof(outline[0])/2;i++)glVertex2f(outline[i*2],outline[i*2+1]);glEnd();
    glBegin(GL_LINES);
    edge(.91f,1.02f,.65f,1.28f);edge(.65f,1.28f,.64f,1.46f);edge(.92f,1.02f,.98f,1.44f);edge(.98f,1.44f,1.10f,1.6f);
    edge(-1.2f,.2f,-1.52f,.48f);edge(-1.52f,.48f,-1.60f,.96f);edge(-1.60f,.96f,-1.43f,1.08f);
    edge(-.64f,-.15f,-.4f,-.57f);edge(-.4f,-.57f,-.14f,-.61f);edge(.4f,-.16f,.15f,-.61f);edge(.15f,-.61f,.36f,-.65f);
    for(int i=0;i<8;i++){float k=-.65f+i*.17f;edge(k,.38f,k+.09f,.28f);edge(k+.09f,.28f,k+.04f,.10f);}
    for(int i=0;i<6;i++){float k=i*.1f;edge(.5f+k*.32f,.64f-k,.75f+k*.32f,.62f-k);}
    edge(1.14f,.61f,1.4f,.5f);edge(1.4f,.5f,1.56f,.6f);glEnd();
    ring(.98f,.82f,0,.045f,6);rosette(-.69f,.04f,.14f);rosette(.55f,.15f,.12f);glPopMatrix();
}
static void panel(float width,float height,int seed,float alpha){
    color(cyan,alpha*.65f);rect(-width/2,-height/2,width,height);rect(-width/2+.12f,-height/2+.12f,width-.24f,height-.24f);
    for(int row=0;row<(int)(height/.62f)-1;row++)for(int col=0;col<(int)(width/.55f)-1;col++){
        color((row+col)%6==0?gold:cyan,alpha*(.32f+.38f*hashf(row*71+col+seed)));
        inscription(seed+row*11+col*3,-width/2+.45f+col*.55f,-height/2+.5f+row*.62f,.47f);
    }
    color(gold,alpha*.7f);for(int j=0;j<5;j++)rosette(-width/2+.4f+j*(width-.8f)/4,height/2-.28f,.14f);
}
static void gate(float alpha){
    transparent();glLineWidth(1.0f);
    for(int side=-1;side<=1;side+=2){
        float x=side*4.5f;
        color(cyan,alpha*.6f);rect(x-1.65f,-5,3.3f,11.4f);rect(x-1.51f,-4.85f,3.02f,11.1f);
        glBegin(GL_LINES);
        for(int r=0;r<26;r++){float y=-5+r*.43f;edge(x-1.65f,y,x+1.65f,y);for(int c=0;c<5;c++){float xx=x-1.65f+c*.73f+(r%2)*.365f;if(xx<x+1.65f)edge(xx,y,xx,y+.43f);}}
        for(int j=0;j<4;j++){float xx=x-1.65f+j*.9f;edge(xx,6.4f,xx,6.92f);edge(xx,6.92f,xx+.46f,6.92f);edge(xx+.46f,6.92f,xx+.46f,6.4f);}
        glEnd();
        for(int row=0;row<4;row++){
            glPushMatrix();glTranslatef(0,0,.018f);color(gold,alpha*(.71f+.31f*exp(-fract(T*.5f-row*.25f)*7)));animal(x,-3.7f+row*2.3f,.79f,side);glPopMatrix();
            color(cyan,alpha*.5f);for(int k=0;k<5;k++)rosette(x-1.22f+k*.60f,-4.65f+row*2.3f,.13f);
        }
        glPushMatrix();glTranslatef(side*7.8f,.8f,-1.5f);glRotatef(-side*28,0,1,0);panel(2.4f,9.4f,side+13,alpha*.8f);glPopMatrix();
    }
    color(gold,alpha*.85f);
    for(int q=0;q<4;q++){
        float r=2.30f+q*.15f;
        glBegin(GL_LINE_STRIP);glVertex3f(-r,-5,0);glVertex3f(-r,.7f,0);
        for(int j=0;j<=40;j++){float a=PI-j*PI/40;glVertex3f(r*cc(a),.7f+r*ss(a),0);}glVertex3f(r,-5,0);glEnd();
    }
    color(cyan,alpha*.56f);rect(-2.85f,3.65f,5.7f,2.35f);
    for(int row=0;row<3;row++)for(int col=0;col<10;col++)inscription(row*17+col,-2.5f+col*.55f,4.03f+row*.61f,.48f);
    for(int i=0;i<28;i++){float a=(i+.5f)*PI/28;glPushMatrix();glTranslatef(2.92f*cc(a),.7f+2.92f*ss(a),0);glRotatef(a*180/PI-90,0,0,1);inscription(i,0,0,.29f);glPopMatrix();}
    opaque();
}
static void floorGrid(float alpha){
    transparent();color(cyan,alpha);glBegin(GL_LINES);
    for(int i=-16;i<=16;i++)line(v(i*1.4f,-5,-70),v(i*1.4f,-5,15));
    for(int i=0;i<45;i++){float z=15-i*1.9f+fract(T*.5f)*1.9f;line(v(-24,-5,z),v(24,-5,z));}glEnd();opaque();
}
static void memoryScene(float a){
    float cameraZ=18-(T-16)*2.4f;
    for(int j=4;j>=0;j--){
        float distance=cameraZ+j*14;if(distance< -3)continue;
        glPushMatrix();glTranslatef(0,0,-j*14);gate(a*(.30f+.70f*(1-smooth(14,45,distance))));glPopMatrix();
    }
    floorGrid(a*.13f);
    transparent();color(gold,a*.28f);glBegin(GL_LINES);
    for(int i=0;i<80;i++){float x=(hashf(i*3)-.5f)*22,z=12-fract(T*.07f+hashf(i*3+1))*65;line(v(x,-4.8f,z),v(x,-4.8f,z-1.5f));}glEnd();opaque();
}
#include "particle_book.h"
#include "portrait.h"
#include "celebration.h"
#include "pavilion.h"
#include "sky_tracers.h"
#include "kirchhoff.h"
static V dragonPath(float u){
    float neck=exp(-u*11),tail=smooth(.58f,1,u),phase=u*TAU*1.32f-T*.55f;
    return v((u-.5f)*21.2f+.48f*ss(u*TAU*2-T*.43f)*tail,
             3.35f*ss(phase)+.55f*ss(u*TAU*2.8f-T*.39f)+.72f*neck*ss(T*.72f)+.52f*tail*ss(u*14-T*1.1f),
             3.6f*cc(u*TAU*1.06f-T*.29f)+.55f*neck*ss(T*.65f)+.70f*tail*ss(u*13-T*.82f));
}
static void orient(V p,V f){V r=norm(cross(v(0,1,0),f)),u=cross(f,r);float m[]={r.x,r.y,r.z,0,u.x,u.y,u.z,0,f.x,f.y,f.z,0,p.x,p.y,p.z,1};glMultMatrixf(m);}
static void horn(V root,int side){
    for(int i=0;i<12;i++){float t=i/11.0f;V p=add(root,v(side*(.10f+.6f*t+.25f*ss(t*4)),t*1.95f,-t*.6f));cube(p,.21f*(1-t*.65f),gold,1.2f);if(i>4&&i<9)cube(add(p,v(side*.23f,(i-4)*.1f,-.14f)),.15f,gold,.9f);}
}
static void dragonScene(void){
    opaque();
    for(int i=112;i>=0;i--){
        float u=i/112.0f;V p=dragonPath(u),f=norm(sub(dragonPath(u),dragonPath(u+.005f)));
        float rad=(.43f+.27f*ss(u*PI))*(1-smooth(.58f,1,u)*.94f);
        float travelling=exp(-fract(beat-u*4.2f)*10),pulse=1+.12f*travelling;
        float reveal=smooth(64+u*.6f,66.5f+u*.6f,T)*(1-smooth(93.6f-u*.45f,96,T));
        glPushMatrix();orient(p,f);glRotatef(12*ss(u*8-T*.72f),0,0,1);
        for(int j=0;j<9;j++){
            float a=j*TAU/9+.32f*ss(u*11-T*.43f);V q=v(cc(a)*rad*pulse,ss(a)*rad*pulse,0);
            V c=j<4?lerp(cyan,v(.05f,.28f,.47f),.5f):lerp(gold,v(.38f,.17f,.02f),.4f);
            cube(q,(.27f-.20f*smooth(.55f,1,u))*pulse*reveal,c,reveal*(.84f+.40f*travelling+.09f*kick)*(j==2?1.3f:1));
        }
        if(i%2==0)cube(v(0,rad+.13f*(1-u),0),.20f*(1-u*.85f)*reveal,lerp(gold,white,travelling*.35f),reveal*(1.1f+travelling*1.2f));
        if(i==26||i==57)for(int side=-1;side<=1;side+=2){
            for(int k=0;k<7;k++){V q=v(side*(.45f+k*.13f),-.1f-k*.14f,.12f*ss(T*1.2f+u*12)*k);cube(q,.19f*reveal,cyan,.75f*reveal);if(k==6)for(int n=-1;n<=1;n++)cube(add(q,v(0,-.2f,n*.18f)),.14f*reveal,gold,1.4f*reveal);}
        }
        glPopMatrix();
    }
    float head=smooth(64,66.5f,T)*(1-smooth(93.6f,96,T));
    glPushMatrix();V hp=dragonPath(0),hf=norm(sub(hp,dragonPath(.008f)));orient(hp,hf);
    glRotatef(5*ss(T*.72f),0,0,1);glScalef(1.45f*head,1.45f*head,1.45f*head);
    // Broad brow, projecting muzzle, antlers, fangs and long filament whiskers.
    for(int z=0;z<8;z++)for(int x=-3;x<=3;x++)for(int y=-2;y<=3;y++){
        if(abs(x)<3&&y>-2&&y<3&&z>0&&z<7)continue;
        float narrow=z>3?.73f:1;cube(v(x*.18f*narrow,y*.17f,z*.17f),.165f,y>=2?cyan:gold,y>=2?1.2f:.55f);
    }
    for(int side=-1;side<=1;side+=2){
        horn(v(side*.47f,.5f,.13f),side);
        cube(v(side*.57f,.32f,.72f),.23f,v(1,.28f,.08f),3);
        for(int i=0;i<4;i++)cube(v(side*.31f,-.43f-i*.045f,.8f+i*.09f),.09f,white,1.2f);
    }
    transparent();color(gold,.9f);glLineWidth(1.3f);
    for(int side=-1;side<=1;side+=2)for(int n=0;n<2;n++){glBegin(GL_LINE_STRIP);for(int i=0;i<30;i++){float k=i/29.0f;glVertex3f(side*(.26f+2.3f*k),-.17f+.45f*ss(k*5+T*2+n),1.25f-k*.8f+n*.16f);}glEnd();}
    glPopMatrix();
    // Expanding beat rings weave through the creature, behind its body.
    glPushMatrix();glRotatef(70,1,0,0);for(int i=0;i<5;i++){float r=3+fract(T*.15f+i*.2f)*11;color(i%2?cyan:gold,.11f*head*(1-(r-3)/11));ring(0,0,-4,r,96);}glPopMatrix();opaque();
}
static V lotus(int i,float time){
    float u=hashf(i*7+2),q=hashf(i*7+3);int petal=i%12;float ang=petal*TAU/12+(q-.5f)*sin(u*PI)*.83f;
    int layer=(i/12)%3;float open=smooth(96,110,time),r=(.4f+u*(7.8f-layer*1.5f))*(.5f+.5f*open)*(1+.017f*kick);
    float y=-2.5f+u*u*(4.9f-layer*.2f)+sin(u*PI)*(.7f+.7f*ss(q*PI))+(1-open)*u*5+layer*.55f;
    y+=.12f*ss(T*1.8f+petal)*u;V a=v(r*cc(ang),y,r*ss(ang));
    float z=1-2*(i+.5f)/14000.0f,theta=i*2.39996323f+time*.15f;
    float rr=4.65f+.09f*ss(theta*8+T*2);V b=v(rr*sqrt(1-z*z)*cc(theta),rr*z+.3f,rr*sqrt(1-z*z)*ss(theta));
    float morph=smooth(113+layer*.18f,119,time),twist=.38f*ss(morph*PI);
    V p=lerp(a,b,morph);
    return v(p.x*cc(twist)-p.z*ss(twist),p.y+.35f*ss(morph*PI),p.x*ss(twist)+p.z*cc(twist));
}
static void starsScene(int phase){
    transparent();pglUseProgram(pointProgram);glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    float assemble=smooth(94,102,T),out=smooth(126,133,T);
    uf(pointProgram,"pointScale",H/720.0f*2.4f);uf(pointProgram,"sculpt",assemble*(1-out));
    V forward=norm(sub(artEye,artAt));
    static const float joins[]={40,64,96,136};
    float flow=0,progress=0;int join=0;
    for(int j=0;j<4;j++)if(T>joins[j]-2.5f&&T<joins[j]+4){
        join=j;progress=smooth(joins[j]-2,joins[j]+2,T);
        flow=smooth(joins[j]-2.5f,joins[j]-.5f,T)*(1-smooth(joins[j]+1,joins[j]+4,T));
    }
    glBegin(GL_POINTS);
    for(int i=0;i<16000;i++){
        V p=stars[i],c=lerp(v(.13f,.27f,.55f),white,hashf(i+82));float a=.2f+.45f*hashf(i+47);
        if(phase==0&&i<5000){
            float u=i/5000.0f,th=i*2.399963f+T*.2f;
            float r=2.2f*sqrt(u)*(1+.06f*kick);V q=v(r*cc(th),(.5f-u)*5,r*ss(th));
            float gather=smooth(.8f,6,T)*(1-smooth(12,16,T));
            q=add(q,v(.35f*ss(T*.4f+u*9),0,0));p=lerp(p,q,gather);c=lerp(c,lerp(cyan,gold,u),gather);a=mix(a,.7f,gather);
        }
        if((phase>=4||T>94)&&i<14000){
            float shape=assemble*(1-out),u=hashf(i*7+2),across=hashf(i*7+3),morph=smooth(113,119,T);
            V q=lotus(i,T);p=lerp(p,q,shape);
            float turn=.28f*ss(out*PI)*assemble,x=p.x;
            p.x=x*cc(turn)-p.z*ss(turn);p.z=x*ss(turn)+p.z*cc(turn);
            V hue=lerp(cyan,gold,u*u);if(i%12==1||i%12==2)hue=lerp(hue,pink,.58f);
            float proximity=clamp(.5f+dot(q,forward)/14,0,1);
            float edge=pow(fabs(across*2-1),9)*.62f+pow(u,10)*.75f;
            float surface=dot(norm(sub(q,v(0,.3f,0))),forward);
            float sphere=.16f+.84f*smooth(-.65f,.70f,surface)+.46f*pow(1-fabs(surface),4);
            float light=mix(.22f+.63f*proximity+edge*(.30f+.70f*proximity),sphere,morph);
            light*=(.94f+.06f*ss(hashf(i)*10+T*1.2f))*(1+.17f*kick);
            hue=lerp(hue,white,clamp(edge*proximity*.13f,0,.22f)*(1-morph));
            c=lerp(c,hue,shape);a=mix(a,light,shape);
        }
        // The same reserve stars form a short connecting current at each cut.
        if(i>=14000&&i<14600&&flow>0){
            float u=(i-14000)/600.0f,angle=u*TAU+T*.24f;
            V halo=v(4.5f*cc(angle),4.5f*ss(angle),-.8f+.24f*ss(angle*3));
            V q=halo;
            if(join==0)q=lerp(v(7*cc(angle),3*ss(angle)-.5f,-8),halo,progress);
            if(join==1)q=lerp(halo,dragonPath(u),progress);
            if(join==2)q=lerp(dragonPath(u),lotus(i-14000,T),progress);
            if(join==3)q=lerp(halo,v(5.8f*cc(angle),1.8f*ss(angle)+.1f,0),progress);
            q=add(q,v(.05f*ss(i*3),.05f*cc(i*7),.05f*ss(i*11)));
            p=lerp(p,q,flow);c=lerp(c,lerp(gold,cyan,progress),flow);a=mix(a,.52f,flow);
        }
        color(c,a);vertex(p);
    }
    glEnd();glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);pglUseProgram(geometryProgram);opaque();
}
static void photonOrbits(float alpha,float radius){
    transparent();color(cyan,alpha*.2f);glLineWidth(1);
    for(int j=0;j<5;j++){
        glPushMatrix();glRotatef(j*37+T*3,0,1,0);glRotatef(60+j*19,1,0,0);ring(0,0,0,radius+j*.13f,144);
        color(gold,alpha*.6f);for(int n=0;n<8;n++){float a=n*TAU/8+T*.18f;ring(radius*cc(a),radius*ss(a),0,.035f,8);}color(cyan,alpha*.2f);glPopMatrix();
    }opaque();
}
static void screenText(void){
    glDisable(GL_DEPTH_TEST);transparent();pglUseProgram(geometryProgram);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,1600,0,900,-10,10);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glLineWidth(1.6f*H/720.0f);
    float title=smooth(1,3,T)*(1-smooth(10,13,T));
    if(T>135)title=smooth(135,138,T);
    if(title>0){
        float y=T>135?460:486;
        glLineWidth(2.1f*H/720.0f);centered("ASTRA",y,91,12,white,title);
        glLineWidth(1.1f*H/720.0f);centered("by GPT-6 Astra",y-46,17,9,gold,title*.88f);
        color(cyan,title*.5f);glBegin(GL_LINES);edge(647,y-66,953,y-66);glEnd();
        centered(T>135?"LIGHT IS A GIFT":"A SIGNAL BECOMES A SONG",y-100,10,11,cyan,title*.65f);
    }
    for(int p=0;p<6;p++){
        Phrase ph=phrases[p];if(T<ph.start-.55f||T>ph.end+1.15f)continue;
        int chars=0;for(int j=0;j<ph.count;j++)chars+=strlen(words[ph.first+j].text)+1;
        float h=24,step=h/8*8.8f,x=800-(chars-1)*step*.5f;
        for(int j=0;j<ph.count;j++){
            Word w=words[ph.first+j];int n=strlen(w.text);
            for(int k=0;k<n;k++){
                float local=T-w.start+.34f-k*.016f,in=smooth(0,.32f,local),out=smooth(ph.end+.25f+k*.025f,ph.end+.95f+k*.025f,T);
                float flight=1-in+out,seed=hashf((ph.first+j)*31+k),a=in*(1-out);
                if(a>.002f){
                    glPushMatrix();glTranslatef(x+k*step+(seed-.5f)*140*flight,99+60*flight+25*ss(seed*30)*flight,0);glRotatef((seed-.5f)*180*flight,0,0,1);glScalef(h/8,h/8,1);
                    float sung=smooth(w.start-.03f,w.start+.06f,T)*(1-smooth(w.start+w.len-.03f,w.start+w.len+.22f,T));
                    color(lerp(cyan,gold,sung),a*(.42f+.58f*sung));glyph(w.text[k]>='a'&&w.text[k]<='z'?w.text[k]-32:w.text[k]);glPopMatrix();
                }
            }
            x+=(n+1)*step;
        }
    }
    glLineWidth(1);opaque();
}
typedef struct {V eye,at;float roll,fov;} Shot;
static Shot sceneCamera(int phase,float time){
    V eye=v(3*ss(time*.12f),1,17),at=v(0,0,0);float roll=0,fov=49;
    if(phase==1){float t=time-16;eye=v(.42f*ss(t*.22f),-1.12f+.12f*ss(t*.27f),18-t*2.4f);at=v(.08f*ss(t*.22f),-1.08f,eye.z-15);roll=.010f*ss(t*.2f);fov=52;}
    if(phase==2){float t=time-40;eye=v(5.1f*ss(t*.11f-.7f),1.0f+ss(t*.15f)*.6f,13.0f+.7f*ss(t*.14f));at=v(0,.0f,0);roll=.025f*ss(t*.17f);fov=46;}
    if(phase==3){float t=time-64;eye=v(-5+5*ss(t*.10f),3.2f+1.5f*ss(t*.08f),22+1.3f*cc(t*.13f));at=v(-1,0,0);roll=.04f*ss(t*.11f);fov=51;}
    if(phase==4){float t=time-96,a=t*.035f-.3f,r=19+2*smooth(113,118,time),returning=smooth(126,130,time);a=mix(a,.19f,returning);r=mix(r,16,returning);eye=v(r*ss(a),mix(8-5*smooth(112,121,time),1.1f,returning),r*cc(a));at=v(0,.2f,0);fov=49;}
    Shot s={eye,at,roll,fov};return s;
}
static void frame(float time){
    T=time;beat=T*2;kick=exp(-fract(beat)*8);hat=exp(-fract(beat*4)*9);energy=.55f+.45f*kick;
    int phase=T<16?0:T<40?1:T<64?2:T<96?3:4;
    pglBindFramebuffer(GL_FRAMEBUFFER,sceneFB);glViewport(0,0,W,H);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);pglUseProgram(backgroundProgram);uf(backgroundProgram,"time",T);uf(backgroundProgram,"act",phase);uf(backgroundProgram,"pulse",kick);u2(backgroundProgram,"res",W,H);quad();
    glEnable(GL_DEPTH_TEST);glDepthFunc(GL_LEQUAL);pglUseProgram(geometryProgram);
    Shot shot=sceneCamera(phase,T);
    static const float cuts[]={16,40,64,96};
    for(int j=0;j<4;j++)if(T>cuts[j]-(j==1?2.5f:1.5f)&&T<cuts[j]+(j==1?0:1.5f)){
        float blend=smooth(cuts[j]-(j==1?2.5f:1.5f),cuts[j]+(j==1?0:1.5f),T);
        Shot a=sceneCamera(j,T<cuts[j]?T:cuts[j]),b=sceneCamera(j+1,T>cuts[j]?T:cuts[j]);
        shot.eye=lerp(a.eye,b.eye,blend);shot.at=lerp(a.at,b.at,blend);
        shot.roll=mix(a.roll,b.roll,blend);shot.fov=mix(a.fov,b.fov,blend);
    }
    artEye=shot.eye;artAt=shot.at;
    camera(shot.eye,shot.at,shot.roll,shot.fov);starsScene(phase);pglUseProgram(geometryProgram);
    if(phase==0){photonOrbits(smooth(0,5,T)*(1-smooth(12,16,T)),3.35f);}
    if(phase==1){memoryScene(smooth(16,18,T)*(1-smooth(38,40,T))*(1-.64f*smooth(21,24,T)*(1-smooth(33,36,T))));particleBook();}
    if(phase==2){portraitSky();diffractionScene();headScene(smooth(40,44,T)*(1-smooth(61.5f,64,T)),1.35f);}
    if(phase==3){dragonLights();dragonScene();pavilion();}
    if(phase==4){
        photonOrbits(smooth(112,120,T)*(1-smooth(127,131,T)),6.4f);
        if(T>127&&T<136)headScene(smooth(127,129,T)*(1-smooth(133,136,T)),1.28f);
    }
    screenText();kirchhoffOverlay();post();
}
