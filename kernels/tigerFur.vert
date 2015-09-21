/* CS148 Summer 2014
 * Final Project
 * tigerFur.vert
 *
 * Name: Tsao-Wei (Brad) Huang
 * ---------------------------
 * This pair of shaders (tigerFur.vert and tigerFur.frag) is used to present 
 * fur generation on top of a texture providing color of the fragments.
 */

// varying variables
varying vec2 furCoord;			// fur texture coordinate of the vertex
varying vec2 colorCoord;		// color texture coordinate of the vertex
varying float currLayer;		// the index of layer the current vertex is in

// unifrom variables
uniform sampler2D furImage;		// fur texture with random cyan dots 
uniform sampler2D colorTexture;	// tiger color texture 

void main()
{
	// texture coordinate 0 stores the coordinate for the fur texture
	furCoord = gl_MultiTexCoord0.st;

	// texture coordinate 1 stores the coordinate for the color texture
	colorCoord = gl_MultiTexCoord1.st;

	// using fog coordinate to pass in the current layer's displacement
	currLayer = gl_FogCoord;

	gl_Position = ftransform();
}