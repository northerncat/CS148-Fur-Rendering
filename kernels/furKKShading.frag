/* CS148 Summer 2014
 * Final Project
 *
 * Name: Tsao-Wei (Brad) Huang
 * ---------------------------
 * This pair of shaders (furKKShading.vert and furKKShading.frag) uses a modified
 * version of Kajiya-Kay shading method for each fragment.
 */
 
// varying variables
varying vec3 N; // normal
varying vec3 T; // hair direction (default set as N)
varying vec3 v; // viewer direction
varying vec2 furCoord;
varying vec2 colorCoord;
varying float currLayer;

// unifrom variables
uniform sampler2D furImage;
uniform sampler2D colorTexture;

void main()
{

		vec4 Ka = vec4(0, 0, 0, 1);
		vec4 Kd = vec4(1, 1, 0, 1);
		vec4 Ks = vec4(1, 1, 0, 1);
		float Pd = 4.0;
		float Ps = 36.0;

		vec4 fur = texture2D(furImage, furCoord);
		vec4 color = texture2D(colorTexture, colorCoord);


		vec3 V = normalize(-v);
		vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
		for (int i = 0; i < gl_MaxLights; i++) {
			vec3 L = normalize(gl_LightSource[i].position.xyz - v);
			vec3 H = normalize(L + V);

			float u = clamp(dot(T, L), -1.0, 1.0);
			float v = clamp(dot(T, H), -1.0, 1.0);

			vec4 Ia = gl_LightSource[i].ambient;
			vec4 Id = gl_LightSource[i].diffuse;
			vec4 Is = gl_LightSource[i].specular;

			// ambient term
			finalColor += color * Ia;
			// diffuse term
			finalColor += pow(1.0 - u*u, Pd/2.0) * color * Id;
			// specular term
			finalColor += pow(1.0 - v*v, Ps/2.0) * color * Is;
		}

		if (fur.r < 1.0 || currLayer == 0.0) {
			finalColor.a = 1.0;
		} else {
			finalColor.a = 0.0;
		}

		if (currLayer != 0.0) {
			finalColor.rgb *= currLayer;
		} else {
			finalColor.rgb *= 0.2;
		}

		gl_FragColor = finalColor;
}