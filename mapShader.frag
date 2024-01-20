#version 410 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord;
in vec3 FragPosition;

uniform float uTexelSize;
uniform sampler2D heightMap;
uniform mat4 model;
uniform vec3 LigthPos;

void main()
{
     float HEIGHT_SCALE = (Height + 10)/37.0f;	// shift and scale the height into a grayscale value

     vec3 ligthPos = LigthPos;
     vec3 ligthColor = vec3(0.9,0.9,0.9);
     vec3 seaColor = vec3(0.15,0.88,0.92);
     vec3 landColor = vec3(HEIGHT_SCALE,HEIGHT_SCALE,HEIGHT_SCALE);

     float left  = texture(heightMap, TexCoord + vec2(-uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
     float right = texture(heightMap, TexCoord + vec2( uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
     float up    = texture(heightMap, TexCoord + vec2(0.0,  uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
     float down  = texture(heightMap, TexCoord + vec2(0.0, -uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
     vec3 norm = normalize(vec3(down - up, 2.0, left - right));

     float abientStrength = 0.1;
     vec3 ambient =  abientStrength * ligthColor;

     vec3 x = dFdx(FragPosition);
     vec3 y = dFdy(FragPosition);
     vec3 normal = inverse(mat3(model)) * normalize(cross(x, y));

     vec3 ligthDir = normalize(ligthPos - FragPosition);

     float diff = max(dot(normal, ligthDir), 0.0);
     vec3 diffuse = diff * ligthColor;

     vec3 resultLand = (ambient + diffuse) * landColor;
     vec3 resultSea = (ambient + diffuse) * seaColor;

    if(LigthPos != vec3(0)){
        if(HEIGHT_SCALE < 0.0001){
            FragColor = vec4(resultLand, 1.0);
        }else{
           FragColor = vec4(resultLand, 1.0);
        }
    }else{
        if(HEIGHT_SCALE < 0.0001){
            FragColor = vec4(seaColor, 1.0);
        }else{
           FragColor = vec4(landColor, 1.0);
        }
    }
    
}