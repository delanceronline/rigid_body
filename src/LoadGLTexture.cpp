//#include <gl\glu.h>			// Header File For The GLu32 Library

GLuint LoadGLTexture( const char *filename )			// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage;							// Create Storage Space For The Texture
	GLuint texture = 0;

	pImage = auxDIBImageLoad(filename);

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ( pImage != NULL && pImage->data != NULL )		// If Texture Image Exists
	{
		glGenTextures(1, &texture);						// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImage->sizeX, pImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		free(pImage->data);								// Free The Texture Image Memory
		free(pImage);									// Free The Image Structure
	}

	return texture;										// Return The Status
}