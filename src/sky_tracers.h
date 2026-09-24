/* Distant multi-beam searchlights. They render before the unmodified portrait.
   The same final-PCM spectrum drives both the haze and the foreground equation. */
static float lightBands[BANDS];
static void portraitSky(void){
    if(T<46||T>=63)return;
    float fade=smooth(46,47.4f,T)*(1-smooth(61.7f,63,T));
    float levels[2][BANDS];spectrum(T,levels);
    for(int b=0;b<BANDS;b++)lightBands[b]=(levels[0][b]+levels[1][b])*.5f;
    transparent();pglUseProgram(geometryProgram);
    for(int bank=0;bank<4;bank++){
        int side=bank&1?1:-1;
        V origin=v(side*(8.4f+3.7f*(bank/2)),-7.5f,-15-5*(bank/2));
        for(int ray=0;ray<5;ray++){
            float spread=(ray-2)*3.65f,sweep=8*ss((T-46)*.28f+bank*.76f);
            V target=v(side*(4.0f+spread+sweep),21+2*ss(T*.24f+bank),-22+4*ss(T*.19f+ray*.14f+bank));
            V axis=norm(sub(target,origin)),width=norm(cross(axis,v(0,0,1)));
            V tint=lerp(cyan,bank/2?pink:v(.39f,.43f,1),.40f+.30f*ss(ray*.9f+bank));
            float optics=smooth(48.6f,49.2f,T)*(1-smooth(53.2f,53.9f,T));
            float intensity=fade*(1-.62f*optics)*(.07f+.14f*lightBands[(bank*4+ray*2)%BANDS]);
            // Radial strips soften the beam edges; the tips disappear into space.
            for(int slice=0;slice<10;slice++){
                float s0=-1+slice*.2f,s1=s0+.2f;
                glBegin(GL_TRIANGLE_STRIP);
                for(int j=0;j<=12;j++){
                    float u=j/12.0f,w=.045f+u*.74f,fall=(1-smooth(.52f,1,u))*(.5f+.5f*smooth(0,.1f,u));
                    V p=lerp(origin,target,u);
                    color(tint,intensity*exp(-s0*s0*5)*fall);vertex(add(p,mul(width,s0*w)));
                    color(tint,intensity*exp(-s1*s1*5)*fall);vertex(add(p,mul(width,s1*w)));
                }
                glEnd();
            }
            // A thin bright core gives the fan its sky-tracer character.
            glBegin(GL_LINES);color(lerp(tint,white,.35f),intensity*.75f);vertex(origin);
            color(tint,0);vertex(target);glEnd();
        }
        glPushMatrix();glTranslatef(origin.x,origin.y,origin.z);glRotatef(78,1,0,0);
        color(cyan,fade*.24f);ring(0,0,0,.40f,40);color(gold,fade*.18f);ring(0,0,0,.55f,40);glPopMatrix();
    }
    opaque();
}
