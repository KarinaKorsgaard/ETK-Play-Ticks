#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex;
uniform vec2 u_mask1;
uniform vec2 u_mask2;
uniform vec2 u_resolution;
uniform float u_radius1;
uniform float u_radius2;
uniform float u_time;

uniform vec2 u_beginLight1,u_beginLight2;

float rand(in float y){
    return fract(sin(y)*10000.0);
}


float circle(in vec2 _st, in vec2 _mask, float _r, float t , float border){
    float result = 0.;
    if (_st.x < border) {
        result = 0.;
    }else {
        vec2 dist = _st-_mask;
        
        float radius = _r - t*_r;
        float r = smoothstep(radius-(radius*t),
                             radius+(radius*t),
                             dot(dist,dist)*4.0);
        result = 1.-r;
        //result = step(r,rand(rand(_st.x)*rand(_st.y)));
    }
    return result;

}

void main (void) {
    vec2 uv = gl_FragCoord.xy/u_resolution.xx;
    
    
    vec4 color = texture2D(tex,gl_FragCoord.xy/u_resolution.xy);
    
    float c1 = circle(uv, u_mask1/u_resolution.xx, u_radius1 + abs(sin(u_time*2.5))*0.001, 0.4 , 0.);
    c1 = u_radius1 > 0. ? c1 : 0.;
    float c2 = circle(uv, u_mask2/u_resolution.xx, u_radius2 + abs(sin(u_time*2.5))*0.001, 0.4, 0.5);
    c2 = u_radius2 > 0. ? c2 : 0.;
    float c3 = circle(uv, u_beginLight1/u_resolution.xx, 0.001 + abs(sin(u_time))*0.001 , 0.2, 0.);
    float c4 = circle(uv, u_beginLight2/u_resolution.xx, 0.001 + abs(sin(u_time))*0.001 , 0.2, 0.5);
    
    color *=1.-(c1+c2);
    
    color.rgb +=c3;
    //color.rg += c3;
    
    color.rgba +=c4;
    //color.rg += c4;
   
    float t = sin(u_time*2.)*0.1 + 0.2;
    float c = circle(uv, u_mask1/u_resolution.xx,0.5*( u_radius1 + abs(sin(u_time*4.))*0.001 ),0.4, 0. );
    color.a +=0.4* c;
    color.r +=0.9* c;
    color.g +=0.6* c;
    
    
    
    t = sin((u_time+13. )*2.)*0.1 + 0.2;
    c = circle(uv, u_mask2/u_resolution.xx, 0.5*( u_radius2 + abs(sin(u_time*4.))*0.001 ),0.4 ,0.5);
    color.a +=0.5* c;
    color.r +=0.9* c;
    color.g +=0.6* c;
    
   // color.a -= 0.3;
    //color.rga *=vec3(0.8,0.5,0.6) + r;
   // color.r +=0.9* circle(uv, u_mask2/u_resolution.xx, u_radius*0.2,t );
   // color.g +=0.7* circle(uv, u_mask2/u_resolution.xx, u_radius*0.2,t );
    
    gl_FragColor = color;
}
