#ifndef _GX_IMAGE_LIST_H_
#define _GX_IMAGE_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gx.h>
#include <gxpng.h>

typedef struct image_list_t_ image_list_t;
struct image_list_t_
{
   int   count;
   
   dc_t **array;
};

extern image_list_t  *gx_imagelist_create( char  *filename, int width, int height); // png ������ �̿��Ͽ� �̹��� ����Ʈ ����
extern dc_t *gx_imagelist_get( image_list_t *list, int index);                 // �ε��� ��ȣ�� �ش��ϴ� �̹��� ���� ��ȯ
extern void gx_imagelist_close( image_list_t *list);                            // �̹��� ����Ʈ ��ü �Ҹ�
	
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif