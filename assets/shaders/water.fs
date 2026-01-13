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
    
    vec3 dayColor = texelColor.rgb * colDiffuse.rgb;
    float dist = distance(fragPosition, viewPos);
    float lightIntensity = 1.0 - smoothstep(lightRadius, lightRadius + 3.0, dist);
    vec3 nightAmbient = dayColor * 0.02; 
    vec3 lampColor = dayColor * 0.6 + vec3(0.1, 0.1, 0.1) * lightIntensity;

    vec3 nightResult = mix(nightAmbient, lampColor, lightIntensity);

    vec3 finalRGB = mix(dayColor, nightResult, nightFactor);

    finalColor = vec4(finalRGB, texelColor.a * colDiffuse.a);
}