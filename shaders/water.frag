uniform samplerCube CubeMap;
varying vec3 normal, view, lightDir;

float waterEta = 0.67;

// amount of refraction
const float SPECULAR_EXP = 24.0;
const float SPECULAR_COEFF = 0.8;

void main (void) {
     vec3 nNormal = normalize(normal);
     vec3 light = normalize(lightDir);
     vec3 vView = view;
     
     if (dot(nNormal, vView) < 0.0) {
	   // this is above water
     } else {
	   // this is under water; invert normal
	   nNormal *= -1.0;
		 waterEta = 1.33;
	}

	vec3 reflected = normalize(reflect(vView, nNormal));
	vec4 flectColor = textureCube(CubeMap, reflected);

	vec3 refracted = refract(vView, nNormal, waterEta);
	vec4 fractColor = textureCube(CubeMap, refracted);

	const float R_0 = 0.4;
	float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-normalize(vView), nNormal)), 5.0);
	fresnel = max(0.0, min(fresnel, 1.0));

	gl_FragColor = mix(fractColor, flectColor, fresnel);

	// specular lighting
	float specular = max(dot(1.0 * reflected, light), 0.0);
	specular = pow(specular, SPECULAR_EXP) * SPECULAR_COEFF;

        gl_FragColor += vec4(specular, specular, specular, 1);

}
