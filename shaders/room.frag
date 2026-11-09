#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
out vec4 fragmentColor;
uniform vec3 uColor;
uniform vec3 uCameraPosition;
uniform float uEmission;
void main() {
    vec3 normal = normalize(worldNormal);
    vec3 viewDirection = normalize(uCameraPosition - worldPosition);
    vec3 lightPosition[2] = vec3[2](vec3(0.0, 3.7, 4.0), vec3(0.0, 3.7, -4.0));
    vec3 result = uColor * vec3(0.18, 0.23, 0.32);
    for (int i = 0; i < 2; ++i) {
        vec3 delta = lightPosition[i] - worldPosition;
        vec3 lightDirection = normalize(delta);
        float attenuation = 1.0 / (1.0 + 0.035 * dot(delta, delta));
        float diffuse = max(dot(normal, lightDirection), 0.0);
        vec3 halfway = normalize(lightDirection + viewDirection);
        float specular = diffuse > 0.0 ? pow(max(dot(normal, halfway), 0.0), 32.0) : 0.0;
        result += (uColor * diffuse + vec3(specular * 0.2)) * vec3(0.45, 0.68, 1.0) * attenuation;
    }
    result = mix(result, uColor, uEmission);
    fragmentColor = vec4(pow(max(result, vec3(0.0)), vec3(1.0 / 2.2)), 1.0);
}
