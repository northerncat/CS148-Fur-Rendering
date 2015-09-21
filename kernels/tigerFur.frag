/* CS148 Summer 2014
 * Final Project
 * tigerFur.frag
 *
 * Name: Tsao-Wei (Brad) Huang
 * ---------------------------
 * This pair of shaders (tigerFur.vert and tigerFur.frag) is used to present 
 * fur generation on top of a texture providing color of the fragments.
 */

// varying variables
varying vec2 furCoord;			// fur texture coordinate of the fragment
varying vec2 colorCoord;		// color texture coordinate of the fragment
varying float currLayer;		// the index of layer the current fragment is in

// unifrom variables
uniform sampler2D furImage;		// fur texture with random cyan dots 
uniform sampler2D colorTexture;	// tiger color texture 

void main()
{
		vec4 fur = texture2D(furImage, furCoord);
		vec4 color = texture2D(colorTexture, colorCoord);

		if (fur.r < 1.0 || currLayer == 0.0) {
			// make fur and the skin layer opaque
			color.a = 1.0;
		} else {
			// make non-fur area transparent
			color.a = 0.0;
		}

		// create a gradient lightest in the outermost layer, and darkest in the innermost layer
		if (currLayer != 0.0) {
			color.rgb *= sqrt(currLayer);
		}

		gl_FragColor = color;
}