#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 uv;
flat in vec4 colorKind;
flat in vec4 parameters;
out vec4 fragmentColor;
uniform vec3 uCameraPosition;
uniform float uTime;
uniform float uAlarm;
uniform float uCooling;
uniform sampler2D uFloor;
uniform sampler2D uMonitor;
float hash(float v) {return fract(sin(v*127.1)*43758.5453);}
void main() {
    int kind=int(colorKind.w+0.5);
    // One shared pulse drives real light intensities AND emissive fixtures.
    float pulse=0.22+0.78*(0.5+0.5*sin(uTime*12.56637));
    vec3 lightColor=mix(vec3(0.48,0.72,1.0),vec3(1.0,0.035,0.018)*pulse,uAlarm);
    vec3 fogColor=mix(vec3(0.065,0.12,0.18),vec3(0.16,0.012,0.018)*pulse,uAlarm);
    vec3 albedo=colorKind.rgb;
    if(kind==1) albedo*=texture(uFloor,uv*parameters.yz).rgb;
    vec3 normal=normalize(worldNormal);
    vec3 viewDirection=normalize(uCameraPosition-worldPosition);
    vec3 result=albedo*mix(vec3(0.11,0.16,0.22),vec3(0.20,0.015,0.012),uAlarm);
    vec3 positions[3]=vec3[3](vec3(0,3.65,-5),vec3(0,3.65,0),vec3(0,3.65,5));
    for(int i=0;i<3;++i) {
        vec3 delta=positions[i]-worldPosition;
        float attenuation=1.0/(1.0+0.055*dot(delta,delta));
        vec3 lightDirection=normalize(delta);
        float diffuse=max(dot(normal,lightDirection),0.0);
        vec3 halfway=normalize(lightDirection+viewDirection);
        float specular=diffuse>0.0?pow(max(dot(normal,halfway),0.0),32.0):0.0;
        result+=(albedo*diffuse+vec3(0.16*specular))*lightColor*attenuation;
    }
    if(kind==2) {
        // Each LED has its own period, phase, duty cycle, and per-cycle state.
        float seed=parameters.x;
        float clock=uTime*(1.2+hash(seed)*5.0)+hash(seed+9.0)*20.0;
        float on=step(fract(clock),0.25+0.45*hash(seed+13.0));
        on*=step(0.22,hash(floor(clock)+seed*7.0));
        result=albedo*(0.08+1.8*on);
    }
    if(kind==3) result=lightColor*1.4;
    if(kind==4) {
        // Only the front face displays the graph; texture origin is top-left.
        if(normal.z>0.9) result=texture(uMonitor,vec2(uv.x,1.0-uv.y)).rgb;
        else result=vec3(0.015,0.025,0.04);
    }
    // More visible near the floor; no transparent geometry or sorting needed.
    float distanceToEye=length(uCameraPosition-worldPosition);
    // Alarm always adds haze, even when the user has turned normal cooling fog off.
    float heightFalloff=exp(-max(worldPosition.y,0.0)*1.8);
    float density=uCooling*(0.022+0.065*heightFalloff)
                 +uAlarm*(0.055+0.055*heightFalloff);
    float fog=1.0-exp(-density*distanceToEye);
    result=mix(result,fogColor,clamp(fog,0.0,0.85));
    result=vec3(1.0)-exp(-max(result,vec3(0.0))*1.35);
    fragmentColor=vec4(pow(result,vec3(1.0/2.2)),1.0);
}
