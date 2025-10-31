#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout (location = 0) uniform float t;  

float shape(vec2 st, float radius) {
	st = vec2(0.5)-st;
    float r = length(st)*3.0;
    float a = atan(st.y,st.x);
    float m = sin(a+t*2.)*2.+1.;
    float f = radius;
    f += sin(a*25.)*.01;
    f += (sin(a*10.)*.015*m);
    return step(r,f);
}

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 st = vec2(gl_GlobalInvocationID.xy)/(vec2(gl_NumWorkGroups.xy)*vec2(8,4));
    float blob = shape(st,0.5);
    vec3 pxColor = vec3(0.52, 0.76, 1.0)*blob +vec3(0.0118, 0.149, 0.2824)*(1.-blob);
    
    imageStore(screen,pixelCoords,vec4(pxColor,1.0));
}