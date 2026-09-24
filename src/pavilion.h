/* A monumental pagoda complex: five-storey central tower, two broad side halls,
   swept roofs, dougong-inspired brackets, balconies, framed windows and stairs.
   The composition is architectural line work, with no image textures. */
static V pavilionPoint(int side,float u,float r,float y){
    float a=TAU*side/8,b=a+TAU/8;
    return lerp(v(r*cc(a),y,r*ss(a)),v(r*cc(b),y,r*ss(b)),u);
}
static V roofPoint(int side,float across,float u,float r,float y){
    float rise=2.15f+r*.085f;
    float h=y+rise*(1-u)*(1-u)+.73f*smooth(.70f,1,u);
    h+=.85f*pow(fabs(across*2-1),4)*u*u*u;
    return pavilionPoint(side,across,r*u,h);
}
static void pagodaRoof(float r,float y,float a){
    for(int side=0;side<8;side++){
        float face=.15f+.85f*smooth(-.8f,.65f,ss((side+.5f)*TAU/8));
        for(int rib=0;rib<=8;rib++)for(int j=0;j<20;j++){
            float u=j/20.0f,q=rib/8.0f;
            color(rib%8==0?gold:lerp(cyan,gold,.08f),a*face*(rib%8==0?.90f:.14f));
            line(roofPoint(side,q,u,r,y),roofPoint(side,q,u+.05f,r,y));
        }
        for(int purlin=0;purlin<4;purlin++)for(int j=0;j<20;j++){
            float u=1-purlin*.22f;
            color(lerp(gold,white,.16f),a*face*(purlin==0?1.0f:.20f));
            line(roofPoint(side,j/20.0f,u,r,y),roofPoint(side,(j+1)/20.0f,u,r,y));
            if(!purlin){
                color(cyan,a*face*.28f);
                line(add(roofPoint(side,j/20.0f,u,r,y),v(0,-.20f,0)),add(roofPoint(side,(j+1)/20.0f,u,r,y),v(0,-.20f,0)));
            }
        }
        // Curled hip ornaments extend the roof's silhouette.
        V tip=roofPoint(side,0,1,r,y),radial=norm(v(tip.x,0,tip.z));
        color(gold,a*face*.72f);
        V p=add(tip,add(mul(radial,.55f),v(0,.25f,0))),q=add(tip,add(mul(radial,.85f),v(0,.82f,0)));
        line(tip,p);line(p,q);line(q,add(q,add(mul(radial,-.12f),v(0,.12f,0))));
    }
}
static void balcony(float r,float y,float a){
    for(int side=0;side<8;side++){
        float face=.16f+.84f*smooth(-.8f,.65f,ss((side+.5f)*TAU/8));
        for(int rail=0;rail<3;rail++){
            color(rail==2?gold:cyan,a*face*(rail==2?.36f:.24f));
            line(pavilionPoint(side,0,r,y+rail*.42f),pavilionPoint(side,1,r,y+rail*.42f));
        }
        for(int j=0;j<10;j++){
            float u=j/10.0f;color(cyan,a*face*.26f);
            line(pavilionPoint(side,u,r,y),pavilionPoint(side,u,r,y+.84f));
            line(pavilionPoint(side,u,r,y+.22f),pavilionPoint(side,u+.05f,r,y+.68f));
            line(pavilionPoint(side,u+.05f,r,y+.68f),pavilionPoint(side,u+.1f,r,y+.22f));
        }
    }
}
static void pagodaStorey(float roofRadius,float roofY,float floorY,float a,int ground){
    float r=roofRadius*.63f;
    pagodaRoof(roofRadius,roofY,a);
    balcony(r*1.13f,floorY+.18f,a*.75f);
    for(int side=0;side<8;side++){
        float face=.16f+.84f*smooth(-.8f,.65f,ss((side+.5f)*TAU/8));
        // Horizontal load-bearing beams and four edges of each corner column.
        for(int j=0;j<3;j++){
            float y=j==0?floorY:j==1?roofY-.7f:roofY+.05f;
            color(j==1?gold:cyan,a*face*.40f);
            line(pavilionPoint(side,0,r,y),pavilionPoint(side,1,r,y));
        }
        for(int k=0;k<4;k++){
            float dx=(k&1)?.14f:-.14f,dz=(k&2)?.14f:-.14f;
            color(lerp(cyan,gold,.12f),a*face*.43f);
            line(add(pavilionPoint(side,0,r,floorY),v(dx,0,dz)),add(pavilionPoint(side,0,r,roofY+.35f),v(dx,0,dz)));
        }
        // Layered cantilever brackets below each hip.
        V column=pavilionPoint(side,0,r,roofY);
        for(int k=0;k<4;k++){
            float width=.34f+k*.23f;V mid=add(column,v(0,-.58f+k*.25f,0));
            color(gold,a*face*(.24f+k*.075f));
            for(int d=-1;d<=1;d+=2){
                V tip=add(mid,v(width*d,.14f,0));line(mid,tip);line(tip,add(tip,v(0,.19f,0)));
                tip=add(mid,v(0,.14f,width*d));line(mid,tip);line(tip,add(tip,v(0,.19f,0)));
            }
        }
        // Three framed bays per facade; an arched doorway on the ground level.
        for(int bay=0;bay<3;bay++){
            float u=.13f+bay*.285f,w=.17f,low=floorY+.72f,high=roofY-1.0f;
            color(cyan,a*face*.28f);
            line(pavilionPoint(side,u,r,low),pavilionPoint(side,u,r,high));
            line(pavilionPoint(side,u+w,r,low),pavilionPoint(side,u+w,r,high));
            line(pavilionPoint(side,u,r,low),pavilionPoint(side,u+w,r,low));
            if(ground&&bay==1){
                for(int j=0;j<12;j++){
                    float t=j*PI/12,tt=(j+1)*PI/12;
                    line(pavilionPoint(side,u+w*.5f+w*.5f*cc(t),r,high+.55f*ss(t)),pavilionPoint(side,u+w*.5f+w*.5f*cc(tt),r,high+.55f*ss(tt)));
                }
            }else{
                line(pavilionPoint(side,u,r,high),pavilionPoint(side,u+w,r,high));
                for(int k=1;k<3;k++){
                    float f=k/3.0f;
                    line(pavilionPoint(side,u+w*f,r,low),pavilionPoint(side,u+w*f,r,high));
                    line(pavilionPoint(side,u,r,mix(low,high,f)),pavilionPoint(side,u+w,r,mix(low,high,f)));
                }
            }
        }
    }
}
static void pagodaFinial(float y,float a){
    color(gold,a*.64f);line(v(0,y,0),v(0,y+2.0f,0));
    for(int j=0;j<5;j++)for(int side=0;side<8;side++){
        float r=.62f-j*.10f;
        line(pavilionPoint(side,0,r,y+.25f+j*.34f),pavilionPoint(side,1,r,y+.25f+j*.34f));
    }
}
static void pavilion(void){
    float a=smooth(64,68,T)*(1-smooth(93.5f,96,T));
    uf(geometryProgram,"haze",1);
    glPushMatrix();glTranslatef(.5f,0,-32);glRotatef(10,0,1,0);
    transparent();glLineWidth(1.0f*H/720.0f);glBegin(GL_LINES);
    // A broad five-tier tower. Floor heights remain below their supporting eaves.
    for(int tier=0;tier<5;tier++){
        float r=11.6f-tier*1.45f,y=-7.8f+tier*3.95f;
        pagodaStorey(r,y,tier?y-3.4f:-12.9f,a,tier==0);
    }
    pagodaFinial(10.7f,a);
    // Symmetric two-storey side halls and open connecting galleries.
    glEnd();
    for(int side=-1;side<=1;side+=2){
        glPushMatrix();glTranslatef(side*17.4f,-.4f,1.0f);glBegin(GL_LINES);
        pagodaStorey(8.7f,-9.1f,-12.9f,a*.80f,1);
        pagodaStorey(6.1f,-5.1f,-8.5f,a*.84f,0);pagodaFinial(-2.45f,a*.75f);
        glEnd();glPopMatrix();
        glBegin(GL_LINES);color(cyan,a*.25f);
        for(int z=0;z<2;z++)for(int j=0;j<7;j++){
            float x=side*(7+j*1.3f),zz=1+z*3.2f;
            line(v(x,-13,zz),v(x,-10,zz));
            if(j<6){line(v(x,-10,zz),v(x+side*1.3f,-10,zz));line(v(x,-12,zz),v(x+side*1.3f,-12,zz));}
        }
        glEnd();
    }
    glBegin(GL_LINES);
    // Monumental terrace, long front balustrade and central ceremonial stairs.
    for(int step=0;step<5;step++){
        float x=27+step*.34f,z=10+step*.25f,y=-13.3f-step*.22f;
        color(step%2?gold:cyan,a*(step%2?.28f:.23f));
        line(v(-x,y,-8),v(-x,y,z));line(v(x,y,-8),v(x,y,z));
        line(v(-x,y,z),v(-4.2f,y,z));line(v(4.2f,y,z),v(x,y,z));
    }
    for(int side=-1;side<=1;side+=2){
        for(int j=0;j<24;j++){
            float x=side*(4.5f+j*.94f);color(cyan,a*.26f);
            line(v(x,-13.1f,9.9f),v(x,-11.95f,9.9f));
            if(j<23){line(v(x,-11.95f,9.9f),v(x+side*.94f,-11.95f,9.9f));line(v(x,-12.65f,9.9f),v(x+side*.94f,-12.65f,9.9f));}
        }
    }
    color(gold,a*.38f);
    for(int j=0;j<12;j++){
        float z=10+j*.44f,y=-13.1f-j*.18f;
        line(v(-4.15f,y,z),v(4.15f,y,z));
        for(int side=-1;side<=1;side+=2)line(v(side*4.15f,y,z),v(side*4.15f,y-.18f,z+.44f));
    }
    glEnd();glPopMatrix();opaque();uf(geometryProgram,"haze",0);
}
