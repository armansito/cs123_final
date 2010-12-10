uniform samplerCube CubeMap;
varying vec3 normal, lightDir, r;

void main (void)
{
	vec4 final_color = textureCube( CubeMap, r);
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	float lambertTerm = dot(N,L);
	
	final_color += textureCube( CubeMap, r);

	/*
	if(lambertTerm > 0.0)
	{
		// Specular
	}
	*/
	
	gl_FragColor = final_color;
	//gl_FragColor = vec4(255, 0, 0, 255);	
}
