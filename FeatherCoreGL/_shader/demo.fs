#version 330
out vec4 FragColor;

in vec3 vertexColor;
in vec2 TexCoord;
in vec3 outPos;
in vec3 outNormal;

uniform sampler2D textureW1;
uniform sampler2D textureW2;
uniform int bWireMode;
uniform float rt;
uniform int opt;
uniform vec3 viewDirection;


void main()
{
    vec4 clr;
    if(0 != bWireMode)
    {
        FragColor = vec4(vertexColor, 1.0);
    }
    else
    {
        vec4 modelColor = rt * texture(textureW1, TexCoord) + (1.0 - rt) * texture(textureW2, TexCoord);

        float contrast = 0.25f;
        float globalIntensity = 0.38f - contrast;
        vec3 globalLight = vec3(1.0, 1.0, 1.0);
        float lightRatio = 1.0f;
        
        float ambientIntensity = 0.85f + (contrast * 2);
        vec3 lightColor = vec3(1.0, 1.0, 1.0) * ambientIntensity;
        vec3 lightPos = vec3(0, 3, 6);
        vec3 lightDir = normalize(lightPos - outPos);
        vec3 norm = normalize(outNormal);

        float diffuse = max(dot(norm, lightDir), 0.0);
        float texRatio = (1.0 - diffuse) * 0.5;
        vec3 diffuseColor = lightRatio * ((diffuse * lightColor) + (globalIntensity * globalLight));
        
        float aa = modelColor.a;
        vec3 vv = outPos;

        if(opt <= 1) {
            if(1 == opt)
            {
                aa = max(1.0 - dot(norm, viewDirection), 0);
            }
            FragColor = vec4(
                modelColor.r * (diffuseColor.r + (texRatio)),
                modelColor.g * (diffuseColor.g + (texRatio)),
                modelColor.b * (diffuseColor.b + (texRatio)),
                aa
            );
        } else {
            FragColor = vec4(TexCoord.x, TexCoord.y, 1.0, 1.0);
        }

    }
}