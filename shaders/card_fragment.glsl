#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D cardTexture;
uniform vec3 cardColor = vec3(1.0, 1.0, 1.0);
uniform float alpha = 1.0;

void main()
{
    vec4 texColor = texture(cardTexture, TexCoord);
    
    // Apply card color tint
    vec3 finalColor = texColor.rgb * cardColor;
    
    // Add a border effect
    vec2 border = step(vec2(0.05), TexCoord) * (1.0 - step(vec2(0.95), TexCoord));
    float borderFactor = border.x * border.y;
    
    // Make border slightly darker
    finalColor = mix(finalColor * 0.7, finalColor, borderFactor);
    
    FragColor = vec4(finalColor, texColor.a * alpha);
}