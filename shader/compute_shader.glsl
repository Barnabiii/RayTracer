#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 st = vec2(gl_GlobalInvocationID.xy)/(vec2(gl_NumWorkGroups.xy)*vec2(8,4));
    vec4 pxColor = vec4(st, 0.0, 1.0);
    
    imageStore(screen,pixelCoords,pxColor);
}