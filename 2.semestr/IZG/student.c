/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#include "student.h"
#include "transform.h"
#include "fragment.h"

#include <memory.h>
#include <math.h>
#include <stdio.h>


/*****************************************************************************
 * Globalni promenne a konstanty
 */
float cas;
/* Typ/ID rendereru (nemenit) */
const int           STUDENT_RENDERER = 1;

int sirka;
int vyska;
/* Materialy */
const S_Material    MAT_WHITE_AMBIENT  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_DIFFUSE  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_SPECULAR = { 1.0, 1.0, 1.0, 1.0 };


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej
 */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");
    
    /* inicializace default rendereru */
    renderer->base.type = STUDENT_RENDERER;
    renInit(&renderer->base);
    
    /* nastaveni ukazatelu na upravene funkce */
    /* napr. renderer->base.releaseFunc = studrenRelease; */
    renderer->base.releaseFunc = studrenRelease;
    renderer->base.projectTriangleFunc = studrenProjectTriangle;
    
    /* inicializace nove pridanych casti */
    renderer->textura = loadBitmap(TEXTURE_FILENAME, &sirka, &vyska);
    
    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{
    S_StudentRenderer * renderer;
    
    if( ppRenderer && *ppRenderer )
    {
        /* ukazatel na studentsky renderer */
        renderer = (S_StudentRenderer *)(*ppRenderer);
        
        /* pripadne uvolneni pameti */
        free(renderer->textura);
        
        /* fce default rendereru */
        renRelease(ppRenderer);
    }
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku s podporou texturovani
 * Upravte tak, aby se trojuhelnik kreslil s texturami
 * (doplnte i potrebne parametry funkce - texturovaci souradnice, ...)
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         S_Coords *t1, S_Coords *t2, S_Coords *t3,
                         double h1, double h2, double h3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    /* zaklad fce zkopirujte z render.c */
    
    int         minx, miny, maxx, maxy;
    int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int         s1, s2, s3;
    int         x, y, e1, e2, e3;
    double      alpha, beta, gamma, w1, w2, w3, z, xt, yt, jmenovatel;
    S_RGBA      col1, col2, col3, color;
    
    IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);
    
    /* vypocet barev ve vrcholech */
    col1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
    col2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
    col3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);
    
    /* obalka trojuhleniku */
    minx = MIN(x1, MIN(x2, x3));
    maxx = MAX(x1, MAX(x2, x3));
    miny = MIN(y1, MIN(y2, y3));
    maxy = MAX(y1, MAX(y2, y3));
    
    /* oriznuti podle rozmeru okna */
    miny = MAX(miny, 0);
    maxy = MIN(maxy, pRenderer->frame_h - 1);
    minx = MAX(minx, 0);
    maxx = MIN(maxx, pRenderer->frame_w - 1);
    
    /* Pineduv alg. rasterizace troj.
     hranova fce je obecna rovnice primky Ax + By + C = 0
     primku prochazejici body (x1, y1) a (x2, y2) urcime jako
     (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */
    
    /* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
    a1 = y1 - y2;
    a2 = y2 - y3;
    a3 = y3 - y1;
    b1 = x2 - x1;
    b2 = x3 - x2;
    b3 = x1 - x3;
    
    /* koeficient C */
    c1 = x1 * y2 - x2 * y1;
    c2 = x2 * y3 - x3 * y2;
    c3 = x3 * y1 - x1 * y3;
    
    /* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
    s1 = a1 * x3 + b1 * y3 + c1;
    s2 = a2 * x1 + b2 * y1 + c2;
    s3 = a3 * x2 + b3 * y2 + c3;
    
    if ( !s1 || !s2 || !s3 )
    {
        return;
    }
    
    /* normalizace, aby vzdalenost od primky byla kladna uvnitr trojuhelniku */
    if( s1 < 0 )
    {
        a1 *= -1;
        b1 *= -1;
        c1 *= -1;
    }
    if( s2 < 0 )
    {
        a2 *= -1;
        b2 *= -1;
        c2 *= -1;
    }
    if( s3 < 0 )
    {
        a3 *= -1;
        b3 *= -1;
        c3 *= -1;
    }
    
    /* koeficienty pro barycentricke souradnice */
    alpha = 1.0 / ABS(s2);
    beta = 1.0 / ABS(s3);
    gamma = 1.0 / ABS(s1);
    
    //pro studrenTextureValue
    S_RGBA studrenColor;
    
    /* vyplnovani... */
    for( y = miny; y <= maxy; ++y )
    {
        /* inicilizace hranove fce v bode (minx, y) */
        e1 = a1 * minx + b1 * y + c1;
        e2 = a2 * minx + b2 * y + c2;
        e3 = a3 * minx + b3 * y + c3;
        
        for( x = minx; x <= maxx; ++x )
        {
            if( e1 >= 0 && e2 >= 0 && e3 >= 0 )
            {
                /* interpolace pomoci barycentrickych souradnic
                 e1, e2, e3 je aktualni vzdalenost bodu (x, y) od primek */
                w1 = alpha * e2;
                w2 = beta * e3;
                w3 = gamma * e1;
                
                /* interpolace z-souradnice */
                z = w1 * v1->z + w2 * v2->z + w3 * v3->z;
                jmenovatel = w1 * (1 / h1) + w2 * (1 / h2) + w3 * (1 / h3);
                xt = (w1 * (t1->x / h1) + w2 * (t2->x / h2) + w3 * (t3->x / h3)) / jmenovatel;
                yt = (w1 * (t1->y / h1) + w2 * (t2->y / h2) + w3 * (t3->y / h3)) / jmenovatel;
                
                studrenColor = studrenTextureValue( (S_StudentRenderer *)pRenderer, xt, yt);
                
                /* interpolace barvy */
                color.red = ROUND2BYTE((w1 * col1.red + w2 * col2.red + w3 * col3.red) * (studrenColor.red / 255.0));
                color.green = ROUND2BYTE((w1 * col1.green + w2 * col2.green + w3 * col3.green) * (studrenColor.green / 255.0));
                color.blue = ROUND2BYTE((w1 * col1.blue + w2 * col2.blue + w3 * col3.blue) * (studrenColor.blue / 255.0));
                color.alpha = 255;
                
                /* vykresleni bodu */
                if( z < DEPTH(pRenderer, x, y) )
                {
                    PIXEL(pRenderer, x, y) = color;
                    DEPTH(pRenderer, x, y) = z;
                }
            }
            
            /* hranova fce o pixel vedle */
            e1 += a1;
            e2 += a2;
            e3 += a3;
        }
    }
}

/******************************************************************************
 * Vykresli i-ty trojuhelnik n-teho klicoveho snimku modelu
 * pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * Upravte tak, aby se model vykreslil interpolovane dle parametru n
 * (cela cast n udava klicovy snimek, desetinna cast n parametr interpolace
 * mezi snimkem n a n + 1)
 * i - index trojuhelniku
 * n - index klicoveho snimku (float pro pozdejsi interpolaci mezi snimky)
 */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, float n)
{
    /* zaklad fce zkopirujte z render.c */
    
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */
    S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;
    int         vertexOffset, normalOffset, vertexOffsetplus, normalOffsetplus; /* offset pro vrcholy a normalove vektory trojuhelniku */
    int         i0, i1, i2, in, i00, i11, i22, inn;             /* indexy vrcholu a normaly pro i-ty trojuhelnik n-teho snimku */
    float desetinacast, jednaminust;
    int celacast;
    double h1, h2, h3;
    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles) && n >= 0 );
    
    /* z modelu si vytahneme i-ty trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);
    
    /* ziskame offset pro vrcholy n-teho snimku */
    vertexOffset = (((int) n) % pModel->frames) * pModel->verticesPerFrame;
    
    /* ziskame offset pro normaly trojuhelniku n-teho snimku */
    normalOffset = (((int) n) % pModel->frames) * pModel->triangles->size;
    
    /* ziskame offset pro normaly trojuhelniku n+1-teho snimku */
    normalOffsetplus = (((int) n+1) % pModel->frames) * pModel->triangles->size;
    
    /* ziskame offset pro vrcholy n+1-teho snimku */
    vertexOffsetplus = (((int) n+1) % pModel->frames) * pModel->verticesPerFrame;
    
    /* indexy vrcholu pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    i0 = triangle->v[ 0 ] + vertexOffset;
    i1 = triangle->v[ 1 ] + vertexOffset;
    i2 = triangle->v[ 2 ] + vertexOffset;
    
    /* indexy vrcholu pro i-ty trojuhelnik n+1-teho snimku - pricteni offsetu */
    i00 = triangle->v[ 0 ] + vertexOffsetplus;
    i11 = triangle->v[ 1 ] + vertexOffsetplus;
    i22 = triangle->v[ 2 ] + vertexOffsetplus;
    
    /* index normaloveho vektoru pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    in = triangle->n + normalOffset;
    
    /* index normaloveho vektoru pro i-ty trojuhelnik n+1-teho snimku - pricteni offsetu */
    inn = triangle->n + normalOffsetplus;
    
    /* transformace vrcholu matici model*/
    // vzorec pro interpolaci (1-t) * p0 + (1-t) * p1
    celacast=(int)n;
    desetinacast=n;
    desetinacast-=celacast;
    jednaminust = 1 - desetinacast;
    S_Coords * p00 = cvecGetPtr(pModel->vertices, i00);
    S_Coords * p11 = cvecGetPtr(pModel->vertices, i11);
    S_Coords * p33 = cvecGetPtr(pModel->vertices, i22);
    S_Coords * n11 = cvecGetPtr(pModel->trinormals, inn);
    
    S_Coords * p0 = cvecGetPtr(pModel->vertices, i0);
    S_Coords * p1 = cvecGetPtr(pModel->vertices, i1);
    S_Coords * p3 = cvecGetPtr(pModel->vertices, i2);
    S_Coords * n1 = cvecGetPtr(pModel->trinormals, in);
    
    S_Coords interpolace1;
    S_Coords interpolace2;
    S_Coords interpolace3;
    
    //Interpolace
    interpolace1.x = jednaminust * (p0->x) + desetinacast * p00->x;
    interpolace1.y = jednaminust * (p0->y) + desetinacast * p00->y;
    interpolace1.z = jednaminust * (p0->z) + desetinacast * p00->z;
    
    interpolace2.x = jednaminust * (p1->x) + desetinacast * p11->x;
    interpolace2.y = jednaminust * (p1->y) + desetinacast * p11->y;
    interpolace2.z = jednaminust * (p1->z) + desetinacast * p11->z;
    
    interpolace3.x = jednaminust * (p3->x) + desetinacast * p33->x;
    interpolace3.y = jednaminust * (p3->y) + desetinacast * p33->y;
    interpolace3.z = jednaminust * (p3->z) + desetinacast * p33->z;
    
    trTransformVertex(&aa, &interpolace1);
    trTransformVertex(&bb, &interpolace2);
    trTransformVertex(&cc, &interpolace3);
    
    //Osvetlovaci model
    S_Coords * nor0 = cvecGetPtr(pModel->normals, i0);
    S_Coords * nor1 = cvecGetPtr(pModel->normals, i1);
    S_Coords * nor2 = cvecGetPtr(pModel->normals, i2);
    
    S_Coords * nor00 = cvecGetPtr(pModel->normals, i00);
    S_Coords * nor11 = cvecGetPtr(pModel->normals, i11);
    S_Coords * nor22 = cvecGetPtr(pModel->normals, i22);
    
    //Interpolace normal
    interpolace1.x = jednaminust * (nor0->x) + desetinacast * nor00->x;
    interpolace1.y = jednaminust * (nor0->y) + desetinacast * nor00->y;
    interpolace1.z = jednaminust * (nor0->z) + desetinacast * nor00->z;
    
    interpolace2.x = jednaminust * (nor1->x) + desetinacast * nor11->x;
    interpolace2.y = jednaminust * (nor1->y) + desetinacast * nor11->y;
    interpolace2.z = jednaminust * (nor1->z) + desetinacast * nor11->z;
    
    interpolace3.x = jednaminust * (nor2->x) + desetinacast * nor22->x;
    interpolace3.y = jednaminust * (nor2->y) + desetinacast * nor22->y;
    interpolace3.z = jednaminust * (nor2->z) + desetinacast * nor22->z;
    
    /* promitneme vrcholy trojuhelniku na obrazovku */
    h1 = trProjectVertex(&u1, &v1, &aa);
    h2 = trProjectVertex(&u2, &v2, &bb);
    h3 = trProjectVertex(&u3, &v3, &cc);
    
    /* pro osvetlovaci model transformujeme take normaly ve vrcholech */
    trTransformVector(&naa, &interpolace1);
    trTransformVector(&nbb, &interpolace2);
    trTransformVector(&ncc, &interpolace3);
    
    /* normalizace normal */
    coordsNormalize(&naa);
    coordsNormalize(&nbb);
    coordsNormalize(&ncc);
    
    //Interpolace normaly
    interpolace3.x = jednaminust * (n1->x) + desetinacast * n11->x;
    interpolace3.y = jednaminust * (n1->y) + desetinacast * n11->y;
    interpolace3.z = jednaminust * (n1->z) + desetinacast * n11->z;
    trTransformVector(&nn, &interpolace3);
    
    /* normalizace normaly */
    coordsNormalize(&nn);
    
    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }
    
    /* rasterizace trojuhelniku */
    studrenDrawTriangle(pRenderer,
                        &aa, &bb, &cc,
                        &naa, &nbb, &ncc,
                        &triangle->t[0], &triangle->t[1], &triangle->t[2], //pridano vrcholy pro texturovani
                        h1, h2, h3, //pridany homogeni souradnice
                        u1, v1, u2, v2, u3, v3
                        );
}

/******************************************************************************
 * Vraci hodnotu v aktualne nastavene texture na zadanych
 * texturovacich souradnicich u, v
 * Pro urceni hodnoty pouziva bilinearni interpolaci
 * Pro otestovani vraci ve vychozim stavu barevnou sachovnici dle uv souradnic
 * u, v - texturovaci souradnice v intervalu 0..1, ktery odpovida sirce/vysce textury
 */

S_RGBA studrenTextureValue( S_StudentRenderer * pRenderer, double u, double v )
{
    //Vzorec z wiki
    u = u * sirka - 0.5;
    v = v * vyska - 0.5;
    //Prohozeny souradny system
    double pom = u;
    u = v;
    v = pom;
    //Vypocet interpolace
    int x = floor(u);
    int y = floor(v);
    double u_ratio = u - x;
    double v_ratio = v - y;
    double u_opposite = 1 - u_ratio;
    double v_opposite = 1 - v_ratio;
    int i1 = x + sirka * y;
    int i2 = x + 1 + sirka * y;
    int i3 = x + sirka * (y+1);
    int i4 = x + 1 + sirka * (y+1);
    
    unsigned char red = (unsigned char)(pRenderer->textura[i1].red * u_opposite + pRenderer->textura[i2].red * u_ratio) * v_opposite + (pRenderer->textura[i3].red * u_opposite + pRenderer->textura[i4].red * u_ratio) * v_ratio;
    
    unsigned char green = (unsigned char)(pRenderer->textura[i1].green * u_opposite + pRenderer->textura[i2].green * u_ratio) * v_opposite + (pRenderer->textura[i3].green * u_opposite + pRenderer->textura[i4].green * u_ratio) * v_ratio;
    
    unsigned char blue = (unsigned char)(pRenderer->textura[i1].blue * u_opposite + pRenderer->textura[i2].blue * u_ratio) * v_opposite + (pRenderer->textura[i3].blue * u_opposite + pRenderer->textura[i4].blue * u_ratio) * v_ratio;
    
    return makeColor(red, green, blue);
}

/******************************************************************************
 ******************************************************************************
 * Funkce pro vyrenderovani sceny, tj. vykresleni modelu
 * Upravte tak, aby se model vykreslil animovane
 * (volani renderModel s aktualizovanym parametrem n)
 */

void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel)
{
    /* test existence frame bufferu a modelu */
    IZG_ASSERT(pModel && pRenderer);
    
    /* nastavit projekcni matici */
    trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);
    
    /* vycistit model matici */
    trLoadIdentity();
    
    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, pRenderer->scene_move_z);
    
    /* nejprve nastavime posuv cele sceny v rovine XY */
    trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);
    
    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(pRenderer->scene_rot_x);
    trRotateY(pRenderer->scene_rot_y);
    
    /* nastavime material */
    renMatAmbient(pRenderer, &MAT_WHITE_AMBIENT);
    renMatDiffuse(pRenderer, &MAT_WHITE_DIFFUSE);
    renMatSpecular(pRenderer, &MAT_WHITE_SPECULAR);
    
    /* a vykreslime nas model (ve vychozim stavu kreslime pouze snimek 0) */
    renderModel(pRenderer, pModel, cas);
}

/* Callback funkce volana pri tiknuti casovace
 * ticks - pocet milisekund od inicializace */
void onTimer( int ticks )
{
    /* uprava parametru pouzivaneho pro vyber klicoveho snimku
     * a pro interpolaci mezi snimky */
    cas=ticks/100.0;
}

/*****************************************************************************
 *****************************************************************************/
