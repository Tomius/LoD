#version 330

uniform sampler2DRect Tex;

out vec4 FragColor;

void main() {
    vec3 color;
    vec3 sum = vec3(0.0);
    ivec2 texcoord = ivec2(gl_FragCoord.xy);

    const int n = 2; // max distance in which pixels also get fetched
    for(int dx = -n; dx <= n; dx++) {
        for(int dy = -n; dy <= n; dy++) {
            sum += pow(texelFetch(Tex, texcoord + (2 * ivec2(dx, dy))).rgb, vec3(3.0));
        }
    }
    sum = sum * sum * 1 / ((2*n + 1) * (2*n + 1));
    vec3 texel = texelFetch(Tex, texcoord).rgb;

    float texelIntensity = length(texel) / sqrt(3.0);
    if(texelIntensity < 0.3) {
        color = (sum * 0.006 + texel).rgb;
    } else {
        if(texelIntensity < 0.5) {
            color = (sum * 0.0045 + texel).rgb;
        } else {
            color = (sum * 0.0035 + texel).rgb;
        }
    }
    // The bloom effect makes everything way too bright, let's compensate that
    FragColor = vec4(pow(color, vec3(1.5)), 1.0);
}
