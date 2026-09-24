/* Fresnel-Kirchhoff, exp(-i omega t), normal incidence; BYU Optics 10.10.
   A stylized cross-section of a narrow aperture accompanies the equation.
   Nine coherent secondary sources use exp(ikr)/r and the obliquity factor;
   the drawing exaggerates the wave fronts for an immediate visual reading. */
#include "formula_points.h"
static void diffractionScene(void){
    if(T<48.8f||T>=53.7f)return;
    float fade=smooth(48.8f,49.35f,T)*(1-smooth(52.85f,53.7f,T));
    float elapsed=T-49.0f,omega=TAU*3.6f*elapsed;
    float modulation=.78f+.22f*(lightBands[2]+lightBands[6])*.5f;
    glPushMatrix();glTranslatef(-2,-.5f,0);
    transparent();pglUseProgram(geometryProgram);glLineWidth(1.4f*H/720.0f);
    glBegin(GL_LINES);
    for(int side=-1;side<=1;side+=2){
        color(lerp(gold,white,.22f),fade*.65f);
        line(v(-3.8f,5.15f+side*.62f,-8),v(-3.8f,5.15f+side*2.8f,-8));
        line(v(-4.0f,5.15f+side*.62f,-8),v(-3.6f,5.15f+side*.62f,-8));
    }
    glEnd();
    // Travelling contours make the change from planar to curved fronts legible.
    // The lower arcs pass behind the portrait, never over its eyes or mouth.
    for(int pass=0;pass<2;pass++){
        glLineWidth((pass?1.45f:4.8f)*H/720.0f);
        float strength=fade*modulation*(pass?.72f:.09f);
        glBegin(GL_LINES);
        for(int n=1;n<=5;n++){
            float x=-3.8f-n*1.50f+fract(elapsed*3.6f)*1.50f;
            for(int j=0;j<25;j++){
                float y=2.6f+j*.20f;
                float edge=(1-smooth(1.8f,2.65f,fabs(y-5.15f)))*smooth(-11.3f,-9.8f,x);
                color(cyan,strength*edge*.46f);
                line(v(x,y,-8.02f),v(x,y+.20f,-8.02f));
            }
        }
        for(int n=0;n<18;n++){
            float radius=elapsed*5.4f-n*1.50f;
            if(radius<.15f||radius>16)continue;
            for(int j=0;j<96;j++){
                float angle=-1.18f+j*2.36f/96,next=angle+2.36f/96;
                V a=v(-3.8f+radius*cc(angle),5.15f+radius*ss(angle),-8.01f);
                V b=v(-3.8f+radius*cc(next),5.15f+radius*ss(next),-8.01f);
                float edge=(1-smooth(2.15f,3.05f,fabs(a.y-5.15f)))*(1-smooth(10,12.5f,a.x));
                float angular=pow(clamp(cc(angle*.95f),0,1),2);
                color(lerp(cyan,white,.25f),strength*edge*angular/(1+radius*.07f));
                line(a,b);
            }
        }
        glEnd();
    }
    glLineWidth(1);
    pglUseProgram(pointProgram);glPointSize(3.2f*H/720.0f);glBegin(GL_POINTS);
    for(int iy=0;iy<56;iy++)for(int ix=0;ix<214;ix++){
        float x=-11.3f+ix*.112f,y=2.35f+iy*.10f,dx=x+3.8f,dy=y-5.15f;
        float real=0,imaginary=0,intensity;
        if(dx<0){
            intensity=pow(clamp(.5f+.5f*cc(TAU*dx/1.50f-omega),0,1),10)*.45f;
        }else{
            for(int source=0;source<9;source++){
                float q=dy-(source-4)*.14f,r=sqrt(dx*dx+q*q+.025f);
                float weight=(1+dx/r)*.5f/(r+.50f),phase=TAU*r/1.50f;
                real+=cc(phase)*weight;imaginary+=ss(phase)*weight;
            }
            float crest=(real*cc(omega)+imaginary*ss(omega))/9;
            intensity=pow(clamp(crest*sqrt(dx+1)*1.55f,0,1),2);
            intensity*=1-smooth(elapsed*5.4f-.65f,elapsed*5.4f+.25f,sqrt(dx*dx+dy*dy));
        }
        float edge=(1-smooth(2.0f,2.8f,fabs(dy)))*smooth(-11.3f,-10.3f,x)*(1-smooth(10,12.55f,x));
        color(lerp(cyan,white,dx>0?.28f:.04f),fade*modulation*edge*intensity*.76f);
        glVertex3f(x,y,-8);
    }
    for(int source=0;source<9;source++){
        color(lerp(gold,white,.45f),fade*(.58f+.20f*kick));
        glVertex3f(-3.8f,5.15f+(source-4)*.14f,-7.98f);
    }
    glEnd();glPopMatrix();pglUseProgram(geometryProgram);opaque();
}
static void kirchhoffOverlay(void){
    if(T<48||T>=54)return;
    float fade=smooth(48,48.4f,T)*(1-smooth(53.65f,54,T));
    glDisable(GL_DEPTH_TEST);transparent();
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,1600,0,900,-10,10);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();glTranslatef(338,171,0);glScalef(.88f,.88f,1);
    // A soft halo only around the strokes improves contrast without a panel.
    pglUseProgram(pointProgram);
    for(int pass=0;pass<2;pass++){
        glPointSize((pass?2.75f:5.6f)*H/720.0f);
        glBlendFunc(GL_SRC_ALPHA,pass?GL_ONE:GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);
        for(unsigned id=0;id<sizeof(equationShape)/sizeof(equationShape[0]);id++){
            float x=equationShape[id][0]*.25f,y=equationShape[id][1]*.25f;
            int band=(int)(x*(BANDS-1)/1050);band=(int)clamp(band,0,BANDS-1);
            float seed=hashf(id+791),arrival=smooth(48+band*.008f,48.62f+band*.008f,T);
            float leave=smooth(53.24f+seed*.08f,54,T),flight=1-arrival+leave;
            float swirl=T*.65f+seed*TAU;
            x+=flight*(ss(swirl)*135+(seed-.5f)*570);
            y+=flight*(cc(swirl)*100+110*(hashf(id+829)-.5f));
            V c=lerp(gold,white,.70f+band%3*.05f);
            if(band==4||band==5)c=lerp(cyan,white,.72f);
            float alpha=fade*arrival*(1-leave);
            color(pass?c:v(.001f,.004f,.010f),alpha*(pass?.47f+.14f*lightBands[band]:.30f));
            glVertex3f(x,y,0);
        }
        glEnd();
    }
    pglUseProgram(geometryProgram);opaque();
}
