attribute vec3 coord;
attribute vec4 color;

varying lowp vec4 colorVarying;

uniform mat3 transform;

void main() {
    vec3 transformedXYW = transform * coord;
    vec2 xy = transformedXYW.xy;
    
    float alpha = 1.0;
    colorVarying = vec4(color.xyz, alpha);
    
    gl_Position = vec4(xy, 0.0, 1.0);
}
