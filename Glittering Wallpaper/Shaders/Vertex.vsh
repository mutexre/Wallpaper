attribute vec3 coord;
attribute vec4 color;

varying lowp vec4 colorVarying;

uniform mat3 transform;

void main()
{
    vec3 transformedXYW = transform * coord;
    vec2 xy = transformedXYW.xy;
    
    gl_Position = vec4(xy, 0.0, 1.0);
    colorVarying = color;
    gl_PointSize = 4.0;
}
