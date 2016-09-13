varying lowp vec4 colorVarying;
varying highp vec2 uvVarying;

uniform sampler2D texture;

void main()
{
    lowp vec4 texColor = texture2D(texture, uvVarying);
    gl_FragColor = vec4(texColor.rgb, colorVarying.a);
}
