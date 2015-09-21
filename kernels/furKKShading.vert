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
	v = vec3(gl_ModelViewMatrix * gl_Vertex);
	N = normalize(gl_NormalMatrix * gl_Normal);
	T = N;

	furCoord = gl_MultiTexCoord0.st;
	colorCoord = gl_MultiTexCoord1.st;
	currLayer = gl_FogCoord;

	gl_Position = ftransform();
}