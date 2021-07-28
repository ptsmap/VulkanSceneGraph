#include <vsg/io/VSG.h>
static auto assimp_phong_frag = []() {std::istringstream str(
R"(#vsga 0.1.5
Root id=1 vsg::ShaderStage
{
  NumUserObjects 0
  stage 16
  entryPointName "main"
  module id=2 vsg::ShaderModule
  {
    NumUserObjects 0
    Source "#version 450
#extension GL_ARB_separate_shader_objects : enable
#pragma import_defines (VSG_DIFFUSE_MAP, VSG_EMISSIVE_MAP, VSG_LIGHTMAP_MAP, VSG_NORMAL_MAP, VSG_SPECULAR_MAP, VSG_TWOSIDED)

#ifdef VSG_DIFFUSE_MAP
layout(binding = 0) uniform sampler2D diffuseMap;
#endif

#ifdef VSG_NORMAL_MAP
layout(binding = 2) uniform sampler2D normalMap;
#endif

#ifdef VSG_LIGHTMAP_MAP
layout(binding = 3) uniform sampler2D aoMap;
#endif

#ifdef VSG_EMISSIVE_MAP
layout(binding = 4) uniform sampler2D emissiveMap;
#endif

#ifdef VSG_SPECULAR_MAP
layout(binding = 5) uniform sampler2D specularMap;
#endif

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelView;
} pc;

layout(binding = 10) uniform MaterialData
{
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 emissiveColor;
    float shininess;
    float alphaMask;
    float alphaMaskCutoff;
} material;

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 normalDir;
layout(location = 2) in vec2 texCoord0;
layout(location = 5) in vec3 viewDir;
layout(location = 6) in vec3 lightDir;

layout(location = 0) out vec4 outColor;

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
#ifdef VSG_NORMAL_MAP
    // Perturb normal, see http://www.thetenthplanet.de/archives/1180
    vec3 tangentNormal = texture(normalMap, texCoord0).xyz * 2.0 - 1.0;

    //tangentNormal *= vec3(2,2,1);

    vec3 q1 = dFdx(worldPos);
    vec3 q2 = dFdy(worldPos);
    vec2 st1 = dFdx(texCoord0);
    vec2 st2 = dFdy(texCoord0);

    vec3 N = normalize(normalDir);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
#else
    return normalize(normalDir);
#endif
}

vec3 computeLighting(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 emissiveColor, float shininess, float ambientOcclusion, vec3 ld, vec3 nd, vec3 vd)
{
    vec3 color = vec3(0.0);
    color.rgb += ambientColor;

    float diff = max(dot(ld, nd), 0.0);
    color.rgb += diffuseColor * diff;

    if (diff > 0.0)
    {
        vec3 halfDir = normalize(ld + vd);
        color.rgb += specularColor * pow(max(dot(halfDir, nd), 0.0), shininess);
    }

    vec3 result = color + emissiveColor;
    result *= ambientOcclusion;

    return result;
}

void main()
{
    vec4 ambientColor = material.ambientColor;
    vec4 diffuseColor = material.diffuseColor;
    vec4 specularColor = material.specularColor;
    vec4 emissiveColor = material.emissiveColor;
    float shininess = material.shininess;
    float ambientOcclusion = 1.0;

#ifdef VSG_DIFFUSE_MAP
    diffuseColor *= texture(diffuseMap, texCoord0.st);
#endif

    if (material.alphaMask == 1.0f)
    {
        if (diffuseColor.a < material.alphaMaskCutoff)
            discard;
    }

#ifdef VSG_EMISSIVE_MAP
    emissiveColor *= texture(emissiveMap, texCoord0.st);
#endif

#ifdef VSG_LIGHTMAP_MAP
    ambientOcclusion *= texture(aoMap, texCoord0.st).r;
#endif

#ifdef VSG_SPECULAR_MAP
    specularColor *= texture(specularMap, texCoord0.st);
#endif

    vec3 nd = getNormal();
    vec3 ld = normalize(lightDir);
    vec3 vd = normalize(viewDir);

    vec3 colorFrontFace = computeLighting(ambientColor.rgb, diffuseColor.rgb, specularColor.rgb, emissiveColor.rgb, shininess, ambientOcclusion, ld, nd, vd);
#ifdef VSG_TWOSIDED
    vec3 colorBackFace = computeLighting(ambientColor.rgb, diffuseColor.rgb, specularColor.rgb, emissiveColor.rgb, shininess, ambientOcclusion, ld, -nd, vd);
    outColor.rgb = colorFrontFace + colorBackFace;
#else
    outColor.rgb = colorFrontFace;
#endif
    outColor.a = diffuseColor.a;
}
"
    hints id=0
    SPIRVSize 1270
    SPIRV 119734787 65536 524298 175 0 131089 1 393227 1 1280527431 1685353262 808793134
     0 196622 0 1 720911 4 4 1852399981 0 26 127 131
     159 171 174 196624 4 7 196611 2 450 589828 1096764487 1935622738
     1918988389 1600484449 1684105331 1868526181 1667590754 29556 262149 4 1852399981 0 327685 9
     1316250983 1634562671 10348 983045 23 1886220131 1281717365 1952999273 677867113 993224310 993224310 993224310
     993224310 1715155302 1719024433 1719024435 1719024435 15155 393221 14 1768058209 1131703909 1919904879 0
     393221 15 1717987684 1130722165 1919904879 0 393221 16 1667592307 1918987381 1869377347 114
     393221 17 1936289125 1702259059 1869377347 114 327685 18 1852401779 1936027241 115 458757
     19 1768058209 1333030501 1970037603 1852795251 0 196613 20 25708 196613 21 25710
     196613 22 25718 327685 26 1836216174 1766091873 114 262149 31 1869377379 114
     262149 37 1717987684 0 262149 52 1718378856 7498052 262149 67 1970496882 29804
     393221 79 1768058209 1131703909 1919904879 0 393221 80 1702125901 1818323314 1635017028 0
     458758 80 0 1768058209 1131703909 1919904879 0 458758 80 1 1717987684 1130722165
     1919904879 0 458758 80 2 1667592307 1918987381 1869377347 114 458758 80 3
     1936289125 1702259059 1869377347 114 393222 80 4 1852401779 1936027241 115 393222 80
     5 1752198241 1935756641 107 458758 80 6 1752198241 1935756641 1953842027 6710895 327685
     82 1702125933 1818323314 0 393221 88 1717987684 1130722165 1919904879 0 393221 92
     1667592307 1918987381 1869377347 114 393221 96 1936289125 1702259059 1869377347 114 327685 100
     1852401779 1936027241 115 458757 105 1768058209 1333030501 1970037603 1852795251 0 196613 124
     25710 196613 126 25708 327685 127 1751607660 1919501428 0 196613 130 25718
     262149 131 2003134838 7498052 393221 134 1869377379 1869760114 1632007278 25955 262149 135
     1634886000 109 262149 138 1634886000 109 262149 141 1634886000 109 262149 144
     1634886000 109 262149 147 1634886000 109 262149 149 1634886000 109 262149 151
     1634886000 109 262149 153 1634886000 109 262149 155 1634886000 109 327685 159
     1131705711 1919904879 0 393221 168 1752397136 1936617283 1953390964 115 393222 168 0
     1785688688 1769235301 28271 393222 168 1 1701080941 1701402220 119 196613 170 25456
     327685 171 1819438967 1936674916 0 327685 174 1131963764 1685221231 48 262215 26
     30 1 327752 80 0 35 0 327752 80 1 35 16
     327752 80 2 35 32 327752 80 3 35 48 327752 80
     4 35 64 327752 80 5 35 68 327752 80 6 35
     72 196679 80 2 262215 82 34 0 262215 82 33 10
     262215 127 30 6 262215 131 30 5 262215 159 30 0
     262216 168 0 5 327752 168 0 35 0 327752 168 0
     7 16 262216 168 1 5 327752 168 1 35 64 327752
     168 1 7 16 196679 168 2 262215 171 30 0 262215
     174 30 2 131091 2 196641 3 2 196630 6 32 262167
     7 6 3 196641 8 7 262176 11 7 7 262176 12
     7 6 786465 13 7 11 11 11 11 12 12 11
     11 11 262176 25 1 7 262203 25 26 1 262187 6
     32 0 393260 7 33 32 32 32 131092 48 262167 77
     6 4 262176 78 7 77 589854 80 77 77 77 77
     6 6 6 262176 81 2 80 262203 81 82 2 262165
     83 32 1 262187 83 84 0 262176 85 2 77 262187
     83 89 1 262187 83 93 2 262187 83 97 3 262187
     83 101 4 262176 102 2 6 262187 6 106 1065353216 262187
     83 107 5 262165 113 32 0 262187 113 114 3 262187
     83 117 6 262203 25 127 1 262203 25 131 1 262176
     158 3 77 262203 158 159 3 262176 165 3 6 262168
     167 77 4 262174 168 167 167 262176 169 9 168 262203
     169 170 9 262203 25 171 1 262167 172 6 2 262176
     173 1 172 262203 173 174 1 327734 2 4 0 3
     131320 5 262203 78 79 7 262203 78 88 7 262203 78
     92 7 262203 78 96 7 262203 12 100 7 262203 12
     105 7 262203 11 124 7 262203 11 126 7 262203 11
     130 7 262203 11 134 7 262203 11 135 7 262203 11
     138 7 262203 11 141 7 262203 11 144 7 262203 12
     147 7 262203 12 149 7 262203 11 151 7 262203 11
     153 7 262203 11 155 7 327745 85 86 82 84 262205
     77 87 86 196670 79 87 327745 85 90 82 89 262205
     77 91 90 196670 88 91 327745 85 94 82 93 262205
     77 95 94 196670 92 95 327745 85 98 82 97 262205
     77 99 98 196670 96 99 327745 102 103 82 101 262205
     6 104 103 196670 100 104 196670 105 106 327745 102 108
     82 107 262205 6 109 108 327860 48 110 109 106 196855
     112 0 262394 110 111 112 131320 111 327745 12 115 88
     114 262205 6 116 115 327745 102 118 82 117 262205 6
     119 118 327864 48 120 116 119 196855 122 0 262394 120
     121 122 131320 121 65788 131320 122 131321 112 131320 112 262201
     7 125 9 196670 124 125 262205 7 128 127 393228 7
     129 1 69 128 196670 126 129 262205 7 132 131 393228
     7 133 1 69 132 196670 130 133 262205 77 136 79
     524367 7 137 136 136 0 1 2 196670 135 137 262205
     77 139 88 524367 7 140 139 139 0 1 2 196670
     138 140 262205 77 142 92 524367 7 143 142 142 0
     1 2 196670 141 143 262205 77 145 96 524367 7 146
     145 145 0 1 2 196670 144 146 262205 6 148 100
     196670 147 148 262205 6 150 105 196670 149 150 262205 7
     152 126 196670 151 152 262205 7 154 124 196670 153 154
     262205 7 156 130 196670 155 156 852025 7 157 23 135
     138 141 144 147 149 151 153 155 196670 134 157 262205
     7 160 134 262205 77 161 159 589903 77 162 161 160
     4 5 6 3 196670 159 162 327745 12 163 88 114
     262205 6 164 163 327745 165 166 159 114 196670 166 164
     65789 65592 327734 7 9 0 8 131320 10 262205 7 27
     26 393228 7 28 1 69 27 131326 28 65592 327734 7
     23 0 13 196663 11 14 196663 11 15 196663 11 16
     196663 11 17 196663 12 18 196663 12 19 196663 11 20
     196663 11 21 196663 11 22 131320 24 262203 11 31 7
     262203 12 37 7 262203 11 52 7 262203 11 67 7
     196670 31 33 262205 7 34 14 262205 7 35 31 327809
     7 36 35 34 196670 31 36 262205 7 38 20 262205
     7 39 21 327828 6 40 38 39 458764 6 41 1
     40 40 32 196670 37 41 262205 7 42 15 262205 6
     43 37 327822 7 44 42 43 262205 7 45 31 327809
     7 46 45 44 196670 31 46 262205 6 47 37 327866
     48 49 47 32 196855 51 0 262394 49 50 51 131320
     50 262205 7 53 20 262205 7 54 22 327809 7 55
     53 54 393228 7 56 1 69 55 196670 52 56 262205
     7 57 16 262205 7 58 52 262205 7 59 21 327828
     6 60 58 59 458764 6 61 1 40 60 32 262205
     6 62 18 458764 6 63 1 26 61 62 327822 7
     64 57 63 262205 7 65 31 327809 7 66 65 64
     196670 31 66 131321 51 131320 51 262205 7 68 31 262205
     7 69 17 327809 7 70 68 69 196670 67 70 262205
     6 71 19 262205 7 72 67 327822 7 73 72 71
     196670 67 73 262205 7 74 67 131326 74 65592
  }
  NumSpecializationConstants 0
}
)");
vsg::VSG io;
return io.read_cast<vsg::ShaderStage>(str);
};
