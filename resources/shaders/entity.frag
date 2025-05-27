#version 330

in vec2 fragTexCoord;
in float texture_selection[10];
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 unlitTexelColor = texture(texture1, fragTexCoord);

    finalColor = mix(texelColor * fragColor, texelColor, unlitTexelColor.x);
}
