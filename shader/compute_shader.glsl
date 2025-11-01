#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout(location = 0) uniform float time;
layout(location = 1) uniform vec4 section;
layout(location = 2) uniform vec4 mouse;

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 totalPixels = ivec2(gl_NumWorkGroups.xy*gl_WorkGroupSize.xy);
    vec2 offset = section.xy/vec2(totalPixels);
    vec2 scaling = section.zw/vec2(totalPixels) - offset;
    vec2 clickUV = mouse.xy/vec2(totalPixels);
    vec2 UVs = vec2(pixelCoords)/vec2(totalPixels);
    vec2 scaled_UVs = UVs*(scaling) + offset;

    vec2 c;
    c.x = (3.5*scaled_UVs.x) - 2.5;
    c.y = (3.0*scaled_UVs.y) - 1.5;

    vec3 px = vec3(0.0);
    float depth = floor(pow(time,3.0));
    vec2 z = vec2(0.0);

    int i;
    for (i = 0; i <= int(depth); i++) {
        z = vec2(pow(z.x,2.0) - pow(z.y,2.0),(2.0*z.x*z.y)) + c;
        if (length(z) > 2.0) {
            float s = float(i)/depth;
            px = vec3((cos(pow(1.4,s*8))+1)*0.3, s, 1.5  -s);
            break;
        }
    }

    if(length(UVs-clickUV) <= 0.002) {
        px = vec3(1.0) - px;
    }
    imageStore(screen,pixelCoords,vec4(px,1.0));
}