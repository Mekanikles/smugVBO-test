#include "glee.h"
#include "GL/glfw.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "math.h"


float myRandom(float i)
{
    return i * ((float)rand() / (float)RAND_MAX);
}

typedef struct Box
{
    float rot;
    float x;
    float y;
    float size;
    struct Box* next;
} Box;

Box* Box_new(float x, float y, float rot, float size)
{
    Box* ret = (Box*)malloc(sizeof(Box));
    ret->rot = rot;
    ret->x = x;
    ret->y = y;
    ret->size = size;
    ret->next = NULL;
    return ret;
}

typedef struct Layer
{
    Box* first;
} Layer;

void Layer_addBox(Layer* l, Box* b)
{
    b->next = l->first;
    l->first = b;
}

void printGLError()
{
    int err = glGetError();

    switch (err)
    {
        case GL_INVALID_ENUM: 
            fprintf(stderr, "GL error GL_INVALID_ENUM\n");
            break;
        case GL_INVALID_VALUE: 
            fprintf(stderr, "GL error GL_INVALID_VALUE\n");
            break;
        case GL_INVALID_OPERATION: 
            fprintf(stderr, "GL error GL_INVALID_OPERATION\n");
            break;
        case GL_STACK_OVERFLOW: 
            fprintf(stderr, "GL error GL_STACK_OVERFLOW\n");
            break;
        case GL_STACK_UNDERFLOW: 
            fprintf(stderr, "GL error GL_STACK_UNDERFLOW\n");
            break;
        case GL_OUT_OF_MEMORY: 
            fprintf(stderr, "GL error GL_OUT_OF_MEMORY\n");
            break;   
        default:
            break;
    }
}

#define LAYERS 20
#define BOXES 5000
#define WIDTH 640
#define HEIGHT 480

int done = 0;
Layer layer[LAYERS];

int g_fVBOSupported = 0;
GLuint vboId_vertex[LAYERS]; 
GLuint vboId_color[LAYERS]; 
GLuint vboId_texture[LAYERS]; 
GLfloat vertices[BOXES*4*2];
GLfloat colors[BOXES*4*4];
GLfloat texcoords[BOXES*4*4];
GLuint texture;

void initVBO()
{
	//g_fVBOSupported = IsExtensionSupported( "GL_ARB_vertex_buffer_object" );
	if(GLEE_ARB_vertex_buffer_object)
	{   
        g_fVBOSupported = 1;
        
        int i;
        for (i = 0; i < LAYERS; i++)
        {

            glGenBuffersARB(1, &vboId_vertex[i]); 
            if (glIsBufferARB(vboId_vertex[i]))
            {
                fprintf(stderr, "created buffer %i\n", i);
            }
            else
            {
                fprintf(stderr, "failed to create buffer, err: %i\n",  glGetError()); 
            }
            
            glGenBuffersARB(1, &vboId_texture[i]); 
            if (glIsBufferARB(vboId_texture[i]))
            {
                fprintf(stderr, "created buffer %i\n", i);
            }
            else
            {
                fprintf(stderr, "failed to create buffer, err: %i\n",  glGetError()); 
            }
            
            glGenBuffersARB(1, &vboId_color[i]); 
            if (glIsBufferARB(vboId_vertex[i]))
            {
                fprintf(stderr, "created buffer %i\n", i);
            }
            else
            {
                fprintf(stderr, "failed to create buffer, err: %i\n",  glGetError()); 
            }
            
        }
	}
    else
    {
        fprintf(stderr, "Warning, VBOs not supported!\n");
    }
   
}



void renderRetained()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();



    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);	
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);	   
    
    float rotx = 0;
    float roty = 0;
    float rot = 0;
    
    int i = 0;
    for (i = 0; i < LAYERS; i++)
    {  
        int j = 0;
        Box* p = layer[i].first;
        while(p != NULL)
        {
            rot = p->rot * (3.1456f / 180.0f);
            rotx = p->x * cosf(rot) + p->y * sinf(rot);
            roty = - (p->y * sinf(rot)) + p->x * cosf(rot);
        
            vertices[j * 8 + 0] = rotx - p->size;  
            vertices[j * 8 + 1] = roty - p->size;
            vertices[j * 8 + 2] = rotx - p->size;      
            vertices[j * 8 + 3] = roty + p->size;       
            vertices[j * 8 + 4] = rotx + p->size;
            vertices[j * 8 + 5] = roty + p->size;
            vertices[j * 8 + 6] = rotx + p->size;
            vertices[j * 8 + 7] = roty - p->size;
            
            texcoords[j * 8 + 0] = 0;  
            texcoords[j * 8 + 1] = 1;
            texcoords[j * 8 + 2] = 0;      
            texcoords[j * 8 + 3] = 0;       
            texcoords[j * 8 + 4] = 1;
            texcoords[j * 8 + 5] = 0;
            texcoords[j * 8 + 6] = 1;
            texcoords[j * 8 + 7] = 1;
            
            int k;
            for (k = 0; k <4; k++)
            {   
                colors[j * 16 + k * 4 + 0] = (float)((i + 6) * (j+1))/(float)(LAYERS*BOXES);    
                colors[j * 16 + k * 4 + 1] = (float)((i + 4) * (j+1))/(float)(LAYERS*BOXES);    
                colors[j * 16 + k * 4 + 2] = (float)((i + 1) * (j+1))/(float)(LAYERS*BOXES);    
                colors[j * 16 + k * 4 + 3] = 0.2;                  
            }
            
            j++;
            p->rot+=(float)((i + 1) * (j+1))/(float)(LAYERS*BOXES);
            p = p->next;
        }   
        
        if( g_fVBOSupported )
        {    
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId_vertex[i]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, BOXES*4*2*sizeof(float), vertices, GL_STATIC_DRAW_ARB );
            glVertexPointer( 2, GL_FLOAT, 0, (char*) NULL );

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId_texture[i]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, BOXES*4*2*sizeof(float), texcoords, GL_STATIC_DRAW_ARB );
            glTexCoordPointer( 2, GL_FLOAT, 0, (char*) NULL );
            
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId_color[i]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, BOXES*4*4*sizeof(float), colors, GL_STATIC_DRAW_ARB );
            glColorPointer(4, GL_FLOAT, 0, (char*) NULL );
         
        } else
        {
            glVertexPointer( 2, GL_FLOAT, 0, vertices );	// Set The Vertex Pointer To Our Vertex Data
            glTexCoordPointer( 2, GL_FLOAT, 0, texcoords );
            glColorPointer(4, GL_FLOAT, 0, colors);
        }
             
        glDrawArrays( GL_QUADS, 0, BOXES*4 );      
    }
        

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);	    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);	   
    
  /*  
    Box* p = layer[0].first;
    glBegin(GL_QUADS);
    {    
        glVertex2f(vertices[0], vertices[1]);
        glVertex2f(vertices[2], vertices[3]);     
        glVertex2f(vertices[4], vertices[5]);
        glVertex2f(vertices[6], vertices[7]);
    }
    */
}

void renderImmediate()
{
    glClear(GL_COLOR_BUFFER_BIT);
  
    glLoadIdentity();
    
    int j;
    int i;
    for (i = 0; i < LAYERS; i++)
    {
        j = 0;
        Box* p = layer[i].first;
        while(p != NULL)
        {
            glPushMatrix();
            //glLoadIdentity();
            glColor4f((float)((i + 6) * (j+1))/(float)(LAYERS*BOXES), (float)((i + 4) * (j+1))/(float)(LAYERS*BOXES), (float)((i + 1) * (j+1))/(float)(LAYERS*BOXES), 0.2);
            //glTranslatef(p->x, p->y, 0.0f);     
            glRotatef(p->rot, 0.0f, 0.0f, 1.0f);    
            
            //fprintf(stderr, "pos: (%i, %i) rot: %i size: %i \n", (int)p->x, (int)p->y, (int)p->rot, (int)p->size);
            
            glBegin(GL_QUADS);
            {    
                glVertex2f(p->x - p->size, p->y - p->size); glTexCoord2f(0,1);
                glVertex2f(p->x- p->size, p->y + p->size);  glTexCoord2f(0,0);   
                glVertex2f(p->x + p->size,  p->y + p->size); glTexCoord2f(1,0);
                glVertex2f(p->x + p->size, p->y - p->size); glTexCoord2f(1,1);
            }
            glEnd();
            glPopMatrix();
            
            p->rot+=(float)((i + 1) * (j+1))/(float)(LAYERS*BOXES);
            j++;
            p = p->next;
        }
    }
}

void generateLayers()
{
    int i;
    for (i = 0; i < LAYERS; i++)
        layer[i].first = NULL;

    int j;
    for (i = 0; i < LAYERS; i++)
    {
        for (j = 0; j < BOXES; j++)
        {     
            Layer_addBox(&layer[i], Box_new(myRandom(WIDTH), myRandom(HEIGHT), myRandom(i*j) * 4, myRandom(5) + 2));
            //Layer_addBox(&layer[i], Box_new(0, 0, 0, 50));         
        }
    }
}

static int GLFWCALL closeWindowCallBack(void* data)
{
	done = 1;
	return GL_TRUE;
} 

void Graphics_init()
{
    fprintf(stderr, "gl errors: %i, %i, %i, %i, %i, %i\n",  GL_INVALID_ENUM,  GL_INVALID_VALUE, GL_INVALID_OPERATION, GL_STACK_OVERFLOW,  GL_STACK_UNDERFLOW,   GL_OUT_OF_MEMORY);

	glfwInit();
	glfwOpenWindow(WIDTH, HEIGHT, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
	glfwSetWindowCloseCallback(&closeWindowCallBack, NULL);
    glDisable(GL_CULL_FACE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,WIDTH,HEIGHT,0,-1, 1);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    printGLError();
    
    initVBO();
    printGLError();
    
    
    glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,texture);

    glfwLoadTexture2D("ball.tga", GLFW_BUILD_MIPMAPS_BIT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glEnable( GL_TEXTURE_2D );
    
    printGLError();
}

int main()
{

    fprintf(stderr, "Running smug VBO test.\n");
    
    Graphics_init();
   
    generateLayers();

    double time = glfwGetTime();
    double lastfps = time;
    int fps = 0;
    int rendermode = 0;

    while(!done)
    {
        time = glfwGetTime();
    
        if (glfwGetKey(GLFW_KEY_ESC))
        {
            done = 1;
        }
        if (glfwGetKey(GLFW_KEY_F1))
        {
            fprintf(stderr, "Rendermode: immediate\n");
            rendermode = 0;
        }
        if (glfwGetKey(GLFW_KEY_F2))
        {
            fprintf(stderr, "Rendermode: retained\n");
            rendermode = 1;
        }    
        
        
        if (rendermode)
            renderRetained();
        else
            renderImmediate();
    
        fps++;
        if (time - lastfps >= 1.0)
        {
            fprintf(stderr, "FPS: %i, objects: %i\n", (int)((double)fps /(time-lastfps)), LAYERS*BOXES);
            fps = 0;
            lastfps += 1.0;
        }
        glfwSwapBuffers();
    }

    glfwTerminate();
    
    fprintf(stderr, "Done.\n");
    
    return 0;
}
