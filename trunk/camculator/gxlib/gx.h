#ifndef __GX_H__
#define __GX_H__

#include <fontinfo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GX_SUCCESS               0                                              // �Լ� ���� ����
#define GXERR_NONE               0                                              // ���� ����
#define GXERR_NO_DEVICE         -1                                              // ��ġ�� ����
#define GXERR_ACCESS_DEVICE     -2                                              // ���� ������ ����
#define GXERR_VSCREEN_INFO      -3                                              // FBIOGET_VSCREENINFO�� ������ ����
#define GXERR_FSCREEN_INFO      -4                                              // FBIOGET_FSCREENINFO�� ������ ����
#define GXERR_MEMORY_MAPPING    -5                                              // ������ ���� �޸� ���ο� ����
#define GXERR_NO_FILE           -6                                              // �̹��� ������ ����
#define GXERR_HEADER_INFO       -7                                              // �̹��� ��� ���� �̻�
#define GXERR_READ_FILE         -8                                              // �̹��� ������ �б� �̻�
#define GXERR_PALETTE_INFO      -9                                              // �ȷ�Ʈ ���� �̻�
#define GXERR_COLOR_DEPTH       -10                                             // Į�� ���̰� ǥ���� �ƴ�
#define GXERR_NO_ASSIGNED_FONT  -11                                             // ������ ��Ʈ �ڵ��� ����
#define GXERR_SIGNATURE         -12                                             // �ñ׳��� ����
#define GXERR_OUT_OF_MEMORY     -13                                             // �޸� ����
#define GXERR_PROCESSING        -14                                             // ó�� �� ���� �߻�
#define GXERR_NO_CANVAS         -15                                             // �̹��� ó���� ���� DC ������ ����
#define GXERR_FILE_NAME         -16                                             // ���� �̸� ����
                                
#define DCTYPE_SCREEN           1                                               // DC Type �� Screen DC ����
#define DCTYPE_BMP              2                                               // DC Type �� Bitmap ����
#define DCTYPE_PNG              3                                               // DC Type �� PNG ����
#define DCTYPE_JPG              4                                               // DC Type �� JPG ����
                                
#define GX_TRUE                 ( 1 == 1)
#define GX_FALSE                ( 0 == 1)

typedef struct color_t_ color_t;
struct color_t_
{
    unsigned char  red;
    unsigned char  green;
    unsigned char  blue;
    unsigned char  alpha;
};

typedef struct font_t_
{
    bdfFontCache    fontcache;
    FILE*           fontfile;
    bdfFontIndex    fndex[GX_FONT_INDEX_MAX];
} font_t;

typedef struct frame_buffer_t_ frame_buffer_t;
struct frame_buffer_t_
{
    int      fd;                                                                // ������ ���ۿ� ���� ���� ��ũ����
    int      width;                                                             // ��Ʈ ������ ��
    int      height;                                                            // ��Ʈ ������ ����
    int      dots;                                                              // ��ü ��Ʈ ���� width * height
    int      bytes;                                                             // �޸� ��ü ũ��
    int      colors;                                                            // Į�� ����, 1=1bit, 4=4bit, 8=8bit, ...
    int      bytes_per_line;                                                    // ���δ� ����Ʈ ����
    int      bits_per_pixel;                                                    // ��Ʈ�� �ȼ� ����

    unsigned short *mapped;                                                     // �޸� ���ε� ������
};

typedef struct dc_t_ dc_t;
struct dc_t_
{
    char        dc_type;                                                // DC�� ���·� Screen, Bitmap, JPEG, PNG�� �����Ѵ�.
    int         width;                                                  // ��Ʈ ������ ��
    int         height;                                                 // ��Ʈ ������ ����
    int         dots;                                                   // ��ü ��Ʈ ���� width * height
    int         bytes;                                                  // �޸��� ��ü Byte ũ��
    int         colors;                                                 // Į�� ����
    int         bytes_per_line;                                         // ���δ� ����Ʈ ����
    int         bits_per_pixel;                                         // ��Ʈ�� �ȼ� ����
    int         coor_x;                                                 // ������ �׸��� �ߴ� ������ ��ǥ
    int         coor_y;                                                 // ������ �׸��� �ߴ� ������ ��ǥ
    color_t     pen_color;                                              // ������ �� Į��
    color_t     brush_color;                                            // ������ �귯�� Į��
    color_t     font_color;                                             // �۾� ����
    font_t     *font;                                                   // ���ڿ� ����� ���� �۲� �ڵ�
    void       *mapped;                                                 // �޸� ���ε� ������

    void (*release_dc)( dc_t *dc);                                      // Device Context �Ҹ� �� ���� �޸𸮸� ����
    void (*clear    )( dc_t *dc, color_t color);                                  // ������ ��ü ĥ�ϱ�
    void (*get_pixel)( dc_t *dc, int coor_x, int coor_y, color_t   *color );      // Į�� ���� �о� ����
    void (*set_pixel)( dc_t *dc, int coor_x, int coor_y, color_t    color );      // �� ���
    void (*hline    )( dc_t *dc, int x1st  , int x_2nd , int coor_y, color_t color);// ���� �߱�
    void (*vline    )( dc_t *dc, int coor_x, int y_1st , int y_2nd , color_t color);// ������ �߱�
};

extern          frame_buffer_t  gx_fb;                                          // ������ ���� ����

extern char    *gx_error_string ( int error_code);                              // ���� �ڵ忡 ���� ���ڿ��� ����
extern void     gx_print_error  ( int error_code, char *remark);                // ���� ������ �μ� ���ڿ� �����Ͽ� ���
extern void     gx_printf_error ( int error_code, const char *fmt, ... );       // ���� ������ ���� ���ڿ� �����Ͽ� ���
extern int      gx_open         ( char *dev_name);                              // �׷��� ���̺귯�� ��� ����
extern void     gx_close        ( void);                                        // �׷��� ���̺귯�� ��� ����
extern dc_t    *gx_get_screen_dc( void);                                        // ȭ�� dc�� ����
extern dc_t    *gx_get_buffer_dc( int width, int height);                       // ȭ�� ����� ���� ���� DC�� ����
extern dc_t    *gx_get_compatible_dc( dc_t *dc);                                // �μ� dc�� ȣȯ�Ǵ� dc_t�� ����
extern void     gx_release_dc   ( dc_t *dc);                                    // dc �� �Ҹ�

extern color_t  gx_color        ( int red, int green, int blue, int alpha);     // Į�� ���� ����
extern void     gx_clear        ( dc_t *dc, color_t color);                     // Ư�� ������ ��ü ĥ��
extern void     gx_clear_area   ( dc_t *dc, int x1, int y1, int x2, int y2, color_t color);    // Ư�� ������ ���� ������ ��� ĥ��
extern void     gx_pen_color    ( dc_t *dc, color_t color);                     // pen Į�� ����
extern void     gx_brush_color  ( dc_t *dc, color_t color);                     // brush Į�� ����
extern void     gx_set_alpha    ( color_t *color, int alpha);                   // �������� ����
extern void     gx_get_pixel    ( dc_t *dc, int coor_x, int coor_y, color_t *color);// Į�� ���� �о� ����
extern void     gx_set_pixel    ( dc_t *dc, int coor_x, int coor_y, color_t  color);// dc�� ���� ����
extern void     gx_move_to      ( dc_t *dc, int coor_x, int coor_y);            // dc�� ��ǥ�� �̵�
extern void     gx_line_to      ( dc_t *dc, int coor_x, int coor_y);            // dc�� ��ǥ�� �� ��ǥ���� ���� �׸�
extern void     gx_line         ( dc_t *dc, int x1, int y1, int x2, int y2);    // dc�� ���� �׸�
extern void     gx_hline        ( dc_t *dc, int x_1st , int x_2nd , int coor_y, color_t color);// ���� �߱�
extern void     gx_vline        ( dc_t *dc, int coor_x, int y_1st , int y_2nd , color_t color);// ������ �߱�
extern void     gx_rectangle    ( dc_t *dc, int x1, int y1, int x2, int y2);    // dc�� �簢���� �׸�
extern void     gx_circle       ( dc_t *dc, int center_x, int center_y, int radius);// ���� ���
extern void     gx_ellipse      ( dc_t *dc, int center_x, int center_y,         // Ÿ���� ���
                                            int width,  int height);
extern void     gx_invrectangle ( dc_t *dc, int x1, int y1, int x2, int y2);    // dc���� ������ ������ ������
extern void     gx_bitblt       ( dc_t *dc_dest, int dest_x, int dest_y,        // dc ������ �ٸ� dc ������ ����
                                  dc_t *dc_sour, int sour_x, int sour_y, int sour_w, int sour_h);
extern void     gx_bitblt90     ( dc_t *dc_dest, dc_t *dc_sour);
extern void     gx_bitblt270    ( dc_t *dc_dest, dc_t *dc_sour);
extern void     gx_to_screen_dc ( dc_t *dc_screen, dc_t *dc_buffer);            // DCTYPE_SCREEN dc�� �ٸ� DCTYPE_SCREEN dc�� ��ü ����
extern void     gx_bitblt_mask  ( dc_t *dc_dest, dc_t *dc_sour, dc_t *dc_mask, color_t color, int left, int top, int width, int height);
extern dc_t    *gx_to_fast_dc( dc_t *dc);                                       // dc�� ���� ȭ�� ����� ������ dc�� ��ȯ

// ���� ������ ����� ���� �Լ�

extern frame_buffer_t 	*gx_open_frame( char *dev_name);
extern void 			 gx_close_frame( frame_buffer_t *ap_fb);
extern dc_t 			*gx_get_frame_dc( frame_buffer_t *ap_fb);

#include <gxbmp.h>
#include <gxbdf.h>
#include <gximagelist.h>
#include <gxmosaic.h>
#include <gxlayer.h>
#include <gxpanel.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

	
#endif