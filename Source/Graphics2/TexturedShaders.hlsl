cbuffer ConstantBuffer
{
    float4x4 completeTransformation;
    float4x4 worldTransformation;    
	float4 cameraPosition;
    float4 lightVector;		// the light's vector
    float4 lightColor;		// the light's color
    float4 ambientColor;	// the ambient light's color
    float4 diffuseColor;	// The diffuse color (reflection cooefficient)
	float4 specularColor;	// The specular color (reflection cooefficient)
	float  shininess;		// The shininess factor
	float3 padding;
}

Texture2D Texture;
SamplerState ss;

struct VertexShaderInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
};

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
    float4 Color : COLOR;
	float2 TexCoord : TEXCOORD0;
    float4 ViewDirection : TEXCOORD1;
};

PixelShaderInput VShader(VertexShaderInput vin)
{
    PixelShaderInput output;
	float4 homogenisedInputPosition = float4(vin.Position, 1.0f);
	
    output.Position = mul(completeTransformation, homogenisedInputPosition);
	output.ViewDirection = normalize(cameraPosition - mul(worldTransformation, homogenisedInputPosition));

    // set the ambient light
    output.Color = ambientColor * diffuseColor;

    // calculate the diffuse light and add it to the ambient light

	float4 vectorBackToLight = -lightVector;
    float4 adjustedNormal = normalize(mul(worldTransformation, float4(vin.Normal, 1.0f)));
    float diffusebrightness = saturate(dot(adjustedNormal, vectorBackToLight));
    output.Color += vin.Color * lightColor * diffusebrightness;

	output.TexCoord = vin.TexCoord;
	output.Normal = adjustedNormal;

    return output;
}

float4 PShader(PixelShaderInput input) : SV_TARGET
{
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float lightIntensity = saturate(dot(input.Normal, lightVector));
	if (lightIntensity > 0.0f)
	{
		float4 reflection = normalize(2 * lightIntensity * input.Normal - lightVector); 
		specular = specularColor * pow(saturate(dot(reflection, input.ViewDirection)), shininess);
	}
	float4 color = input.Color * Texture.Sample(ss, input.TexCoord);
	color = saturate(color + specular);
	return color;
}