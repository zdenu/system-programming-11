//
//  util.cpp
//  camculator
//
//  Created by Christopher Kim on 6/11/13.
//  Copyright (c) 2013 Christopher Kim. All rights reserved.
//

#include "util.h"

namespace Util
{
	void dumpMemory(void *data, int dataSize )
    {
        printf("## dump memory size:%d address:%p\n", dataSize, data);
        
        if( NULL == data )
            return;
        
        int i = 0;
        int32_t length = dataSize;
        char *address = (char*)data;
        char *line = (char*)address;
        unsigned char ch;
        
        int dumpDataSize = length * 16;
        char* dumpData = new char[dumpDataSize];
        char temp[256];
        snprintf( dumpData, dumpDataSize, ";\n0x%08llX | ", (int64_t)address);
        
        while (length-- > 0) {
            
            snprintf( temp, sizeof(temp), "%02X ", (unsigned char)*address++);
            strcat( dumpData, temp );
            
            if( (i + 1)% 8 == 0 && (i + 1)%16 != 0 )
            {
                snprintf( temp, sizeof(temp), "  " );
                strcat( dumpData, temp );
            }
            
            if (!(++i % 16) || (length == 0 && i % 16))
            {
                if (length == 0)
                {
                    while (i++ % 16)
                    {
                        snprintf( temp, sizeof(temp), "   ");
                        strcat( dumpData, temp );
                        if( (i + 1)% 8 == 0 && (i + 1)%16 != 0 )
                        {
                            snprintf( temp, sizeof(temp), "  " );
                            strcat( dumpData, temp );
                        }
                    }
                }
                
                snprintf( temp, sizeof(temp), "|");
                strcat( dumpData, temp );
                while (line < address)
                {
                    ch = *line++;
                    snprintf( temp, sizeof(temp), "%c", (ch < 33 || ch == 255) ? 0x2E : ch);
                    strcat( dumpData, temp );
                }
                
                if (length > 0)
                {
                    snprintf( temp, sizeof(temp), "\n0x%08llX | ", (int64_t)address);
                    strcat( dumpData, temp );
                }
            }
        }
        
        snprintf( temp, sizeof(temp), "\n");
        strcat( dumpData, temp );
        
		printf("%s\n", dumpData);
		
        delete [] dumpData;
    }
		
	void PrintGifError(int ErrorCode)
	{
		char *Err = GifErrorString(ErrorCode);
		
		if (Err != NULL)
			fprintf(stderr, "GIF-LIB error: %s.\n", Err);
		else
			fprintf(stderr, "GIF-LIB undefined error %d.\n", ErrorCode);
	}
	
	void DumpScreen2RGB(char** pOutBuffer,
						int& outSize,
						ColorMapObject *ColorMap,
						GifRowType *ScreenBuffer,
						int ScreenWidth,
						int ScreenHeight)
	{
		GifRowType GifRow;
		GifColorType *ColorMapEntry;
		
		unsigned char *Buffer, *BufferP;
		
		(*pOutBuffer) = (char*)malloc(ScreenWidth * sizeof(USHORT) * ScreenHeight);
		if ((*pOutBuffer) == NULL)
			return;
		
		if ((Buffer = (unsigned char *) malloc(ScreenWidth * sizeof(USHORT))) == NULL)
			return ;
		
		for (int i = 0; i < ScreenHeight; i++)
		{
			GifRow = ScreenBuffer[i];
			BufferP = Buffer;
			
			for (int j = 0; j < ScreenWidth; j++)
			{
				ColorMapEntry = &ColorMap->Colors[GifRow[j]];
//				*BufferP++ = ColorMapEntry->Red;
//				*BufferP++ = ColorMapEntry->Green;
//				*BufferP++ = ColorMapEntry->Blue;
//				
				unsigned short pixel = MAKE_PIXEL(ColorMapEntry->Red,
										 		  ColorMapEntry->Green,
										 		  ColorMapEntry->Blue);

				memcpy(BufferP, &pixel, sizeof(unsigned short));
				BufferP += sizeof(unsigned short);
			}
			
			memcpy((*pOutBuffer) + (i * (ScreenWidth * sizeof(USHORT))), Buffer, ScreenWidth);
//			if (fwrite(Buffer, ScreenWidth, 1, f[0]) != 1)
//				return;
		}
		
		free((char *) Buffer);
	}
	
	
	bool GIF2RGB(char *FileName,
				 char** pOutBuffer,
				 int& outSize,
				 int& width,
				 int& height)
	{
		int	i, j, Size, Row, Col, Width, Height, ExtCode, Count;
		GifRecordType RecordType;
		GifByteType *Extension;
		GifRowType *ScreenBuffer;
		GifFileType *GifFile;
		int InterlacedOffset[] = { 0, 4, 2, 1 };
		int InterlacedJumps[] = { 8, 8, 4, 2 };
		int ImageNum = 0;
		ColorMapObject *ColorMap;
		
		int Error;
		if ((GifFile = DGifOpenFileName(FileName, &Error)) == NULL)
		{
			printf("cannot open gif file %s\n", FileName);
			return false;
		}

		if ((ScreenBuffer = (GifRowType *)
			 malloc(GifFile->SHeight * sizeof(GifRowType))) == NULL)
			return false;
		
		Size = GifFile->SWidth * sizeof(GifPixelType);
		if ((ScreenBuffer[0] = (GifRowType)malloc(Size)) == NULL)
			return false;
		
		for (int i = 0; i < GifFile->SWidth; ++i)
			ScreenBuffer[0][i] = GifFile->SBackGroundColor;
		
		for (int i = 1; i < GifFile->SHeight; ++i)
		{
			if ((ScreenBuffer[i] = (GifRowType) malloc(Size)) == NULL)
				return false;
			
			memcpy(ScreenBuffer[i], ScreenBuffer[0], Size);
		}
		
		do {
			if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR)
			{
				return false;
			}
			switch (RecordType)
			{
				case IMAGE_DESC_RECORD_TYPE:
					if (DGifGetImageDesc(GifFile) == GIF_ERROR)
						return false;

					Row = GifFile->Image.Top;
					Col = GifFile->Image.Left;
					Width = GifFile->Image.Width;
					Height = GifFile->Image.Height;
					
					if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
						GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight)
						return false;

					if (GifFile->Image.Interlace)
					{
						for (Count = i = 0; i < 4; i++)
						{
							for (j = Row + InterlacedOffset[i];
								 j < Row + Height;
								 j += InterlacedJumps[i])
							{
								if (DGifGetLine(GifFile, &ScreenBuffer[j][Col],
												Width) == GIF_ERROR)
									return false;
							}
						}
					}
					else
					{
						for (i = 0 ; i < Height ; i++)
						{
							if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col],
											Width) == GIF_ERROR)
								return false;
						}
					}
					break;
				case EXTENSION_RECORD_TYPE:
					if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR)
						return false;
					while (Extension != NULL)
					{
						if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR)
							return false;
					}
					break;
				case TERMINATE_RECORD_TYPE:
					break;
				default:		    /* Should be trapped by DGifGetRecordType. */
					break;
			}
		} while (RecordType != TERMINATE_RECORD_TYPE);
		
		/* Lets dump it - set the global variables required and do it: */
		ColorMap = (GifFile->Image.ColorMap
					? GifFile->Image.ColorMap
					: GifFile->SColorMap);
		if (ColorMap == NULL)
		{
			printf("there is no gif color map.\n");
			return false;
		}
		
		DumpScreen2RGB(pOutBuffer,
					   outSize,
					   ColorMap,
					   ScreenBuffer,
					   GifFile->SWidth,
					   GifFile->SHeight);
		
		(void)free(ScreenBuffer);
		
		if (DGifCloseFile(GifFile) == GIF_ERROR)
		{
			printf("gif close error.");
			return false;
		}
		
		return true;
	}
	
};