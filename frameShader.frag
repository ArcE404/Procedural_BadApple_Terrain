#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 u_resolution;
uniform float u_time;

vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

void main()
{
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= (u_resolution.x/u_resolution.y);
    vec3 color = vec3(0.0);
	float f  = 0.0;
    vec2 uv = vec2(st*10);
    uv.xy += u_time * 5;
     uv *= .1;
    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
	f  = 0.5000*noise( uv ); uv = m*uv;
	f += 0.2500*noise( uv ); uv = m*uv;
	f += 0.1250*noise( uv ); uv = m*uv;
	f += 0.0625*noise( uv ); uv = m*uv;
	f += 0.0425*noise( uv ); uv = m*uv;
	//f += 0.0215*noise( uv ); uv = m*uv;
    f = abs(f);
    f = f * -1.0;
    //f = f * 0.5;
    f = f*1;
    color = vec3( f*.9+.4 );

    FragColor = texture(screenTexture, -TexCoords) * vec4(color,1);
}