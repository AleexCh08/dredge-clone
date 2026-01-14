#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 viewPos;
uniform float nightFactor;
uniform float lightRadius;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 objectColor = texelColor.rgb * fragColor.rgb * colDiffuse.rgb;

    float dist = distance(fragPosition, viewPos);
    
    float lightIntensity = 1.0 - smoothstep(lightRadius, lightRadius + 3.0, dist);

    vec3 nightAmbient = objectColor * 0.15 + vec3(0.0, 0.02, 0.05); 
    vec3 lampColor = objectColor * 0.6 + vec3(0.1, 0.1, 0.1) * lightIntensity;

    vec3 nightResult = mix(nightAmbient, lampColor, lightIntensity);
    vec3 finalRGB = mix(objectColor, nightResult, nightFactor);

    vec3 fogColor = vec3(0.02, 0.02, 0.05); 
    float fogFactor = smoothstep(20.0, 80.0, dist); 

    float fogStrength = mix(0.0, 1.0, nightFactor);
    fogFactor *= fogStrength;
    fogFactor = clamp(fogFactor - lightIntensity, 0.0, 1.0);
    finalRGB = mix(finalRGB, fogColor, fogFactor);

    finalColor = vec4(finalRGB, texelColor.a * colDiffuse.a);
}