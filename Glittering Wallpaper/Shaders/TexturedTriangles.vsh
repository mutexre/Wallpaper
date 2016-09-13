attribute vec3 coord;
attribute vec4 color;

varying lowp vec4 colorVarying;
varying highp vec2 uvVarying;

uniform mat3 transform;
uniform mat3 uvTransform;

void main()
{
    vec3 transformedXYW = transform * coord;
    vec2 xy = transformedXYW.xy;
        
    gl_Position = vec4(xy, 0.0, 1.0);
    colorVarying = color;
    
    uvVarying = (uvTransform * transformedXYW).xy;
    /*uvVarying = 0.5 * (xy + vec2(1.0, 1.0));
    uvVarying.y = -0.8 * uvVarying.y - 0.1;*/
}
