#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout(location = 0) uniform float time;

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 UVs = vec2(gl_GlobalInvocationID.xy)/(gl_NumWorkGroups.xy*gl_WorkGroupSize.xy);

    vec2 c;
    c.x = (3.5*UVs.x) - 2.5;
    c.y = (3.0*UVs.y) - 1.5;

    vec3 px = vec3(1.0);
    float depth = 10;
    vec2 z = vec2(0.0);

    int i;
    for (i = 0; i <= int(depth); i++) {
        z = vec2(pow(z.x,2.0) - pow(z.y,2.0),(2.0*z.x*z.y)) + c;
        if (length(z) > 2.0) {
            float s = float(i)/depth;
            px = vec3(s);
            //px = vec3((cos(s*3.14)+1)*0.3, s, 1.5  -s);
            break;
        }
    }

    imageStore(screen,pixelCoords,vec4(px,1.0));
}