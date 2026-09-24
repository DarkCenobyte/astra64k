/* Astra v2: a porcelain voxel shell, panoramic black glass, expressive LEDs. */
static V headPoint(float a,float y){
    float w=1.73f*sqrt(clamp(1-(y/2.74f)*(y/2.74f),0,1));
    w*=1-.27f*smooth(-.25f,-2.50f,y);
    float x=w*ss(a),z=1.30f*cc(a)*sqrt(clamp(1-(y/2.90f)*(y/2.90f),0,1));
    z+=.065f*exp(-pow((fabs(x)-.87f)/.48f,2)-pow((y+.45f)/.66f,2))*clamp(cc(a),0,1);
    float yy=y<-.65f?-.65f+(y+.65f)*.78f:y>1.30f?1.30f+(y-1.30f)*.88f:y;
    return v(x,yy,z);
}
static float glassZ(float x,int lower){return lower?1.44f-.18f*x*x:1.58f-.255f*x*x;}
static void led(float x,float y,float w,float h,V c,float level,int lower){
    color(mul(c,level),1);glBegin(GL_QUADS);
    glVertex3f(x-w,y-h,glassZ(x-w,lower)+.025f);glVertex3f(x+w,y-h,glassZ(x+w,lower)+.025f);
    glVertex3f(x+w,y+h,glassZ(x+w,lower)+.025f);glVertex3f(x-w,y+h,glassZ(x-w,lower)+.025f);glEnd();
}
static void glass(int lower,float amount){
    float width=lower?.70f:1.78f,height=lower?.265f:.67f,cy=lower?-.96f:.44f;
    glPushMatrix();glScalef(amount,amount,amount);
    opaque();
    glBegin(GL_QUADS);
    for(int i=0;i<56;i++){
        float x0=width*(i/28.0f-1),x1=width*((i+1)/28.0f-1);
        float h0=height*sqrt(clamp(1-pow(x0/width,8),0,1)),h1=height*sqrt(clamp(1-pow(x1/width,8),0,1));
        color(v(.0025f,.006f,.011f),1);
        glVertex3f(x0,cy-h0,glassZ(x0,lower));glVertex3f(x1,cy-h1,glassZ(x1,lower));
        color(v(.006f,.012f,.023f),1);
        glVertex3f(x1,cy+h1,glassZ(x1,lower));glVertex3f(x0,cy+h0,glassZ(x0,lower));
    }
    glEnd();
    transparent();color(lower?pink:cyan,amount*(lower?.24f:.38f));glLineWidth(1.1f);
    for(int side=-1;side<=1;side+=2){
        glBegin(GL_LINE_STRIP);for(int i=0;i<=56;i++){
            float x=width*(i/28.0f-1),h=height*sqrt(clamp(1-pow(x/width,8),0,1));
            glVertex3f(x,cy+side*h,glassZ(x,lower)+.013f);
        }glEnd();
    }
    if(!lower){
        color(white,.085f*amount);glBegin(GL_LINE_STRIP);
        for(int i=0;i<32;i++){float x=-1.5f+i*.085f;glVertex3f(x,.91f+.035f*ss(x*2),glassZ(x,0)+.014f);}glEnd();
    }
    opaque();glPopMatrix();
}
static void audioVisualizers(float assembly){
    float levels[2][BANDS];spectrum(T,levels);
    transparent();glLineWidth(1.0f);
    for(int ch=0;ch<2;ch++){
        int side=ch*2-1;
        glPushMatrix();glTranslatef(side*4.75f,0,-2.5f);glRotatef(side*-18,0,1,0);
        color(cyan,.12f*assembly);
        glBegin(GL_LINES);
        for(int i=0;i<5;i++)edge(-1.43f,-2.35f+i*1.05f,1.43f,-2.35f+i*1.05f);
        edge(-1.43f,-2.35f,-1.43f,2.0f);edge(1.43f,-2.35f,1.43f,2.0f);glEnd();
        glBegin(GL_QUADS);
        for(int b=0;b<BANDS;b++){
            float x=-1.35f+b*.15f,h=levels[ch][b]*20;
            for(int j=0;j<20;j++){
                float y=-2.28f+j*.205f,q=j/19.0f;
                V c=q<.52f?lerp(cyan,v(.25f,.42f,1),q/.52f):lerp(pink,gold,(q-.52f)/.48f);
                float light=clamp(h-j,0,1);
                color(c,assembly*(.014f+light*.82f));
                glVertex3f(x,y,0);glVertex3f(x+.10f,y,0);glVertex3f(x+.10f,y+.15f,0);glVertex3f(x,y+.15f,0);
            }
            float y=-2.28f+levels[ch][b]*4.10f;
            color(lerp(cyan,gold,b/(float)(BANDS-1)),assembly*(levels[ch][b]>.01f?.85f:0));
            glVertex3f(x,y,.01f);glVertex3f(x+.10f,y,.01f);glVertex3f(x+.10f,y+.035f,.01f);glVertex3f(x,y+.035f,.01f);
        }
        glEnd();glPopMatrix();
    }
}
static void headScene(float assembly,float scale){
    glPushMatrix();glScalef(scale,scale,scale);float speak=mouth(T);
    opaque();
    for(int row=0;row<43;row++)for(int col=0;col<66;col++){
        float y=-2.52f+row*.12f,a=col*TAU/66;V p=headPoint(a,y);
        int id=row*66+col;float h=hashf(id+923),front=cc(a);
        if(front>.40f&&y>-.20f&&y<1.08f)continue;
        if(front>.74f&&fabs(p.x)<.69f&&fabs(y+.96f)<.26f)continue;
        float as=smooth(h*.45f,.56f+h*.44f,assembly);
        V spawn=mul(norm(v(hashf(id*3)-.5f,hashf(id*3+1)-.5f,hashf(id*3+2)-.5f)),8+5*h);
        V c=lerp(v(.045f,.065f,.105f),v(.20f,.25f,.30f),.5f+.5f*ss(a));
        if(front>.4f){
            c=add(c,mul(v(.075f,.032f,.055f),exp(-pow((fabs(p.x)-1.0f)/.32f,2)-pow((y+.45f)/.55f,2))));
            if(y>1.55f&&((col+5)%19==0||row==37))c=lerp(c,cyan,.27f);
            if(fabs(p.x)>1.25f&&row%9==0)c=lerp(c,pink,.23f);
        }else if(col%13==0||row==34)c=lerp(c,cyan,.22f);
        if(h>.988f)c=lerp(c,gold,.40f);
        cube(lerp(spawn,p,as),.108f*(.55f+.45f*as),c,as*(1.17f+.12f*kick));
    }
    float display=smooth(.24f,.91f,assembly);
    if(display>.001f){
        glass(0,display);glass(1,display);
        glPushMatrix();glScalef(display,display,display);
        float joy=clamp(smooth(52,56,T)*(1-smooth(59,61,T))+smooth(127,130,T),0,1);
        float blink=1-.96f*exp(-pow((fract((T-40)*.19f+.27f)-.93f)/.018f,2));
        float gaze=.035f*ss(T*.61f);
        for(int side=-1;side<=1;side+=2)for(int ix=-10;ix<=10;ix++)for(int iy=-5;iy<=5;iy++){
            float dx=ix*.047f,dy=iy*.047f,x=side*.78f+dx,y=.43f+dy;
            float oval=pow(dx/.445f,2)+pow(dy/(.165f*blink+.014f),2);
            float eye=(fabs(oval-1)<.29f&&fabs(dx)<.45f)?.82f:0;
            if(fabs(dx-gaze)<.065f&&fabs(dy)<.14f*blink)eye=1.42f;
            float happy=fabs(dy-(.13f-.48f*dx*dx))<.026f&&fabs(dx)<.43f?1.10f:0;
            float intensity=mix(eye,happy,joy);
            if(side*ix>=8&&iy>=2&&iy==side*ix-6)intensity=.70f;
            V c=intensity>.9f?white:cyan;
            led(x,y,.0155f,.0155f,intensity>.01f?c:v(.025f,.048f,.061f),intensity>.01f?intensity:1,0);
        }
        for(int ix=-9;ix<=9;ix++)for(int iy=-4;iy<=4;iy++){
            float x=ix*.061f,y=iy*.043f,curve=-.035f+.26f*x*x;
            float rest=fabs(y-curve)<.025f?1:0;
            float oval=pow(x/(.50f-.08f*speak),2)+pow(y/(.035f+.16f*speak),2);
            float singing=fabs(oval-1)<.34f?1:0,lit=mix(rest,singing,smooth(.02f,.20f,speak));
            led(x,-.96f+y,.021f,.011f,lit>.05f?lerp(pink,gold,.27f):v(.045f,.017f,.030f),lit>.05f?lit*(.75f+speak*.7f):1,1);
        }
        glPopMatrix();
    }
    for(int side=-1;side<=1;side+=2){
        for(int j=0;j<10;j++){float y=.62f-j*.17f;cube(v(side*(1.65f-j*.018f),y,-.05f),.094f,lerp(gold,pink,.28f),.65f*assembly);}
        glPushMatrix();glTranslatef(side*1.77f,.32f,-.22f);glRotatef(90,0,1,0);transparent();
        color(gold,.47f*assembly);ring(0,0,0,.43f,36);color(cyan,.24f*assembly);ring(0,0,0,.50f,36);glPopMatrix();
    }
    transparent();glLineWidth(1.0f);color(cyan,.28f*assembly);
    ring(0,0,-.65f,3.22f,120);ring(0,0,-.65f,3.29f,120);color(gold,.46f*assembly);
    glBegin(GL_LINES);for(int i=0;i<96;i++){float a=TAU*i/96+T*.045f,r=i%8==0?3.63f:3.39f;line(v(cc(a)*3.30f,ss(a)*3.30f,-.65f),v(cc(a)*r,ss(a)*r,-.65f));}glEnd();
    audioVisualizers(assembly);
    glPopMatrix();opaque();
}
