varying lowp vec4 colorVarying;
uniform sampler2D texture;

void main() {
    highp vec2 uv = 0.5 * (gl_FragCoord.xy + vec2(1.0, 1.0));
    gl_FragColor = texture2D(texture, uv); //colorVarying;
}
