/* Sparse, beat-triggered constellations. Background only: no lotus repetition. */
static V sparkPosition(V origin,V direction,float age,float speed){
    return add(origin,add(mul(direction,speed*(1-exp(-age*.95f))),v(0,-.23f*age*age,0)));
}
static void dragonLights(void){
    transparent();pglUseProgram(pointProgram);glPointSize(2.55f*H/720.0f);glBegin(GL_POINTS);
    for(int event=0;event<12;event++){
        float age=T-(66+event*2.5f);if(age<-.9f||age>3.8f)continue;
        float side=event%2?1:-1;
        V center=v(side*(10+4*hashf(event+250)),6.0f+4.2f*hashf(event+150),-12-7*hashf(event+350));
        V hue=event%3==0?gold:event%3==1?cyan:lerp(pink,gold,.15f);
        if(age<0){
            for(int i=0;i<26;i++){float a=age-i*.012f;V p=add(center,v(a*side*.7f,7*a,-a*.8f));color(lerp(hue,white,.4f),(.85f-i*.029f)*smooth(-.9f,-.5f,age));vertex(p);}
        }else{
            float alpha=(1-smooth(1.1f,3.8f,age))*smooth(0,.10f,age)*(1-smooth(94.8f,96,T));
            for(int i=0;i<224;i++){
                float z=1-2*(i+.5f)/224.0f,theta=i*2.39996323f+event*.8f;
                V dir=v(sqrt(1-z*z)*cc(theta),z,sqrt(1-z*z)*ss(theta));
                float speed=3.2f+2.8f*hashf(i*7+event*193);
                for(int trail=0;trail<6;trail++){
                    float a=age-trail*.045f;if(a<0)continue;
                    color(lerp(hue,white,trail==0?.58f:.06f),alpha*(.78f-trail*.12f)*(1+.11f*kick));
                    vertex(sparkPosition(center,dir,a,speed));
                }
            }
        }
    }
    // Two distant meteor sweeps provide quiet motion between the bursts.
    for(int m=0;m<2;m++)for(int i=0;i<52;i++){
        float u=fract((T-64)*.048f+m*.47f)-i*.0019f;if(u<0)continue;
        V p=v(-22+44*u,8+m*2.8f+2*ss(u*PI),-24+4*cc(u*PI));
        color(m?pink:cyan,(1-i/52.0f)*.16f*smooth(64,67,T)*(1-smooth(93,96,T)));vertex(p);
    }
    glEnd();pglUseProgram(geometryProgram);opaque();
}
