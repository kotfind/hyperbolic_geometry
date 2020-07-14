#version 400

#define LINE_WIDTH 5
#define POINT_SIZE 10

uniform float time;
uniform ivec2 resolution;

uniform vec3 circles[100];
uniform int circlesSize;

uniform vec2 points[100];
uniform int pointsSize;

uniform vec3 lines[100];
uniform int linesSize;

out vec4 fragColor;

float circle(in vec2 uv, in vec2 c, in float r) {
    float d = distance(uv, c);
    return step(r, d) * step(d, r + float(LINE_WIDTH) / float(min(resolution.x, resolution.y)));
}

float point(in vec2 uv, in vec2 c) {
    float d = distance(uv, c);
    return step(d, float(POINT_SIZE) / float(min(resolution.x, resolution.y)));
}

float line(in vec2 uv, in vec3 l) {
    float d = abs(l.x*uv.x + l.y*uv.y + l.z) / length(l.xy);
    return step(d, 0.5 * float(LINE_WIDTH) / float(min(resolution.x, resolution.y)));
}

void main() {
    vec2 uv = 2. * (gl_FragCoord.xy - 0.5*vec2(resolution)) / float(min(resolution.x, resolution.y));

    vec3 col = vec3(0., 0., pow(1. - length(uv), 15.));
    for (int i = 0; i < circlesSize; ++i) {
        float v = circle(uv, circles[i].xy, circles[i].z);
        col = max(col, vec3(v));
    }

    for (int i = 0; i < linesSize; ++i) {
        float v = line(uv, lines[i]);
        col = max(col, vec3(v));
    }

    for (int i = 0; i < pointsSize; ++i) {
        float v = point(uv, points[i]);
        col.g = max(col.g, v);
        col.rb = min(col.rb, vec2(1. - v));
    }

    if (length(uv) > 1.) col = vec3(0.);
    if (length(uv) > 1. && length(uv) < 1. + 0.005) col = vec3(1., 0., 0.);

    fragColor = vec4(col, 1.);
}
