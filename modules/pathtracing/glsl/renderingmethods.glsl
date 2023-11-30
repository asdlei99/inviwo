#include "random.glsl"
#include "transmittancemethods.glsl"
#include "util.glsl"

vec3 RMVolumeRender_SingleScatter_SingleLight(float rayStep, sampler3D volume, VolumeParameters volParam, sampler2D tf, 
    vec3 samplePos, vec3 cameraDir, LightParameters light, uint hashSeed, int rcChannel, float extinctionUpper) {
    
    vec3 result = vec3(0f);
    vec3 sampleWorldPos = (volParam.textureToWorld*vec4(samplePos,1f)).xyz;
    vec4 voxel = getNormalizedVoxel(volume, volParam, samplePos);
    
    vec4 tfSample = applyTF(tf, voxel);

    vec3 sampleAmbient = tfSample.rgb;
    vec3 sampleDiffuse = tfSample.rgb;
    vec3 sampleSpecular = tfSample.rgb;

    vec3 toLightTextureV = (volParam.worldToTexture*vec4(light.position, 1f)).xyz - samplePos;
    vec3 toLightTextureD = normalize(toLightTextureV);
    float t0 = 0.0f;
    float t1 = length(toLightTextureV);
    float tau = 1f;   

    RayBBIntersection_TextureSpace(samplePos, toLightTextureD, t0, t1);

    float meanfreepath_l = WoodcockTracking(samplePos, toLightTextureD, 0f, t1, hashSeed, 
        volume, volParam, tf, extinctionUpper, tau);
    float Tl = meanfreepath_l >= t1 ? 1.0f : 0.0f;     


    vec3 gradient = gradientCentralDiff(vec4(0f), volume, volParam, samplePos, 0);
    gradient = normalize(gradient);
    gradient *= sign(voxel[0] / (1.0 - volParam.formatScaling) - volParam.formatOffset);

    vec3 color = shadeBlinnPhong(light, sampleAmbient, sampleDiffuse, sampleSpecular,
        sampleWorldPos,
        gradient, // should it be minus?
        cameraDir);

    result = color*Tl;
    
    return result;
}