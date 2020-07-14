#version 400

#define LINE_WIDTH 5

uniform float time;
uniform ivec2 resolution;
uniform vec3 circles;

out vec4 fragColor;

float circle(in vec2 uv, in vec2 c, in float r) {
    float d = distance(uv, c);
    return step(r, d) * step(d, r + float(LINE_WIDTH) / float(min(resolution.x, resolution.y)));
}

void main() {
    vec2 uv = 2. * (gl_FragCoord.xy - 0.5*vec2(resolution)) / float(min(resolution.x, resolution.y));

    vec3 col = vec3(circle(uv, circles.xy, circles.z));
    if (length(uv) > 1. && length(uv) < 1. + 0.005) col = vec3(1., 0., 0.);

    fragColor = vec4(col, 1.);
}
