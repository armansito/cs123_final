uniform samplerCube CubeMap;
varying vec3 normal, lightDir, r, I;

void main (void)
{
	vec4 final_color = textureCube( CubeMap, r);
	float lambertTerm = dot(N,L);
	
	final_color += textureCube( CubeMap, r);

	/*
	if(lambertTerm > 0.0)
	{
		// Specular
	}
	*/

	float redEta = 0.84;
	float grnEta = 0.87; 
	float bluEta = 0.91;	

	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);

	vec3 reflected = reflect(I, N);
	vec3 refractedRed = refract(I, N, redEta);
	vec3 refractedGrn = refract(I, N, grnEta);
	vec3 refractedBlu = refract(I, N, bluEta);
	vec3 refracted = vec3(refractedRed[0], refractedGrn[1], refractedBlu[2]);

	vec4 flectColor = textureCube(CubeMap, reflected);
	vec4 fractColor = textureCube(CubeMap, refracted);

	gl_FragColor = flectColor;
}
