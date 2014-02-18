/*
 * Copyright (c) 1999  Silicon Graphics, Inc.  All rights reserved.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
 * POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* texture.h - by David Blythe, SGI */

/* Simple SGI .rgb(a) image file loader routine. */

#include <GL/glut.h>

unsigned * read_texture(const char *name, int *width, int *height, int *components);
GLvoid initTexture( GLubyte *image, GLsizei imageWidth, GLsizei imageHeight );
GLvoid initSlimeTexture( GLubyte *image, GLsizei imageWidth, GLsizei imageHeight );
