#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <time.h>
#include    <gx.h>

dc\_t   **dc\_screen;                      // 화면 Device Context**

int     view\_width;                     // 화면 폭
int     view\_height;                    // 화면 높이
int     half\_width;                     // 화면 폭의 절반 크기
int     half\_height;                    // 화면 높이의 절반 크기
int     center\_x;                       // 화면 중앙 x 좌표
int     center\_y;                       // 화면 중앙 y 좌표

void press\_any\_key( void)
{
> printf( "press any key......\n");
> getchar();
}

/ ----------------------------------------------------------------------------
> @brief 선 그리기 예제
**/
void test\_draw\_line( void)
{
> int     ndx;**

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));                              // 화면을 흑색으로 clear

> gx\_move\_to( dc\_screen, rand() % view\_width, rand() % view\_height);          // 임의의 좌표로 이동
> for ( ndx= 0; ndx < 100; ndx++)                                             // gxline\_to()를 이용하여 여러 개의 선을 출력
> {
> > dc\_screen->pen\_color  = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > gx\_line\_to( dc\_screen, rand() % view\_width, rand() % view\_height);

> }
}

void test\_get\_set\_pixel( void)
{
> color\_t	color;
> int     coor\_x;
> int     coor\_y;
> int     ndx;

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));                              // 화면을 흑색으로 clear

> // 화면의 왼쪽 반 부분에 임의의 선을 그림
> gx\_move\_to( dc\_screen, rand() % half\_width, rand() % view\_width);           // 임의의 좌표로 이동
> for ( ndx= 0; ndx < 100; ndx++)
> {
> > dc\_screen->pen\_color  = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > gx\_line\_to( dc\_screen, rand() % half\_width, rand() % view\_height);

> }
> // 화면 왼쪽 반을 오른쪽 반에 복사
> for ( coor\_y = 0; coor\_y < view\_height; coor\_y++)
> {
> > for ( coor\_x = 0; coor\_x < half\_width; coor\_x++)
> > {
> > > gx\_get\_pixel( dc\_screen, coor\_x           , coor\_y, &color);
> > > gx\_set\_pixel( dc\_screen, coor\_x+half\_width, coor\_y,  color);

> > }

> }
}

void  test\_rectangle( void)
{
> int     x1, y1, x2, y2;
> int     ndx;

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));
> dc\_screen->brush\_color   = gx\_color( 0, 0, 0, 0);

> for ( ndx= 0; ndx < 50; ndx++)
> {
> > x1  = rand() % view\_width;
> > y1  = rand() % view\_height;
> > x2  = rand() % view\_width;
> > y2  = rand() % view\_height;


> switch ( rand() % 3)
> {
> case 0   :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( 0, 0, 0, 0);
> > break;

> case 1   :
> > dc\_screen->pen\_color     = gx\_color( 0, 0, 0, 0);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> default  :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> }
> gx\_rectangle( dc\_screen, x1, y1, x2, y2);
> usleep( 50 **1000);                                                      // 50 msec
> }
}**

void  test\_circle( void)
{
> int      coor\_x, coor\_y;
> int      radius;
> int      ndx;

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));
> for ( ndx = 0; ndx < 50; ndx++)
> {
> > coor\_x         = rand() % view\_width;
> > coor\_y         = rand() % view\_height;
> > radius         = rand() % half\_width;


> switch ( rand() % 3)
> {
> case 0   :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( 0, 0, 0, 0);
> > break;

> case 1   :
> > dc\_screen->pen\_color     = gx\_color( 0, 0, 0, 0);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> default  :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> }
> gx\_circle( dc\_screen, coor\_x, coor\_y, radius);
> usleep( 50 **1000);
> }
}**

void test\_ellipse( void)
{
> int      coor\_x, coor\_y;
> int      width,  height;
> int      ndx;

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));
> for ( ndx = 0; ndx < 50; ndx++)
> {
> > coor\_x         = rand() % view\_width;
> > coor\_y         = rand() % view\_height;
> > width          = rand() % half\_width;
> > height         = rand() % half\_height;


> switch ( rand() % 3)
> {
> case 0   :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( 0, 0, 0, 0);
> > break;

> case 1   :
> > dc\_screen->pen\_color     = gx\_color( 0, 0, 0, 0);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> default  :
> > dc\_screen->pen\_color     = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > dc\_screen->brush\_color   = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > break;

> }
> gx\_ellipse( dc\_screen, coor\_x, coor\_y, width, height);
> usleep( 50 **1000);
> }
}**

void test\_bitmap( void)
{
> bmp\_t   **bmp;**

> bmp = gx\_bmp\_open( "bpp-01.bmp");
> if ( NULL == bmp)
> > gx\_print\_error( "bpp-01.bmp");                                         // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);
> > gx\_bmp\_close( bmp);
> > usleep( 300**1000);                                                     // 300 msec

> }

> bmp = gx\_bmp\_open( "bpp-04.bmp");
> if ( NULL == bmp)
> > gx\_print\_error( "bpp-04.bmp");                                         // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);
> > gx\_bmp\_close( bmp);
> > usleep( 300**1000);                                                     // 300 msec

> }

> bmp = gx\_bmp\_open( "bpp-08.bmp");
> if ( NULL == bmp)
> > gx\_print\_error( "bpp-08.bmp");                                         // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);
> > gx\_bmp\_close( bmp);
> > usleep( 300**1000);                                                     // 300 msec

> }

> bmp = gx\_bmp\_open( "bpp-24.bmp");
> if ( NULL == bmp)
> > gx\_print\_error( "bpp-24.bmp");                                         // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);
> > gx\_bmp\_close( bmp);
> > usleep( 300**1000);                                                     // 300 msec

> }

> bmp = gx\_bmp\_open( "bpp-32.bmp");
> if ( NULL == bmp)
> > gx\_print\_error( "bpp-32.bmp");                                         // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);
> > gx\_bmp\_close( bmp);
> > usleep( 300**1000);                                                     // 300 msec

> }
}

void test\_jpg( void)
{
> char    **jpg\_name = "jpeg-image.jpg";
> jpg\_t**jpg;

> jpg = gx\_jpg\_open( jpg\_name);
> if ( NULL == jpg)
> > gx\_print\_error( jpg\_name);                                              // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)jpg, 0, 0, jpg->width, jpg->height);
> > gx\_jpg\_close( jpg);

> }
}**

void test\_png( void)
{
> png\_t   **png;**

> png = gx\_png\_open( "png-08bpp.png");
> if ( NULL == png)
> > gx\_print\_error( "png-08bpp.png");                                       // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 100, ( dc\_t **)png, 0, 0, png->width, png->height);
> > gx\_png\_close( png);
> > usleep( 300**1000);                                                     // 300 msec

> }

> png = gx\_png\_open( "png-24bpp.png");
> if ( NULL == png)
> > gx\_print\_error( "png-24bpp.png");                                       // 실행 중 에러 내용을 출력

> else
> {
> > gx\_bitblt( dc\_screen, 0, 200, ( dc\_t **)png, 0, 0, png->width, png->height);
> > gx\_png\_close( png);
> > usleep( 300**1000);                                                     // 300 msec

> }
}

void test\_text( void)
{
> if ( gx\_set\_font( "nbold32.bdf"))
> > gx\_print\_error("nbold32.bdf");

> else
> {
> > dc\_screen->pen\_color = gx\_color( 0, 0, 0, 255);
> > gx\_text\_out( dc\_screen, 50 , 270, "FALINUX 포럼!!"  );

> }
}

void test\_list( void)
{
> image\_list\_t  **list;                // 이미지 리스트 객체 핸들
> png\_t**png\_cell;
> int            ndx;

> if ( !( list = gx\_imagelist\_create( "sample-list.png", 90, 90)))
> > gx\_print\_error( "gx\_imagelist\_create()");

> else
> {
> > for ( ndx = 0; ndx < list->count; ndx++)
> > {
> > > // 이미지 리스트에서 인덱스에 해당하는 이미지를 구함
> > > png\_cell = gx\_imagelist\_get( list, ndx);
> > > gx\_bitblt( dc\_screen, 200, 200,
> > > > (dc\_t **)png\_cell, 0, 0, png\_cell->width, png\_cell->height);

> > > usleep( 500**1000);                                                 // 500 msec

> > }
> > gx\_imagelist\_close( list);

> }
}

void test\_mosaic( void)
{
> mosaic\_t   **mosaic;
> int         ndx;**

> gx\_clear( dc\_screen, gx\_color( 0, 0, 0, 255));

> // 모자익 객체를 생성

> mosaic = gx\_mosaic\_create(  (dc\_t **)gx\_png\_open( "fore.png"),               // 평소에 출력되는 이미지
> > (dc\_t**)gx\_png\_open( "back.png"),               // 색상에 따라 출력될 이미지
> > (dc\_t **)gx\_png\_open( "color\_table.png"),        // 출력 영역을 선택하는 이미지
    1. 0, 100);          // 출력 대상 DC에 출력할 x, y 위치**


> // 이미지 내의 아이템별 좌표 정보를 등록

> gx\_mosaic\_add( mosaic,  0,  0, 30, 91,                                      // 아이템에 해당하는 이미지의 left, top, width, height
> > 8, 37);                                             // 해당 아이템의 색상을 구하는 x, y 좌표.

> gx\_mosaic\_add( mosaic, 23,  2, 59, 44, 40, 14);                             // 아이템 좌표는 계속 등록할 수 있다.
> gx\_mosaic\_add( mosaic, 60, 10, 36, 42, 75, 28);
> gx\_mosaic\_add( mosaic, 22, 37, 55, 47, 46, 62);
> gx\_mosaic\_add( mosaic, 31, 56, 65, 41, 83, 84);

> for ( ndx = 0; ndx < mosaic->count; ndx++)
> {
> > gx\_mosaic\_draw( dc\_screen, mosaic, ndx);
> > usleep( 500 **1000);                                                     // 500 msec

> }**

> gx\_mosaic\_close( mosaic);
}

void test\_layer( void)
{
> png\_t      **png;                    // PNG 이미지를 로드하기 위해 사용
> bmp\_t**bmp;                    // BmP 이미지를 로드하기 위해 사용
> layer\_t    **layer;                  // 레이어 객체
> int         ndx\_fish;               // 물고기 그림 레이어 인덱스 번호
> int         ndx\_chimpanzee;         // 침팬지 그림 레이어 인덱스 번호
> int         ndx\_lft\_button;         // 왼쪽 버튼의 평상 시 레이어 인덱스  번호
> int         ndx\_lft\_clicked;        // 왼쪽 버튼이 눌렸을 때 레이어 인덱스 번호
> int         ndx\_top\_button;         // 위쪽 버튼의 평상 시 레이어 인덱스 번호
> int         ndx\_top\_clicked;        // 위쪽 버튼이 눌렸을 때 레이어 인덱스 번호
> int         ndx\_rht\_button;         // 오른쪽 버튼의 평상 시 레이어 인덱스 번호
> int         ndx\_rht\_clicked;        // 오른쪽 버튼이 눌렸을 때 레이어 인덱스 번호
> int         ndx\_btm\_button;         // 아래쪽 버튼의 평상 시 레이어 인덱스 번호
> int         ndx\_btm\_clicked;        // 아래쪽 버튼이 눌렸을 때 레이어 인덱스 번호
> int         ndx;**

> // layer 객체 생성

> layer = gx\_layer\_create(  dc\_screen,            // 출력 대상 DC
> > 50 ,  25,             // Layer의 Screen 상의 위치
> > 300, 250);            // Layer의 폭과 넓이


> // 파일을 이용하여 레이어를 추가

> png = gx\_png\_open( "fish.png");             ndx\_fish        = gx\_layer\_add( layer, (dc\_t **)png,-100, 200, GX\_FALSE);
> png = gx\_png\_open( "chimpanzee.png");       ndx\_chimpanzee  = gx\_layer\_add( layer, (dc\_t**)png, 300,  10, GX\_FALSE);
> png = gx\_png\_open( "left.png");             ndx\_lft\_button  = gx\_layer\_add( layer, (dc\_t **)png,   5,  18, GX\_TRUE );
> png = gx\_png\_open( "left-clicked.png");     ndx\_lft\_clicked = gx\_layer\_add( layer, (dc\_t**)png,   5,  18, GX\_FALSE);
> png = gx\_png\_open( "top.png");              ndx\_top\_button  = gx\_layer\_add( layer, (dc\_t **)png,  18,   4, GX\_TRUE );
> png = gx\_png\_open( "top-clicked.png");      ndx\_top\_clicked = gx\_layer\_add( layer, (dc\_t**)png,  18,   4, GX\_FALSE);
> png = gx\_png\_open( "right.png");            ndx\_rht\_button  = gx\_layer\_add( layer, (dc\_t **)png, 110,  18, GX\_TRUE );
> png = gx\_png\_open( "right-clicked.png");    ndx\_rht\_clicked = gx\_layer\_add( layer, (dc\_t**)png, 110,  18, GX\_FALSE);
> png = gx\_png\_open( "bottom.png");           ndx\_btm\_button  = gx\_layer\_add( layer, (dc\_t **)png,  18, 107, GX\_TRUE );
> png = gx\_png\_open( "bottom-clicked.png");   ndx\_btm\_clicked = gx\_layer\_add( layer, (dc\_t**)png,  18, 107, GX\_FALSE);
> bmp = gx\_bmp\_open( "bpp-32.bmp");                             gx\_layer\_add( layer, (dc\_t **)bmp,   0,   0, GX\_TRUE);
> gx\_layer\_redraw( layer);                                                    // 레이어 전체를 출력
> usleep( 500000);**

> // 버튼을 갱신하면서 화면에 출력, 깜빡이는 현상이 발생

> gx\_layer\_visible( layer, ndx\_lft\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_lft\_clicked  , GX\_TRUE );
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_lft\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_lft\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_top\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_top\_clicked  , GX\_TRUE );
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_top\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_top\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_rht\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_rht\_clicked  , GX\_TRUE );
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_rht\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_rht\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_btm\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_btm\_clicked  , GX\_TRUE );
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_btm\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_btm\_clicked  , GX\_FALSE);

> // 버튼의 출력 상태가 변해도 수동으로 출력, 깜빡임이 없음

> gx\_layer\_manual( layer, GX\_TRUE);                                           // 레이어에 변화가 있어도 화면으로 출력하지 않음. gx\_layer)redraw()를 호출해 주어야 함

> gx\_layer\_visible( layer, ndx\_lft\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_lft\_clicked  , GX\_TRUE );
> gx\_layer\_redraw( layer);
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_lft\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_lft\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_top\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_top\_clicked  , GX\_TRUE );
> gx\_layer\_redraw( layer);
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_top\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_top\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_rht\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_rht\_clicked  , GX\_TRUE );
> gx\_layer\_redraw( layer);
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_rht\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_rht\_clicked  , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_btm\_button   , GX\_FALSE);
> gx\_layer\_visible( layer, ndx\_btm\_clicked  , GX\_TRUE );
> gx\_layer\_redraw( layer);
> usleep( 500 **1000);                                                         // 500 msec**

> gx\_layer\_visible( layer, ndx\_btm\_button   , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_btm\_clicked  , GX\_FALSE);
> gx\_layer\_redraw( layer);

> gx\_layer\_manual( layer, GX\_FALSE);                                  // 레이어에 변화가 있으면 바로 화면 갱신

> // 침팬지와 물고기를 이동

> gx\_layer\_visible( layer, ndx\_fish        , GX\_TRUE );
> gx\_layer\_visible( layer, ndx\_chimpanzee  , GX\_TRUE );
> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       ,  1, -1);
> > gx\_layer\_move( layer, ndx\_chimpanzee , -1,  0);

> }
> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       ,  1, 1);
> > gx\_layer\_move( layer, ndx\_chimpanzee , -1, -1);

> }
> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       , -1, 0);
> > gx\_layer\_move( layer, ndx\_chimpanzee ,  1, 1);

> }

> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       , 0, -1);
> > gx\_layer\_move( layer, ndx\_chimpanzee , 0,  1);

> }
> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       , -1,  0);
> > gx\_layer\_move( layer, ndx\_chimpanzee ,  0, -1);

> }
> for ( ndx = 0; ndx < 200; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       ,  1,  0);
> > gx\_layer\_move( layer, ndx\_chimpanzee ,  0,  1);

> }
> for ( ndx = 0; ndx < 300; ndx++)
> {
> > gx\_layer\_move( layer, ndx\_fish       ,  0,  1);
> > gx\_layer\_move( layer, ndx\_chimpanzee ,  1,  1);

> }

> gx\_layer\_move\_to( layer, ndx\_fish        ,  120,  100);
> gx\_layer\_move\_to( layer, ndx\_chimpanzee  ,   80,    0);

> gx\_layer\_close( layer);
}

void test\_gx\_open\_file( void)
{
> dc\_t       **dc\_comp;**

> dc\_comp = gx\_get\_compatible\_dc( dc\_screen);             // dc\_screen의 클론을 만듦

> gx\_open\_file( dc\_comp, "bpp-32.bmp");                   // 파일을 open
> gx\_bitblt( dc\_screen, 0, 0, dc\_comp, 0, 0, dc\_screen->width, dc\_screen->height);
> press\_any\_key();

> gx\_open\_file( dc\_comp, "jpeg-image.jpg");               // 파일을 open
> gx\_bitblt( dc\_screen, 0, 0, dc\_comp, 0, 0, dc\_screen->width, dc\_screen->height);
> press\_any\_key();

> gx\_open\_file( dc\_comp, "png-24bpp.png");                // 파일을 open
> gx\_bitblt( dc\_screen, 0, 0, dc\_comp, 0, 0, dc\_screen->width, dc\_screen->height);
> press\_any\_key();

> gx\_release\_dc( dc\_comp);
}

void test\_bitblt\_mask( void)
{
> dc\_t       **dc\_fore;                                                        // 평소 출력할 이미지
> dc\_t**dc\_back;                                                        // 색상이 선택되었을 때 출력할 이미지
> dc\_t       **dc\_mask;                                                        // mask 이미지**

> color\_t     color;

> dc\_fore = gx\_get\_compatible\_dc( dc\_screen);
> dc\_back = gx\_get\_compatible\_dc( dc\_screen);
> dc\_mask = gx\_get\_compatible\_dc( dc\_screen);

> gx\_open\_file( dc\_fore, "gx\_bitblt\_mask\_fore.bmp");
> gx\_open\_file( dc\_back, "gx\_bitblt\_mask\_back.bmp");
> gx\_open\_file( dc\_mask, "gx\_bitblt\_mask\_mask.bmp");

> gx\_bitblt( dc\_screen, 0, 0, dc\_fore, 0, 0, dc\_screen->width, dc\_screen->height);
> press\_any\_key();

> gx\_get\_pixel( dc\_mask, 200, 300, &color);

> gx\_bitblt\_mask( dc\_screen, dc\_back, dc\_mask, color, 24, 45, 530, 414);
> press\_any\_key();
> gx\_bitblt\_mask( dc\_screen, dc\_fore, dc\_mask, color, 24, 45, 530, 414);
> press\_any\_key();

> gx\_get\_pixel( dc\_mask, 365, 165, &color);

> gx\_bitblt\_mask( dc\_screen, dc\_back, dc\_mask, color, 333, 133, 82, 89);
> press\_any\_key();
> gx\_bitblt\_mask( dc\_screen, dc\_fore, dc\_mask, color, 333, 133, 82, 89);

> gx\_release\_dc( dc\_back);
> gx\_release\_dc( dc\_mask);
> gx\_release\_dc( dc\_fore);
}

void test\_gx\_get\_compatible\_dc( void)
/ ----------------------------------------------------------------------------
> @brief dc\_screen에 출력하기 전에 dc\_screen의 복사본을 만들어 놓고, 복사본에 축력을 모두 처리한 후에 한번에 화면으로 출력. test\_gx\_png\_create() 와는 달리 화면 깜빡임이 없다.
**/
{
> dc\_t**scr;                                        // dc\_screen 의 클론
> png\_t      **png;                                        // 파형 부분만 그리고 나머지는 투명 영역으로 이용
> bmp\_t**bmp;                                        // 배경 이미지용 bmp
> int         loop;
> int         ndx;                                        // 루프를 위한 인덱스

> scr = gx\_get\_compatible\_dc( dc\_screen);                 // dc\_screen의 클론을 만듬
> bmp = gx\_bmp\_open  ( "bpp-16.bmp");                     // 배경에 출력하기 위한 그림을 로딩
> png = gx\_png\_create( bmp->width, bmp->height);          // 투명 영역을 제공하기 위한 PNG 생성, BMP 크기로

> // 화면에 BMP 위에 선을 긋는 것을 10회 반복
> for ( loop = 0; loop < 10; loop++)
> {
> > gx\_clear( ( dc\_t **)png, gx\_color( 0, 0, 0, 0));**


> // 난수를 이용하여 여러 개의 직선을 PNG 에 그려서 PNG를 완성

> gx\_move\_to( ( dc\_t **)png, rand() % png->width, rand() % png->height);   // 난수로 특정 좌표로 이동
> for ( ndx = 0; ndx < 50; ndx++)
> {
> > png->pen\_color  = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > gx\_line\_to( (dc\_t**)png, rand() % png->width, rand() % png->height);

> }

> // 화면에 출력할 모든 내용을 우선 scr에 출력
> gx\_clear ( scr, gx\_color( 0, 0, 0, 255));                             // 화면을 깨끗하게 지움
> gx\_bitblt( scr, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);   // bmp를 화면에 출력
> gx\_bitblt( scr, 0, 0, ( dc\_t**)png, 0, 0, png->width, png->height);   // png를 화면에 출력

> // 모두 그려진 scr을 한번에 화면으로 출력. test\_gx\_png\_create()와는 달리 깜빡임이 없다
> gx\_bitblt( dc\_screen, 0, 0, scr, 0, 0, scr->width, scr->height);

> // 0.5초간 대기
> usleep( 500 **1000);
> }**

> gx\_png\_close( png);
> gx\_bmp\_close( bmp);
> gx\_release\_dc( scr);
}

void test\_gx\_png\_create( void)
/ ----------------------------------------------------------------------------
> @brief PNG를 파일없이 생성하여 투명영역을 이용하는 예를 보여줌
**/
{
> png\_t**png;                                        // 파형 부분만 그리고 나머지는 투명 영역으로 이용
> bmp\_t      **bmp;                                        // 배경 이미지용 bmp
> int         loop;
> int         ndx;                                        // 루프를 위한 인덱스**

> bmp = gx\_bmp\_open  ( "bpp-16.bmp");                     // 배경에 출력하기 위한 그림을 로딩
> png = gx\_png\_create( bmp->width, bmp->height);          // 투명 영역을 제공하기 위한 PNG 생성, BMP 크기로

> // 화면에 BMP 위에 선을 긋는 것을 10회 반복
> for ( loop = 0; loop < 10; loop++)
> {
> > gx\_clear( ( dc\_t **)png, gx\_color( 0, 0, 0, 0));         //**


> // 난수를 이용하여 여러 개의 직선을 PNG 에 그려서 PNG를 완성

> gx\_move\_to( ( dc\_t **)png, rand() % png->width, rand() % png->height);   // 난수로 특정 좌표로 이동
> for ( ndx = 0; ndx < 50; ndx++)
> {
> > png->pen\_color  = gx\_color( rand() %128 +128, rand() %128 +128, rand() %128 +128, 255);
> > gx\_line\_to( (dc\_t**)png, rand() % png->width, rand() % png->height);

> }

> // 화면에 배경 BMP를 출력하고, 다시 그 위에 선들이 그려져 있는 PNG를 출력
> gx\_clear ( dc\_screen, gx\_color( 0, 0, 0, 255));                             // 화면을 깨끗하게 지움
> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);   // bmp를 화면에 출력
> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t**)png, 0, 0, png->width, png->height);   // png를 화면에 출력

> // 0.5초간 대기
> usleep( 500 **1000);
> }**

> gx\_png\_close( png);
> gx\_bmp\_close( bmp);
}
/ ----------------------------------------------------------------------------
> @brief PNG의 투명 영역을 이용하여 좌로 흐르는 파형 그래프를 출력하는 예제
**/
void draw\_wave\_with\_create\_png( void)
{
  1. efine     WAVE\_WIDTH      10                          // 파형의 간격
> png\_t**png;                                        // 파형 부분만 그리고 나머지는 투명 영역으로 이용
> bmp\_t      **bmp;                                        // 배경 이미지용 bmp
> int         coor\_y;                                     // 파형을 그리기 위한 새로 구한 Y 좌표
> int         old\_y          = 0;                         // 이전 파형과 연결하여 그리기 위한 이전 Y 좌표
> int         ndx;                                        // 루프를 위한 인덱스**

> bmp = gx\_bmp\_open  ( "bpp-16.bmp");                     // 배경에 출력하기 위한 그림을 로딩
> png = gx\_png\_create( bmp->width, bmp->height);          // 투명 영역을 제공하기 위한 PNG 생성, BMP 크기로

> for( ndx = 0; ndx < 50; ndx++)                          // 파형 출력을 50까지
> {
> > coor\_y  = rand() %png->height;                      // 난수로 새(新) Y 좌표


> png->pen\_color  = gx\_color( 255, 255, 0, 255);
> gx\_line( ( dc\_t **)png,
> > png->width-WAVE\_WIDTH, old\_y,           // png 폭 - WAVE\_WIDTH, 이전   Y 좌표에서
> > png->width-1         , coor\_y);         // png 폭             , 새(新) Y 좌표까지 선을 긋기**



> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);   // bmp를 화면에 출력
> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t**)png, 0, 0, png->width, png->height);   // png를 화면에 출력


> // 다음 새(新) 파형을 오른쪽 끝에 그리기 위한 준비를 한다.
> // 참고로 이 과정에는 화면 출력이 없다. 즉, 화면에 변화가 없다
> // png의 오른쪽 부분을 왼쪽으로 이동
> gx\_bitblt(  ( dc\_t **)png, 0, 0,
> > ( dc\_t**)png, WAVE\_WIDTH, 0, png->width-WAVE\_WIDTH, png->height);


> // 옮긴 후에 새로 그린 선을 투명색으로 다시 그려 지운다
> png->pen\_color  = gx\_color( 0, 0, 0, 0);
> gx\_line( ( dc\_t **)png,
> > png->width-WAVE\_WIDTH, old\_y,           // png 폭 - WAVE\_WIDTH, 이전   Y 좌표에서
> > png->width-1         , coor\_y);         // png 폭             , 새(新) Y 좌표까지 선을 긋기**


> // 그래프 이동을 천천히
> old\_y   = coor\_y;                                   // 새로 구한 Y 좌표를 old\_y 로 대입, 다음 그래프에 사용
> usleep(  100 **1000);
> }
> gx\_png\_close( png);
> gx\_bmp\_close( bmp);
}**

void draw\_wave\_with\_clear\_area( void)
/ ----------------------------------------------------------------------------
> @brief gx\_clear\_area() 함수를 이용하여 좌로 흐르는 파형 그래프를 출력하는 예제
**/
{
  1. efine     WAVE\_WIDTH      10                          // 파형의 간격
> png\_t**png;                                        // 파형 부분만 그리고 나머지는 투명 영역으로 이용
> bmp\_t      **bmp;                                        // 배경 이미지용 bmp
> int         coor\_y;                                     // 파형을 그리기 위한 새로 구한 Y 좌표
> int         old\_y          = 0;                         // 이전 파형과 연결하여 그리기 위한 이전 Y 좌표
> int         ndx;                                        // 루프를 위한 인덱스**

> bmp = gx\_bmp\_open  ( "bpp-16.bmp");                     // 배경에 출력하기 위한 그림을 로딩
> png = gx\_png\_create( bmp->width, bmp->height);          // 투명 영역을 제공하기 위한 PNG 생성, BMP 크기로
> png->pen\_color  = gx\_color( 255, 255, 0, 255);          // 파형의 색깔을 노랑색으로

> for( ndx = 0; ndx < 50; ndx++)                          // 파형 출력을 50까지
> {
> > coor\_y  = rand() %png->height;                      // 난수로 새(新) Y 좌표


> gx\_line( ( dc\_t **)png,
> > png->width-WAVE\_WIDTH, old\_y,           // png 폭 - WAVE\_WIDTH, 이전   Y 좌표에서
> > png->width-1         , coor\_y);         // png 폭             , 새(新) Y 좌표까지 선을 긋기**


> old\_y   = coor\_y;                                   // 새로 구한 Y 좌표를 old\_y 로 대입, 다음 그래프에 사용


> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t **)bmp, 0, 0, bmp->width, bmp->height);   // bmp를 화면에 출력
> gx\_bitblt( dc\_screen, 0, 0, ( dc\_t**)png, 0, 0, png->width, png->height);   // png를 화면에 출력

> // 다음 새(新) 파형을 오른쪽 끝에 그리기 위한 준비를 한다.
> // 참고로 이 과정에는 화면 출력이 없다. 즉, 화면에 변화가 없다
> // png의 오른쪽 부분을 왼쪽으로 이동
> gx\_bitblt(  ( dc\_t **)png, 0, 0,
> > ( dc\_t**)png, WAVE\_WIDTH, 0, png->width-WAVE\_WIDTH, png->height);


> // 옮기고 남은 오른쪽 나머지 부분을 깨끗이 지운다.
> gx\_clear\_area( (dc\_t **)png,
> > png->width-WAVE\_WIDTH, 0,
> > png->width           , png->height,
> > gx\_color( 0, 0, 0, 0));

> // 그래프 이동을 천천히
> usleep(  100**1000);
> }
> gx\_png\_close( png);
> gx\_bmp\_close( bmp);
}

int   main( void)
/ ----------------------------------------------------------------------------
> @brief gxLib 사용 예제 메인 함수
**/
{
> if  ( gx\_init( "/dev/fb"))                                                  // gxLib 초기화
> > gx\_print\_error( "메인에서 호출");                                       // 실행 중 에러 내용을 출력

> else
> {
> > if ( !( dc\_screen = gx\_get\_screen\_dc()))                                // 화면 출력을 위한 스크린 DC 구함
> > > gx\_print\_error( "메인에서 호출");                                   // 실행 중 에러 내용을 출력

> > else
> > {
> > > srand( (unsigned)time(NULL)+(unsigned)getpid());**


> view\_width  = dc\_screen->width;                                     // 화면 폭
> view\_height = dc\_screen->height;                                    // 화면 높이
> half\_width  = view\_width  / 2;                                      // 화면 폭의 절반 크기
> half\_height = view\_height / 2;                                      // 화면 높이의 절반 크기
> center\_x    = half\_width;                                           // 화면 중앙 x 좌표
> center\_y    = half\_height;                                          // 화면 중앙 y 좌표

> gx\_clear( dc\_screen, gx\_color( 255, 255, 255, 255));

> test\_draw\_line();               press\_any\_key();                    // 선 긋기 테스트
> test\_get\_set\_pixel();           press\_any\_key();                    // 점 색상 읽기 및 쓰기 테스트
> test\_rectangle();               press\_any\_key();                    // 사각형 그리기 테스트
> test\_circle();                  press\_any\_key();                    // 원 그리기 테스트
> test\_ellipse();                 press\_any\_key();                    // 타원 그리기 테스트
> test\_bitmap();                  press\_any\_key();                    // 비트맵 파일 출력
> test\_jpg();                     press\_any\_key();                    // JPG 파일 출력
> test\_png();                     press\_any\_key();                    // PNG 파일 출력
> test\_text();                    press\_any\_key();
> test\_list();                    press\_any\_key();
> test\_mosaic();                  press\_any\_key();                    // 모자이크 테스트
> test\_layer();                                                       // 레이어 테스트
> test\_gx\_open\_file();
> test\_bitblt\_mask();
> test\_gx\_get\_compatible\_dc();    press\_any\_key();                    // gx\_get\_compatible\_dc() 활용 예
> test\_gx\_png\_create();           press\_any\_key();
> draw\_wave\_with\_create\_png();                                        // 파일 없이 PNG 이미지를 만들어 사용하는 예
> draw\_wave\_with\_clear\_area();                                        // 특정 영역을 지우는 gx\_clear\_area()를 이용하는 예
> gx\_release\_dc( dc\_screen);
> }
> gx\_close();
> }
> return   0;
}