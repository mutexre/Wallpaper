attribute vec3 coord;
attribute vec2 pos;
attribute vec4 color;

varying lowp vec4 colorVarying;

uniform mat3 transform;
uniform float t;
uniform float speed;
uniform float columns;

void main()
{
    vec3 transformedXYW = transform * coord;
    vec2 xy = transformedXYW.xy;
    
    float row = 2.0 * pos.x;
    float col = pos.y;
    row += col;
    
    float currentColumn = ceil(abs(sin(speed * t)) * columns);
    float alpha = 0.5 * currentColumn / pos.x;
    
    /*if (mod(t, 2.0) == 0.0) {
        if (mod(row, 2.0) == 0.0)
            alpha = 0.0;
    }*/
    
    colorVarying = vec4(color.xyz, alpha);
    
    gl_Position = vec4(xy, 0.0, 1.0);
}
